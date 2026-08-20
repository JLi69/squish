#include "game.hpp"
#include "app.hpp"
#include <set>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

glm::mat4 Camera2D::getMat() const {
	glm::mat4 camMat = glm::mat4(1.0f);
	camMat = glm::scale(camMat, glm::vec3(zoom));
	camMat = glm::translate(camMat, glm::vec3(-pos, 0.0f));
	return camMat;
}

void Camera2D::follow(float dt, glm::vec2 followPos) {
	float dist = glm::length(followPos - pos);
	if(dist < MIN_FOLLOW_DIST)
		return;
	glm::vec2 dir = glm::normalize(followPos - pos);
	pos += dir * std::min(dt * followSpeed * dist, dist);
}

Level &Game::getLevel() {
	return level;
}

Player &Game::getPlayer() {
	return player;
}

void Game::initTestLevel() {
	player = Player(0, 0);
	level = genTestLevel();
	tileVaos = getTileMapVaos(level);
}

void Game::initCaveLevel() {
	player = Player(0, 0);
	std::random_device rd;
	level = genCaveLevel(rd());
	tileVaos = getTileMapVaos(level);
}

void Game::movePlayer() {
	int dx = 0,
		dy = 0;
	// Move the player
	if(getKeyInputState(GLFW_KEY_UP) == JUST_PRESSED)
		dy++;
	if(getKeyInputState(GLFW_KEY_DOWN) == JUST_PRESSED)
		dy--;
	Tile aboveTile = level.getWallTile(player.x, player.y + dy);
	if(!canPush(level, player.x, player.y + dy, dx, dy) && !aboveTile.isEmpty())
		dy = 0;
	if(dy == 0) {
		if(getKeyInputState(GLFW_KEY_RIGHT) == JUST_PRESSED)
			dx++;
		if(getKeyInputState(GLFW_KEY_LEFT) == JUST_PRESSED)
			dx--;
	}

	if(dx < 0)
		player.sprite.flip = true;
	else if(dx > 0)
		player.sprite.flip = false;

	player.x += dx;
	player.y += dy;
}

bool Game::pushBlocks(int prevx, int prevy, int &x, int &y) {
	int dirx = x - prevx, 
		diry = y - prevy;
	if(dirx == 0 && diry == 0)
		return false;
	if(canPush(level, x, y, dirx, diry) && !level.isBlocked(x, y)) {
		Tile tile = level.getWallTile(x, y);
		level.addPushedBlock(x, y, dirx, diry, level.getWallTile(x, y));
		level.setWallTile(x, y, Tile());
		
		// Mark which tile map vaos should be updated
		std::set<std::pair<int, int>> toUpdate;
		for(int dx = -1; dx <= 1; dx++) {
			for(int dy = -1; dy <= 1; dy++) {
				int chunkx = tileToChunkCoord(x + dx * 3);
				int chunky = tileToChunkCoord(y + dy * 3);
				toUpdate.insert({ chunkx, chunky });

				chunkx = tileToChunkCoord(x + dirx + dx * 3);
				chunky = tileToChunkCoord(y + diry + dy * 3);
				toUpdate.insert({ chunkx, chunky });
			}
		}
		
		for(const auto &chunk : toUpdate)
			chunksToUpdate.push(chunk);
	}
	else if(!level.getWallTile(x, y).isEmpty()) {
		x = prevx;
		y = prevy;
		return false;
	}

	return true;
}

void Game::update(float dt) {
	player.update(dt);
	// Update player
	if(!player.translationAnimationActive) {
		int prevx = player.x, prevy = player.y;
		movePlayer();
		if(pushBlocks(prevx, prevy, player.x, player.y))
			player.activateTranslationAnimation(prevx, player.x, prevy, player.y);
	}
	std::set<std::pair<int ,int>> pushTileChunkUpdateList;
	level.updatePushedTiles(dt, pushTileChunkUpdateList);
	for(const auto &chunkpos : pushTileChunkUpdateList)
		chunksToUpdate.push(chunkpos);
	
	camera.follow(dt, player.getDisplayPos());
}

void Game::updateChunkVaos() {
	std::set<std::pair<int, int>> updated;
	while(!chunksToUpdate.empty()) {
		std::pair<int, int> top = chunksToUpdate.top();
		// Skip over any chunks that we already updated
		if(updated.count(top)) {
			chunksToUpdate.pop();
			continue;
		}
		int chunkx = top.first, 
			chunky = top.second;
		updateTileMapVaos(tileVaos, level, chunkx, chunky);
		chunksToUpdate.pop();
		updated.insert(top);
	}
}

bool canPush(const Level &level, int x, int y, int dirx, int diry) {
	if(level.isBlocked(x + dirx, y + diry))
		return false;
	if(!level.getWallTile(x, y).canPush)
		return false;
	if(!level.getWallTile(x + dirx, y + diry).isEmpty())
		return false;
	return true;
}

const TileVaos &Game::getTileVaos() const {
	return tileVaos;
}

Camera2D &Game::getCamera() {
	return camera;
}
