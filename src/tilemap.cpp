#include "tilemap.hpp"
#include "importfile.hpp"
#include "string_utils.hpp"
#include <cassert>

std::unordered_map<uint16_t, glm::vec2> Tile::textureOffsets;
static std::unordered_map<std::string, Tile> nameToTile;

Tile tile(const std::string &name) {
	// Return an empty tile if no such tile name was found
	if(!nameToTile.count(name))
		return Tile();
	return nameToTile.at(name);
}

Tile::Tile() {
	tileId = 0;
}

Tile::Tile(uint16_t id) {
	tileId = id;
}

bool Tile::isEmpty() const {
	return tileId == 0;
}

/*
 * Takes in a string (idStr) that is read from an impfile that should be the
 * string version of an integer and parses that value into an integer (id) that
 * can be used to identify a type of tile.
 * Returns true upon success, false otherwise
 */
static bool parseId(const std::string &idStr, const std::string &tileName, uint16_t &id) {
	try {
		id = std::stoul(idStr);
	}
	catch(std::invalid_argument const &exc) {
		fprintf(
			stderr,
			"ERROR: failed to parse id for '%s': %s",
			tileName.c_str(), 
			exc.what()
		);
		return false;
	}
	catch(std::out_of_range const &exc) {
		fprintf(
			stderr,
			"ERROR: failed to parse id for '%s': %s",
			tileName.c_str(), 
			exc.what()
		);
		return false;
	}

	return true;
}

/*
 * Takes in a string (texCoordStr) that is expected to be of the format
 * "X, Y" and then parses that value into a vec2 (texCoord) that can be used
 * as an offset for a tile's texture coordinates.
 * Returns true upon success, false otherwise
 */
static bool parseTexCoords(
	const std::string &texCoordStr, 
	const std::string &tileName,
	glm::vec2 &texCoord
) {
	std::vector<std::string> texCoordValues = str_utils::split(texCoordStr, ",");
	if(texCoordValues.size() != 2) {
		fprintf(
			stderr, 
			"ERROR: Found %d values in '%s', expected 2",
			(int)texCoordValues.size(),
			tileName.c_str()
		);
		return false;
	}

	try {
		texCoord.x = std::stof(texCoordValues[0]);
		texCoord.y = std::stof(texCoordValues[1]);
	}
	catch(std::invalid_argument const &exc) {
		fprintf(
			stderr,
			"ERROR: failed to parse tex_coords for '%s': %s",
			tileName.c_str(), 
			exc.what()
		);
		return false;
	}
	catch(std::out_of_range const &exc) {
		fprintf(
			stderr,
			"ERROR: failed to parse tex_coords for '%s': %s",
			tileName.c_str(), 
			exc.what()
		);
		return false;
	}

	return true;
}

void Tile::initTiles(const char *path) {
	std::vector<impfile::Entry> entries = impfile::parseFile(path);

	for(const auto &entry : entries) {
		// Attempt to parse the tile id
		std::string idStr = entry.getVar("id");
		uint16_t id = 0;
		if(!parseId(idStr, entry.name, id))
			continue;

		// Attempt to parse the texture coordinates
		glm::vec2 texCoord(0.0f, 0.0f);
		std::string texCoordStr = entry.getVar("tex_coords");
		parseTexCoords(texCoordStr, entry.name, texCoord);
		textureOffsets[id] = texCoord;

		Tile tile = Tile(id);
		std::vector<std::string> tags = str_utils::split(entry.getVar("tags"), ",");
		for(const auto &tag : tags) {
			std::string trimmedTag = str_utils::trim(tag);
			if(trimmedTag == "can_push")
				tile.canPush = true;
		}
		nameToTile[entry.name] = tile;
	}
}

glm::vec2 Tile::getTexOffset() const {
	if(!textureOffsets.count(tileId))
		return glm::vec2(0.0f, 0.0f);
	return textureOffsets.at(tileId);
}

TileMap::TileMap() {
	leftx = 0;
	bottomy = 0;
	width = 0;
	height = 0;
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

void TileMap::setTile(int x, int y, const std::string &tileName) {
	setTile(x, y, tile(tileName));
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
