#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <glm/glm.hpp>
#include <string>

struct Tile {	
	uint16_t tileId;
	bool canPush = false;

	static std::unordered_map<uint16_t, glm::vec2> textureOffsets;

	// Default constructor - returns an empty tile
	Tile();
	// Constructor to create a tile from a given id value
	Tile(uint16_t id);

	// Returns whether the tile is completely empty (has an id of 0)
	bool isEmpty() const;

	static void initTiles(const char *path);
	glm::vec2 getTexOffset() const;
};

class TileMap {
	// An array for storing the tile data
	std::vector<Tile> tiles;
	// The coordinates of the top left corner of the tile map
	int leftx, bottomy;
	// The width and height of the tile map
	int width, height;
public:
	// Default constructor
	TileMap();

	/*
	 * Creates an empty tile map with the top left and bottom right bounds given
	 * lx, by: the coordinates of the bottom left corner (inclusive)
	 * rx, ty: the coordinates of the top right corner (inclusive)
	 * We assume that lx < rx and by < ty
	 */
	TileMap(int lx, int by, int rx, int ty);

	/*
	 * Returns the tile at the specified position.
	 * If the coordinates are out of bounds, return an empty tile
	 */
	Tile getTile(int x, int y) const;

	/*
	 * Updates a tile at a given position
	 * If the coordinates are out of bounds, this method does nothing
	 */
	void setTile(int x, int y, Tile tile);
	void setTile(int x, int y, const std::string &tileName);

	int getLeftX() const;
	int getRightX() const;
	int getBottomY() const;
	int getTopY() const;

	int getWidth() const;
	int getHeight() const;
};

Tile tile(const std::string &name);
