#include "game.hpp"
#include "app.hpp"
#include <set>
#include <glm/gtc/matrix_transform.hpp>
#include "generate_level.hpp"
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
	GeneratedLevel genLevel = genTestLevel();
	level = genLevel.level;
	enemies = std::move(genLevel.enemies);
	tileVaos = getTileMapVaos(level);
}

void Game::initCaveLevel() {
	player = Player(0, 0);
	std::random_device rd;
	GeneratedLevel genLevel = genCaveLevel(rd());
	level = genLevel.level;
	enemies = std::move(genLevel.enemies);
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
		for(auto &enemy : enemies) {
			if(enemy == nullptr)
				continue;
			if(enemy->x == x + dirx && enemy->y == y + diry)
				enemy->setDir(dirx, diry);
		}

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

	for(auto &enemy : enemies) {
		if(enemy == nullptr)
			continue;

		if(enemy->isInsideTile(level)) {
			// Enemy got squished
			if(!enemy->uncollide(level))
				enemy.reset();
			continue;
		}

		enemy->update(dt);
		if(!enemy->isInsideTile(level) && enemy->updateMoveTimer(dt)) {
			int prevx = enemy->x;
			int prevy = enemy->y;
			enemy->moveEnemy(level);
			if(enemy->isInsideTile(level)) {
				enemy->translationAnimationActive = false;
				enemy->x = prevx;
				enemy->y = prevy;
			}
		}
		enemy->updateDir(level, player);	
	}

	camera.follow(dt, player.getDisplayPos());

	clearEnemyList();
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
	if(level.isBlocked(x + dirx, y + diry) || level.isBlocked(x, y))
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

EnemyList &Game::getEnemies() {
	return enemies;
}

void Game::clearEnemyList() {
	const int MAX_REMOVE = 32;
	int removed = 0;
	while(removed < MAX_REMOVE) {
		int index = -1;
		for(int i = 0; i < enemies.size(); i++) {
			if(enemies.at(i) == nullptr) {
				index = i;
				break;
			}
		}
		if(index < 0)
			break;
		enemies.at(index) = std::move(enemies.at(enemies.size() - 1));
		enemies.pop_back();
		removed++;
	}

	if(removed > 0)
		fprintf(stderr, "DEBUG: removed %d enemies.\n", removed);
}
