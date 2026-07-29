#pragma once

#include "tilemap.hpp"
#include "gfx.hpp"
#include "animation.hpp"
#include <map>
#include <set>

struct PushedTile {
	// Vao that should be displayed for this tile
	gfx::Vao vao;	
	AnimationValue translateX, translateY;
	int destinationX, destinationY;
	Tile tile;
	PushedTile(int startx, int starty, int dirx, int diry, Tile t);
};

class Level {
	TileMap floorTiles;
	TileMap wallTiles;
	std::map<std::pair<int, int>, PushedTile> pushedTiles;
public:
	// Constructor
	Level(int lx, int by, int rx, int ty);

	void setFloorTile(int x, int y, Tile tile);
	void setFloorTile(int x, int y, const std::string tileName);
	Tile getFloorTile(int x, int y) const;

	void setWallTile(int x, int y, Tile tile);
	void setWallTile(int x, int y, const std::string tileName);
	Tile getWallTile(int x, int y) const;

	int getLeftX() const;
	int getRightX() const;
	int getBottomY() const;
	int getTopY() const;

	int getWidth() const;
	int getHeight() const;

	void addPushedBlock(int x, int y, int dirx, int diry, Tile tile);
	void clearBlocked(int x, int y);
	bool isBlocked(int x, int y) const;
	void updatePushedTiles(float dt, std::set<std::pair<int ,int>> &chunksToUpdate);
	const std::map<std::pair<int, int>, PushedTile> &getPushedTiles() const;
};

// For debug purposes: generates test level
Level genTestLevel();
