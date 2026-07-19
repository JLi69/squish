#include "tilemap_gfx.hpp"

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
	Tile tile,
	float shadingTop,
	float shadingBot
) {
	glm::vec2 texBotLeft = tile.getTexOffset() + glm::vec2(0.01f, 0.01f);

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

mesh::ElementArrayBuffer<float> getTilemapVertDataForChunk(
	const TileMap &tilemap,
	int chunkx,
	int chunky
) {
	mesh::ElementArrayBuffer<float> vertexData;

	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3,
	};

	unsigned int quadcount = 0;
	for(int tx = 0; tx < CHUNK_SIZE; tx++) {
		for(int ty = 0; ty < CHUNK_SIZE; ty++) {
			Tile tile = tilemap.getTile(tx + chunkx * CHUNK_SIZE, ty + chunky * CHUNK_SIZE);
			if(tile.isEmpty())
				continue;

			float x = float(tx);
			float y = float(ty);
			if(tile.isWall) {
				float ycoord = y + float(chunky * CHUNK_SIZE);
				float z = (ycoord - tilemap.getTopY()) / float(tilemap.getTopY() - tilemap.getBottomY());
				addTileQuadToBuffer(vertexData, x, y, z, 0.6f, tile, 0.75f, 0.6f);
			}
			else {
				Tile aboveTile = tilemap.getTile(tx + chunkx * CHUNK_SIZE, ty + chunky * CHUNK_SIZE + 1);
				if(aboveTile.isWall)
					addTileQuadToBuffer(vertexData, x, y, 1.0f, 1.0f, tile, 0.75f, 0.9f);
				else
					addTileQuadToBuffer(vertexData, x, y, 1.0f, 1.0f, tile, 0.9f, 0.9f);
			}
			for(int i = 0; i < 6; i++)
				vertexData.indices.push_back(4 * quadcount + indices[i]);
			quadcount++;
		}
	}

	for(int tx = 0; tx < CHUNK_SIZE; tx++) {
		for(int ty = 0; ty < CHUNK_SIZE; ty++) {
			Tile tile = tilemap.getTile(tx + chunkx * CHUNK_SIZE, ty + chunky * CHUNK_SIZE);
			if(tile.isEmpty())
				continue;

			if(!tile.isWall)
				continue;

			float x = float(tx);
			float y = float(ty) + 0.6f;
			float ycoord = float(ty) + float(chunky * CHUNK_SIZE);
			float z = (ycoord - tilemap.getTopY()) / float(tilemap.getTopY() - tilemap.getBottomY());
			addTileQuadToBuffer(vertexData, x, y, z, 1.0f, tile, 1.0f, 1.0f);
			for(int i = 0; i < 6; i++)
				vertexData.indices.push_back(4 * quadcount + indices[i]);
			quadcount++;
		}
	}

	return vertexData;
}

TileVaos getTileMapVaos(const TileMap &tilemap) {
	TileVaos tilevaos = TileVaos();

	int startChunkX = tileToChunkCoord(tilemap.getLeftX());
	int endChunkX = tileToChunkCoord(tilemap.getRightX());
	int startChunkY = tileToChunkCoord(tilemap.getBottomY());
	int endChunkY = tileToChunkCoord(tilemap.getTopY());

	for(int chunkx = startChunkX; chunkx <= endChunkX; chunkx++) {
		for(int chunky = startChunkY; chunky <= endChunkY; chunky++) {
			mesh::ElementArrayBuffer<float> vertices = getTilemapVertDataForChunk(
				tilemap,
				chunkx,
				chunky
			);
			// Ignore empty vertex chunks
			if(vertices.mesh.vertices.empty())
				continue;

			std::pair<int, int> coords = { chunkx, chunky };
			
			gfx::Vao vao;
			vao.genBuffers(4);
			glBindVertexArray(vao.vaoid);
			// vertex positions
			glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(0));	
			glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture offset
			glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(1));
			glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
			glVertexAttribPointer(1, 2, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			// shading
			glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(2));	
			glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
			glVertexAttribPointer(2, 1, GL_FLOAT, false, 6 * sizeof(float), (void*)(5 * sizeof(float)));
			glEnableVertexAttribArray(2);
			// indices
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.buffers.at(3));
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				vertices.indices.size() * sizeof(unsigned int),
				&vertices.indices[0],
				GL_STATIC_DRAW
			);
			vao.vertcount = vertices.indices.size();
			glBindVertexArray(0);

			tilevaos.insert({ coords, vao });
		}
	}

	return tilevaos;
}

void updateTileMapVaos(
	TileVaos &vaos,
	const TileMap &tilemap,
	int chunkx,
	int chunky
) {
	mesh::ElementArrayBuffer<float> vertices = getTilemapVertDataForChunk(
		tilemap,
		chunkx,
		chunky
	);

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
	// vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(0));	
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture offset
	glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(1));
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// shading
	glBindBuffer(GL_ARRAY_BUFFER, vao.buffers.at(2));	
	glBufferData(GL_ARRAY_BUFFER, vertices.mesh.size(), vertices.mesh.ptr(), GL_STATIC_DRAW);	
	glVertexAttribPointer(2, 1, GL_FLOAT, false, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao.buffers.at(3));
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		vertices.indices.size() * sizeof(unsigned int),
		&vertices.indices[0],
		GL_STATIC_DRAW
	);
	vao.vertcount = vertices.indices.size();
	glBindVertexArray(0);
}
