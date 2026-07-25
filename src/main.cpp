#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gfx.hpp"
#include "app.hpp"
#include "assets.hpp"
#include "display.hpp"
#include "game.hpp"

void display(Game &game, int w, int h) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Display level
	setupShader("tile_shader", w, h, DEFAULT_ZOOM);
	displayLevel(game.getTileVaos());

	// Display player
	setupShader("shadow_shader", w, h, DEFAULT_ZOOM);
	setupShader("sprite_shader", w, h, DEFAULT_ZOOM);
	const Player &player = game.getPlayer();
	glm::vec2 playerPos(float(player.x), float(player.y));
	displaySprite(player.sprite, playerPos, game.getLevel());
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

	Game game = Game();
	game.initTestLevel();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float dt = 0.0f;
	// Main loop
	while(!glfwWindowShouldClose(window)) {
		game.update(dt);

		game.updateChunkVaos();
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		display(game, w, h);

		updateInputStates();
		glfwSwapBuffers(window);
		glfwPollEvents();

		gfx::outputErrors();
	}

	// Clean up
	glfwTerminate();
}
