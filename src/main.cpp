#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gfx.hpp"
#include "shader.hpp"
#include "tilemap.hpp"
#include "app.hpp"
#include "tilemap_gfx.hpp"

bool canPush(const TileMap &tilemap, int x, int y, int dirx, int diry) {
	if(!tilemap.getTile(x, y).canPush)
		return false;
	if(tilemap.getTile(x + dirx, y + diry).isWall)
		return false;
	return true;
}

int main() {
	// Initialize the window
	GLFWwindow *window = initWindow();

	Tile::initNameToId();
	Tile::initTextureOffsets();

	TileMap tilemap(-8, -8, 8, 8);
	// Test map
	Tile wallTile = Tile(1);
	wallTile.isWall = true;
	Tile floorTile = Tile(2);
	for(int x = -4; x <= 4; x++) {
		for(int y = -4; y <= 4; y++) {
			if((x >= -2 && x <= 0) && y == 2) {
				tilemap.setTile(x, y, wallTile);
				continue;
			}
			if(abs(x) == 4 || abs(y) == 4) {
				tilemap.setTile(x, y, wallTile);
				continue;
			}
			tilemap.setTile(x, y, floorTile);
		}
	}
	tilemap.setTile(2, -2, wallTile);
	
	Tile crateTile = Tile(3);
	crateTile.isWall = true;
	crateTile.canPush = true;
	tilemap.setTile(-2, -2, crateTile);

	TileVaos tileVaos = getTileMapVaos(tilemap);
	gfx::Vao quadVao = gfx::createQuadVao();
	// Shader
	ShaderProgram tileShader(
		"assets/shaders/tilemap_vert.glsl",
		"assets/shaders/tilemap_frag.glsl"
	);
	ShaderProgram spriteShader(
		"assets/shaders/sprite_vert.glsl",
		"assets/shaders/sprite_frag.glsl"
	);
	ShaderProgram shadowShader(
		"assets/shaders/sprite_vert.glsl",
		"assets/shaders/shadow_frag.glsl"
	);
	// Texture
	unsigned int tileTextures;
	glGenTextures(1, &tileTextures);
	gfx::loadTexture("assets/textures/tiles.png", tileTextures, true);
	unsigned int playerTexture;
	glGenTextures(1, &playerTexture);
	gfx::loadTexture("assets/textures/player.png", playerTexture, true);

	int playerx = 0, playery = 0;
	bool flipPlayer = false;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Main loop
	while(!glfwWindowShouldClose(window)) {
		int prevx = playerx, prevy = playery;
		// Move the player
		if(getKeyInputState(GLFW_KEY_UP) == JUST_PRESSED)
			playery++;
		else if(getKeyInputState(GLFW_KEY_DOWN) == JUST_PRESSED)
			playery--;
		else if(getKeyInputState(GLFW_KEY_RIGHT) == JUST_PRESSED) {
			flipPlayer = false;
			playerx++;
		}
		else if(getKeyInputState(GLFW_KEY_LEFT) == JUST_PRESSED) {
			flipPlayer = true;
			playerx--;
		}

		int dirx = playerx - prevx, 
			diry = playery - prevy;
		if(canPush(tilemap, playerx, playery, dirx, diry)) {
			Tile tile = tilemap.getTile(playerx, playery);
			// Quick hack, probably should have seperate layers for walls/floors
			tilemap.setTile(playerx, playery, floorTile);
			tilemap.setTile(playerx + dirx, playery + diry, tile);
			// Update tile map vaos
			int chunkx = tileToChunkCoord(playerx);
			int chunky = tileToChunkCoord(playery);
			// Probably can be more efficient and not update so many vaos at once
			// but I'm just hacking something together and this should work fine
			for(int x = chunkx - 1; x <= chunkx + 1; x++)
				for(int y = chunky - 1; y <= chunky + 1; y++)
					updateTileMapVaos(tileVaos, tilemap, x, y);
					
		}
		else if(tilemap.getTile(playerx, playery).isWall) {
			playerx = prevx;
			playery = prevy;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tileShader.use();
		glBindTexture(GL_TEXTURE_2D, tileTextures);
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		glm::mat4 windowMat = calculateWindowMat(float(w), float(h), 0.175f);
		tileShader.uniformMat4x4("windowMat", windowMat);
		tileShader.uniformVec2("textureScale", glm::vec2(16.0f, 16.0f));
		// Draw level
		for(const auto &tileVao : tileVaos) {
			const std::pair<int, int> &coords = tileVao.first;
			const gfx::Vao &vao = tileVao.second;
			vao.bind();
			glm::vec2 offset(
				float(coords.first * CHUNK_SIZE),
				float(coords.second * CHUNK_SIZE)
			);
			tileShader.uniformVec2("offset", offset);
			glDrawElements(GL_TRIANGLES, vao.vertcount, GL_UNSIGNED_INT, 0);
		}

		quadVao.bind();
		glm::vec3 shadowPos = glm::vec3(
			float(playerx),
			float(playery) - 0.1f,
			1.0f
		);
		shadowShader.use();
		shadowShader.uniformMat4x4("windowMat", windowMat);
		glm::mat4 shadowTransform = glm::mat4(1.0f);
		shadowTransform = glm::translate(shadowTransform, shadowPos);
		shadowTransform = glm::scale(shadowTransform, glm::vec3(1.4f, 0.7f, 1.0f));
		spriteShader.uniformMat4x4("transform", shadowTransform);
		glDrawElements(GL_TRIANGLES, quadVao.vertcount, GL_UNSIGNED_INT, 0);

		spriteShader.use();
		spriteShader.uniformBool("flipVert", flipPlayer);
		spriteShader.uniformMat4x4("windowMat", windowMat);
		float displayy = float(playery) + 0.4f;
		float z = (displayy - tilemap.getTopY()) / float(tilemap.getTopY() - tilemap.getBottomY());
		glm::vec3 playerPos = glm::vec3(
			float(playerx),
			displayy,
			z
		);
		glm::mat4 playerTransform = glm::mat4(1.0f);
		playerTransform = glm::translate(playerTransform, playerPos);
		playerTransform = glm::scale(playerTransform, glm::vec3(1.2f, 1.2f, 1.0f));
		spriteShader.uniformMat4x4("transform", playerTransform);
		glBindTexture(GL_TEXTURE_2D, playerTexture);
		glDrawElements(GL_TRIANGLES, quadVao.vertcount, GL_UNSIGNED_INT, 0);	

		updateInputStates();
		glfwSwapBuffers(window);
		glfwPollEvents();

		gfx::outputErrors();
	}

	// Clean up
	glfwTerminate();
}
