#include "game.hpp"
#include "app.hpp"
#include <set>

Sprite::Sprite(const std::string &texId, glm::vec2 pos) {
	spriteTexId = texId;
	offset = pos;
}

Player::Player(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("player", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.4f, 0.7f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.1f);

	sprite.offset = glm::vec2(0.0f, 0.4f);
	sprite.scale = glm::vec2(1.2f, 1.2f);
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

void Game::movePlayer() {
	int dx = 0,
		dy = 0;
	// Move the player
	if(getKeyInputState(GLFW_KEY_UP) == JUST_PRESSED)
		dy++;
	if(getKeyInputState(GLFW_KEY_DOWN) == JUST_PRESSED)
		dy--;
	if(getKeyInputState(GLFW_KEY_RIGHT) == JUST_PRESSED)
		dx++;
	if(getKeyInputState(GLFW_KEY_LEFT) == JUST_PRESSED)
		dx--;

	if(dx < 0)
		player.sprite.flip = true;
	else if(dx > 0)
		player.sprite.flip = false;

	player.x += dx;
	player.y += dy;
}

void Game::pushBlocks(int prevx, int prevy, int &x, int &y) {
	int dirx = x - prevx, 
		diry = y - prevy;
	if(canPush(level, x, y, dirx, diry)) {
		Tile tile = level.getWallTile(x, y);
		level.setWallTile(x, y, Tile());
		level.setWallTile(x + dirx, y + diry, tile);
		
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
	}
}

void Game::update(float dt) {
	// Update player
	int prevx = player.x, prevy = player.y;
	movePlayer();
	pushBlocks(prevx, prevy, player.x, player.y);
}

void Game::updateChunkVaos() {
	while(!chunksToUpdate.empty()) {
		std::pair<int, int> top = chunksToUpdate.top();
		int chunkx = top.first, 
			chunky = top.second;
		updateTileMapVaos(tileVaos, level, chunkx, chunky);
		chunksToUpdate.pop();
	}
}

bool canPush(const Level &level, int x, int y, int dirx, int diry) {
	if(!level.getWallTile(x, y).canPush)
		return false;
	if(!level.getWallTile(x + dirx, y + diry).isEmpty())
		return false;
	return true;
}

const TileVaos &Game::getTileVaos() const {
	return tileVaos;
}
