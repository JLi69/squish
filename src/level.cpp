#include "level.hpp"
#include "tilemap_gfx.hpp"

PushedTile::PushedTile(int startx, int starty, int dirx, int diry, Tile t) {
	translateX = AnimationValue(
		float(startx), 
		float(startx + dirx), 
		TRANSLATION_ANIMATION_LENGTH
	);
	translateY = AnimationValue(
		float(starty),
		float(starty + diry),
		TRANSLATION_ANIMATION_LENGTH
	);

	destinationX = startx + dirx;
	destinationY = starty + diry;

	tile = t;
}

Level::Level(int lx, int by, int rx, int ty) {
	floorTiles = TileMap(lx, by, rx, ty);
	wallTiles = TileMap(lx, by, rx, ty);
}

void Level::setFloorTile(int x, int y, Tile tile) {
	floorTiles.setTile(x, y, tile);
}

void Level::setFloorTile(int x, int y, const std::string tileName) {
	floorTiles.setTile(x, y, tileName);
}

Tile Level::getFloorTile(int x, int y) const {
	return floorTiles.getTile(x, y);
}

void Level::setWallTile(int x, int y, Tile tile) {
	wallTiles.setTile(x, y, tile);
}

void Level::setWallTile(int x, int y, const std::string tileName) {
	wallTiles.setTile(x, y, tileName);
}

Tile Level::getWallTile(int x, int y) const {
	return wallTiles.getTile(x, y);
}

int Level::getLeftX() const {
	return std::min(floorTiles.getLeftX(), wallTiles.getLeftX());
}

int Level::getRightX() const {
	return std::max(floorTiles.getRightX(), wallTiles.getRightX());
}

int Level::getBottomY() const {
	return std::min(floorTiles.getBottomY(), wallTiles.getBottomY());
}

int Level::getTopY() const {
	return std::max(floorTiles.getTopY(), wallTiles.getTopY());
}

void Level::addPushedBlock(int x, int y, int dirx, int diry, Tile tile) {
	PushedTile pushedTile = PushedTile(x, y, dirx, diry, tile);
	pushedTile.vao = genWallTileVao(tile);
	pushedTiles.insert({{ x, y }, pushedTile });
}

void Level::clearBlocked(int x, int y) {
	const PushedTile &pushedTile = pushedTiles.at({ x, y });
	glDeleteVertexArrays(1, &pushedTile.vao.vaoid);
	glDeleteBuffers(pushedTile.vao.buffers.size(), &pushedTile.vao.buffers[0]);
	pushedTiles.erase({ x, y });
}

bool Level::isBlocked(int x, int y) const {
	return pushedTiles.count({ x, y }) > 0;
}

void Level::updatePushedTiles(float dt, std::set<std::pair<int ,int>> &chunksToUpdate) {
	std::vector<std::pair<int, int>> toClear;
	for(auto &pair : pushedTiles) {
		PushedTile &pushedTile = pair.second;
		pushedTile.translateX.update(dt);
		pushedTile.translateY.update(dt);
		if(pushedTile.translateX.time >= 1.0f || pushedTile.translateY.time >= 1.0f) {
			std::pair<int, int> pos = pair.first;
			toClear.push_back(pos);
			setWallTile(pushedTile.destinationX, pushedTile.destinationY, pushedTile.tile);

			for(int dx = -1; dx <= 1; dx++) {
				for(int dy = -1; dy <= 1; dy++) {
					std::pair<int, int> chunkpos;
					chunkpos.first = tileToChunkCoord(pushedTile.destinationX + dx);
					chunkpos.second = tileToChunkCoord(pushedTile.destinationY + dy);
					chunksToUpdate.insert(chunkpos);
				}
			}	
		}
	}

	for(const auto &pos : toClear) {
		clearBlocked(pos.first, pos.second);
	}
}

const std::map<std::pair<int, int>, PushedTile>& Level::getPushedTiles() const {
	return pushedTiles;
}
