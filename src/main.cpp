#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gfx.hpp"
#include "level.hpp"
#include "app.hpp"
#include "tilemap_gfx.hpp"
#include "assets.hpp"
#include "display.hpp"
#include "game.hpp"

bool canPush(const Level &level, int x, int y, int dirx, int diry) {
	if(!level.getWallTile(x, y).canPush)
		return false;
	if(!level.getWallTile(x + dirx, y + diry).isEmpty())
		return false;
	return true;
}

int main() {
	// Initialize the window
	GLFWwindow *window = initWindow();

	Tile::initTextureOffsets();
	VAOS->genSimple();
	// Setup shaders
	SHADERS->importFromFile("assets/shaders.impfile");	
	// Setup textures
	TEXTURES->importFromFile("assets/textures.impfile");

	Level testLevel = genTestLevel();
	Player player(0, 0);
	TileVaos tileVaos = getTileMapVaos(testLevel);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Main loop
	while(!glfwWindowShouldClose(window)) {
		int prevx = player.x, prevy = player.y;
		// Move the player
		if(getKeyInputState(GLFW_KEY_UP) == JUST_PRESSED)
			player.y++;
		else if(getKeyInputState(GLFW_KEY_DOWN) == JUST_PRESSED)
			player.y--;
		else if(getKeyInputState(GLFW_KEY_RIGHT) == JUST_PRESSED) {
			player.sprite.flip = false;
			player.x++;
		}
		else if(getKeyInputState(GLFW_KEY_LEFT) == JUST_PRESSED) {
			player.sprite.flip = true;
			player.x--;
		}

		int dirx = player.x - prevx, 
			diry = player.y - prevy;
		if(canPush(testLevel, player.x, player.y, dirx, diry)) {
			Tile tile = testLevel.getWallTile(player.x, player.y);
			// Quick hack, probably should have seperate layers for walls/floors
			testLevel.setWallTile(player.x, player.y, Tile());
			testLevel.setWallTile(player.x + dirx, player.y + diry, tile);
			// Update tile map vaos
			int chunkx = tileToChunkCoord(player.x);
			int chunky = tileToChunkCoord(player.y);
			// Probably can be more efficient and not update so many vaos at once
			// but I'm just hacking something together and this should work fine
			for(int x = chunkx - 1; x <= chunkx + 1; x++)
				for(int y = chunky - 1; y <= chunky + 1; y++)
					updateTileMapVaos(tileVaos, testLevel, x, y);
					
		}
		else if(!testLevel.getWallTile(player.x, player.y).isEmpty()) {
			player.x = prevx;
			player.y = prevy;
		}

		int w, h;
		glfwGetWindowSize(window, &w, &h);

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Display level
		setupShader("tile_shader", w, h, DEFAULT_ZOOM);
		displayLevel(tileVaos);

		// Display player
		setupShader("shadow_shader", w, h, DEFAULT_ZOOM);
		setupShader("sprite_shader", w, h, DEFAULT_ZOOM);
		glm::vec2 playerPos(float(player.x), float(player.y));
		displaySprite(player.sprite, playerPos, testLevel);

		updateInputStates();
		glfwSwapBuffers(window);
		glfwPollEvents();

		gfx::outputErrors();
	}

	// Clean up
	glfwTerminate();
}
