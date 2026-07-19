#include "level.hpp"

Level::Level(int lx, int by, int rx, int ty) {
	floorTiles = TileMap(lx, by, rx, ty);
	wallTiles = TileMap(lx, by, rx, ty);
}

void Level::setFloorTile(int x, int y, Tile tile) {
	floorTiles.setTile(x, y, tile);
}

Tile Level::getFloorTile(int x, int y) const {
	return floorTiles.getTile(x, y);
}

void Level::setWallTile(int x, int y, Tile tile) {
	wallTiles.setTile(x, y, tile);
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

Level genTestLevel() {
	Level testLevel(-8, -8, 8, 8);

	// Test tiles
	Tile wallTile = Tile(1);
	Tile floorTile = Tile(2);
	Tile crateTile = Tile(3);
	crateTile.canPush = true;

	// Generate walls and floors
	for(int x = -4; x <= 4; x++) {
		for(int y = -4; y <= 4; y++) {
			testLevel.setFloorTile(x, y, floorTile);
			if((x >= -2 && x <= 0) && y == 2) {
				testLevel.setWallTile(x, y, wallTile);
				continue;
			}
			if(abs(x) == 4 || abs(y) == 4) {
				testLevel.setWallTile(x, y, wallTile);
				continue;
			}
		}
	}
	testLevel.setWallTile(2, -2, wallTile);

	// Add a pushable crate
	testLevel.setWallTile(-2, -2, crateTile);

	return testLevel;
}
