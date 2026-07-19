#pragma once

#include "tilemap.hpp"

class Level {
	TileMap floorTiles;
	TileMap wallTiles;
public:
	// Constructor
	Level(int lx, int by, int rx, int ty);

	void setFloorTile(int x, int y, Tile tile);
	Tile getFloorTile(int x, int y) const;

	void setWallTile(int x, int y, Tile tile);
	Tile getWallTile(int x, int y) const;

	int getLeftX() const;
	int getRightX() const;
	int getBottomY() const;
	int getTopY() const;

	int getWidth() const;
	int getHeight() const;
};

// For debug purposes: generates test level
Level genTestLevel();
