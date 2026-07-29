#include "tilemap_gfx.hpp"

const unsigned int INDICES[] = {
	0, 1, 2,
	1, 2, 3,
};

int tileToChunkCoord(int coord) {
	if(coord >= 0)
		return coord / CHUNK_SIZE;
	// Coord is negative, subtract 1
	return coord / CHUNK_SIZE - 1;
}

static void addTileQuadToBuffer(
	mesh::ElementArrayBuffer<float> &vertexData,
	float x,
	float y,
	float z,
	float height,
	glm::vec2 texOffset,
	float shadingTop,
	float shadingBot
) {
	glm::vec2 texBotLeft = texOffset + glm::vec2(0.01f, 0.01f);

	mesh::addToMesh(vertexData.mesh, glm::vec3(x, y, z));
	mesh::addToMesh(vertexData.mesh, texBotLeft);
	mesh::addToMesh(vertexData.mesh, shadingBot);
	
	mesh::addToMesh(vertexData.mesh, glm::vec3(x + 1.0f, y, z));
	mesh::addToMesh(vertexData.mesh, texBotLeft + glm::vec2(0.98f, 0.0f));
	mesh::addToMesh(vertexData.mesh, shadingBot);
	
	mesh::addToMesh(vertexData.mesh, glm::vec3(x, y + height, z));
	mesh::addToMesh(vertexData.mesh, texBotLeft + glm::vec2(0.0f, 0.98f));
	mesh::addToMesh(vertexData.mesh, shadingTop);

	mesh::addToMesh(vertexData.mesh, glm::vec3(x + 1.0f, y + height, z));
	mesh::addToMesh(vertexData.mesh, texBotLeft + glm::vec2(0.98f, 0.98f));
	mesh::addToMesh(vertexData.mesh, shadingTop);
}

mesh::ElementArrayBuffer<float> getLevelVertDataForChunk(
	const Level &level,
	int chunkx,
	int chunky
) {
	mesh::ElementArrayBuffer<float> vertexData;

	unsigned int quadcount = 0;
	// Add floor
	for(int tx = 0; tx < CHUNK_SIZE; tx++) {
		for(int ty = 0; ty < CHUNK_SIZE; ty++) {
			int tilePosX = tx + chunkx * CHUNK_SIZE;
			int tilePosY = ty + chunky * CHUNK_SIZE;
			Tile tile = level.getFloorTile(tilePosX, tilePosY);
			if(tile.isEmpty())
				continue;
			if(!level.getWallTile(tilePosX, tilePosY).isEmpty())
				continue;

			float x = float(tx);
			float y = float(ty);
			// Check the above tile to make sure that it is a wall, so that the
			// floor tile can be shaded appropriately
			Tile aboveTile = level.getWallTile(tilePosX, tilePosY + 1);
			glm::vec2 texOffset = tile.getTexOffset();
			if(!aboveTile.isEmpty())
				addTileQuadToBuffer(vertexData, x, y, 1.0f, 1.0f, texOffset, FLOOR_WALL_SHADING, FLOOR_SHADING);
			else
				addTileQuadToBuffer(vertexData, x, y, 1.0f, 1.0f, texOffset, FLOOR_SHADING, FLOOR_SHADING);

			for(int i = 0; i < 6; i++)
				vertexData.indices.push_back(4 * quadcount + INDICES[i]);
			quadcount++;
		}
	}

	// Add walls
	for(int tx = 0; tx < CHUNK_SIZE; tx++) {
		for(int ty = 0; ty < CHUNK_SIZE; ty++) {
			int tilePosX = tx + chunkx * CHUNK_SIZE;
			int tilePosY = ty + chunky * CHUNK_SIZE;
			Tile tile = level.getWallTile(tilePosX, tilePosY);
			if(tile.isEmpty())
				continue;

			float x = float(tx);
			float y = float(ty);
			float ycoord = y + float(chunky * CHUNK_SIZE);
			float z = (ycoord - level.getTopY()) / float(level.getTopY() - level.getBottomY());
			glm::vec2 texOffset = tile.getTexOffset();
			addTileQuadToBuffer(vertexData, x, y, z, WALL_HEIGHT, texOffset, WALL_TOP_SHADING, WALL_BOT_SHADING);
			for(int i = 0; i < 6; i++)
				vertexData.indices.push_back(4 * quadcount + INDICES[i]);
			quadcount++;
		}
	}

	for(int tx = 0; tx < CHUNK_SIZE; tx++) {
		for(int ty = 0; ty < CHUNK_SIZE; ty++) {
			int tilePosX = tx + chunkx * CHUNK_SIZE;
			int tilePosY = ty + chunky * CHUNK_SIZE;
			Tile tile = level.getWallTile(tilePosX, tilePosY);
			if(tile.isEmpty())
				continue;

			float x = float(tx);
			float y = float(ty) + WALL_HEIGHT;
			float ycoord = float(ty) + float(chunky * CHUNK_SIZE);
			float z = (ycoord - level.getTopY()) / float(level.getTopY() - level.getBottomY());
			glm::vec2 texOffset = tile.getTexOffset();
			addTileQuadToBuffer(vertexData, x, y, z, 1.0f, texOffset, 1.0f, 1.0f);
			for(int i = 0; i < 6; i++)
				vertexData.indices.push_back(4 * quadcount + INDICES[i]);
			quadcount++;
		}
	}

	return vertexData;
}

