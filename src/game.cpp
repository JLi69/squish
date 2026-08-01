#include "game.hpp"
#include "app.hpp"
#include <set>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

const float DEFAULT_PLAYER_SCALE = 1.1f;
const glm::vec2 DEFAULT_PLAYER_OFFSET = glm::vec2(0.0f, 0.4f);

Sprite::Sprite(const std::string &texId, glm::vec2 pos) {
	spriteTexId = texId;
	offset = pos;
}

Player::Player(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("player", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.2f, 0.5f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.1f);

	sprite.offset = DEFAULT_PLAYER_OFFSET;
	sprite.scale = glm::vec2(DEFAULT_PLAYER_SCALE, DEFAULT_PLAYER_SCALE);

	squishyAnimation = AnimationValue(
		DEFAULT_PLAYER_SCALE - 0.05f,
		DEFAULT_PLAYER_SCALE + 0.05f,
		PLAYER_SQUISH_ANIMATION_LENGTH
	);
	squishyAnimation.loop = true;
}

void Player::update(float dt) {
	if(translationAnimationActive) {
		translateX.update(dt);
		translateY.update(dt);
	}
	if(translateX.time >= 1.0f || translateY.time >= 1.0f)
		translationAnimationActive = false;
	
	squishyAnimation.update(dt);
	sprite.scale.y = squishyAnimation.value();
	sprite.offset.y = DEFAULT_PLAYER_OFFSET.y + (sprite.scale.y - DEFAULT_PLAYER_SCALE) / 2.0f;
}

void Player::activateTranslationAnimation(float x1, float x2, float y1, float y2) {
	translateX = AnimationValue(x1, x2, TRANSLATION_ANIMATION_LENGTH);
	translateY = AnimationValue(y1, y2, TRANSLATION_ANIMATION_LENGTH);
	translationAnimationActive = true;
}

glm::vec2 Player::getDisplayPos() const {
	if(translationAnimationActive)
		return glm::vec2(translateX.value(), translateY.value());
	return glm::vec2(float(x), float(y));
}

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
				int chunkx = tileToChunkCoord(x + dx);
				int chunky = tileToChunkCoord(y + dy);
				toUpdate.insert({ chunkx, chunky });

				chunkx = tileToChunkCoord(x + dirx + dx);
				chunky = tileToChunkCoord(y + diry + dy);
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
