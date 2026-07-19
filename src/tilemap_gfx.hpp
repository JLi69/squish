#pragma once

#include "tilemap.hpp"
#include "gfx.hpp"
#include <map>

const float CHUNK_SIZE = 32;

typedef std::map<std::pair<int, int>, gfx::Vao> TileVaos;

int tileToChunkCoord(int coord);

mesh::ElementArrayBuffer<float> getTileMapVertDataForChunk(
	const TileMap &tilemap,
	int chunkx,
	int chunky
);

TileVaos getTileMapVaos(const TileMap &tilemap);
void updateTileMapVaos(
	TileVaos &vaos,
	const TileMap &tilemap,
	int chunkx,
	int chunky
);
