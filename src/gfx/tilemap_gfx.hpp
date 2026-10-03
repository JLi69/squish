#pragma once

#include "level.hpp"
#include "gfx.hpp"
#include <map>

const float CHUNK_SIZE = 32;

const float FLOOR_SHADING = 0.8f;
const float FLOOR_WALL_SHADING = 0.6f;
const float WALL_HEIGHT = 0.6f;
const float WALL_TOP_SHADING = 0.6f;
const float WALL_BOT_SHADING = 0.4f;

typedef std::map<std::pair<int, int>, gfx::Vao> TileVaos;

int tileToChunkCoord(int coord);

mesh::ElementArrayBuffer<float> getTileMapVertDataForChunk(
	const Level &level,
	int chunkx,
	int chunky
);

TileVaos getTileMapVaos(const Level &level);
void updateTileMapVaos(TileVaos &vaos, const Level &level, int chunkx, int chunky);
gfx::Vao genWallTileVao(Tile tile);