// Assumes buffers.size() >= 4
static void levelDataToBuffers(
	const std::vector<unsigned int> &buffers,
	const mesh::ElementArrayBuffer<float> &vertices
) {
	// vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, buffers.at(0));	
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture offset
	glBindBuffer(GL_ARRAY_BUFFER, buffers.at(1));
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// shading
	glBindBuffer(GL_ARRAY_BUFFER, buffers.at(2));	
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
	glVertexAttribPointer(2, 1, GL_FLOAT, false, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.at(3));
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		vertices.indices.size() * sizeof(unsigned int),
		&vertices.indices[0],
		GL_STATIC_DRAW
	);
}

TileVaos getTileMapVaos(const Level &level) {
	TileVaos tilevaos = TileVaos();

	int startChunkX = tileToChunkCoord(level.getLeftX());
	int endChunkX = tileToChunkCoord(level.getRightX());
	int startChunkY = tileToChunkCoord(level.getBottomY());
	int endChunkY = tileToChunkCoord(level.getTopY());

	for(int chunkx = startChunkX; chunkx <= endChunkX; chunkx++) {
		for(int chunky = startChunkY; chunky <= endChunkY; chunky++) {
			mesh::ElementArrayBuffer<float> vertices 
				= getLevelVertDataForChunk(level, chunkx, chunky);
			// Ignore empty vertex chunks
			if(vertices.mesh.vertices.empty())
				continue;

			std::pair<int, int> coords = { chunkx, chunky };
			
			gfx::Vao vao;
			vao.genBuffers(4);
			glBindVertexArray(vao.vaoid);
			levelDataToBuffers(vao.buffers, vertices);
			vao.vertcount = vertices.indices.size();
			glBindVertexArray(0);

			tilevaos.insert({ coords, vao });
		}
	}

	return tilevaos;
}

void updateTileMapVaos(
	TileVaos &vaos,
	const Level &level,
	int chunkx,
	int chunky
) {
	mesh::ElementArrayBuffer<float> vertices 
		= getLevelVertDataForChunk(level, chunkx, chunky);

	// No vertices, and it doesn't exist in the vao list, ignore
	if(vertices.mesh.vertices.empty() && !vaos.count({ chunkx, chunky }))
		return;

	// If the new mesh is empty, try to delete it from the vao list
	if(vertices.mesh.vertices.empty() && vaos.count({ chunkx, chunky })) {
		gfx::Vao vao = vaos.at({ chunkx, chunky });
		glDeleteVertexArrays(1, &vao.vaoid);
		glDeleteBuffers(vao.buffers.size(), &vao.buffers[0]);
		vaos.erase({ chunkx, chunky });
		return;
	}

	// Create a new vao if we have a non empty vertex list and it doesn't exist
	if(!vaos.count({ chunkx, chunky })) {
		vaos[{ chunkx, chunky }] = gfx::Vao();
		vaos[{ chunkx, chunky }].genBuffers(4);
	}

	gfx::Vao &vao = vaos[{ chunkx, chunky }];	

	glBindVertexArray(vao.vaoid);
	levelDataToBuffers(vao.buffers, vertices);
	vao.vertcount = vertices.indices.size();
	glBindVertexArray(0);
}

gfx::Vao genWallTileVao(Tile tile) {
	gfx::Vao vao;

	unsigned int quadcount = 0;
	mesh::ElementArrayBuffer<float> vertices;

	glm::vec2 texOffset = tile.getTexOffset();
	addTileQuadToBuffer(vertices, 0.0f, 0.0f, 0.0f, WALL_HEIGHT, texOffset, WALL_TOP_SHADING, WALL_BOT_SHADING);
	for(int i = 0; i < 6; i++)
		vertices.indices.push_back(4 * quadcount + INDICES[i]);
	quadcount++;

	float y = WALL_HEIGHT;
	addTileQuadToBuffer(vertices, 0.0f, y, 0.0f, 1.0f, texOffset, 1.0f, 1.0f);
	for(int i = 0; i < 6; i++)
		vertices.indices.push_back(4 * quadcount + INDICES[i]);
	quadcount++;

	vao.genBuffers(4);
	glBindVertexArray(vao.vaoid);
	levelDataToBuffers(vao.buffers, vertices);
	vao.vertcount = vertices.indices.size();
	glBindVertexArray(0);

	return vao;
}
