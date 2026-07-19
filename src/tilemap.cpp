#include "tilemap.hpp"
#include <cassert>

std::unordered_map<uint16_t, glm::vec2> Tile::textureOffsets;
std::unordered_map<std::string, uint16_t> Tile::nameToId;

Tile::Tile() {
	tileId = 0;
}

Tile::Tile(uint16_t id) {
	tileId = id;
}

bool Tile::isEmpty() const {
	return tileId == 0;
}

void Tile::initNameToId() {
	nameToId["brick_wall"] = 1;
	nameToId["tile_floor"] = 2;
	nameToId["crate"] = 3;
}

void Tile::initTextureOffsets() {
	textureOffsets[1] = glm::vec2(1.0f, 0.0f);
	textureOffsets[2] = glm::vec2(2.0f, 0.0f);
	textureOffsets[3] = glm::vec2(3.0f, 0.0f);
}

glm::vec2 Tile::getTexOffset() const {
	if(!textureOffsets.count(tileId))
		return glm::vec2(0.0f, 0.0f);
	return textureOffsets.at(tileId);
}

TileMap::TileMap(int lx, int by, int rx, int ty) {
	assert(lx <= rx);
	assert(by <= ty);

	leftx = lx;
	bottomy = by;

	width = rx - lx + 1;
	height = ty - by + 1;

	tiles = std::vector<Tile>(width * height);
}

Tile TileMap::getTile(int x, int y) const {
	// Translate the coordinates to be relative to the bottom left
	int relativeX = x - leftx;
	int relativeY = y - bottomy;

	// Out of bounds, return an emtpy tile
	if(relativeX < 0 || relativeY < 0 || relativeX >= width || relativeY >= height)
		return Tile();

	return tiles.at(relativeY * width + relativeX);
}

void TileMap::setTile(int x, int y, Tile tile) {
	// Translate the coordinates to be relative to the bottom left
	int relativeX = x - leftx;
	int relativeY = y - bottomy;

	// Out of bounds, return an emtpy tile
	if(relativeX < 0 || relativeY < 0 || relativeX >= width || relativeY >= height)
		return;

	tiles.at(relativeY * width + relativeX) = tile;
}

int TileMap::getLeftX() const {
	return leftx;
}

int TileMap::getRightX() const {
	return leftx + width - 1;
}

int TileMap::getBottomY() const {
	return bottomy;
}

int TileMap::getTopY() const {
	return bottomy + height - 1;
}

int TileMap::getWidth() const {
	return width;
}

int TileMap::getHeight() const {
	return height;
}
