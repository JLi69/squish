#pragma once

#include "level.hpp"
#include "gfx.hpp"
#include <map>

const float CHUNK_SIZE = 32;

typedef std::map<std::pair<int, int>, gfx::Vao> TileVaos;

int tileToChunkCoord(int coord);

mesh::ElementArrayBuffer<float> getTileMapVertDataForChunk(
	const Level &level,
	int chunkx,
	int chunky
);

TileVaos getTileMapVaos(const Level &level);
void updateTileMapVaos(TileVaos &vaos, const Level &level, int chunkx, int chunky);
