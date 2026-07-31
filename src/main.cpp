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

	Camera2D &camera = game.getCamera();
	glm::mat4 camMat = camera.getMat();

	// Display level
	setupShaderCam("tile_shader", w, h, DEFAULT_ZOOM, camMat);
	displayLevel(game.getTileVaos());

	// Display pushed blocks	
	TEXTURES->bindTexture("tiles", GL_TEXTURE0);
	ShaderProgram &tileShader = SHADERS->getShader("tile_shader");
	tileShader.uniformVec2("textureScale", glm::vec2(16.0f, 16.0f));
	float topy = game.getLevel().getTopY(),
		  boty = game.getLevel().getBottomY();
	for(const auto &pair : game.getLevel().getPushedTiles()) {
		const PushedTile &pushedTile = pair.second;
		glm::vec2 offset(pushedTile.translateX.value(), pushedTile.translateY.value());
		float z = getZFromY(pushedTile.translateY.value(), topy, boty);
		displayChunk(pushedTile.vao, tileShader, offset, z);
	}

	VAOS->bind("quad");
	setupShaderCam("tile_shadow_shader", w, h, DEFAULT_ZOOM, camMat);
	ShaderProgram &tileShadowShader = SHADERS->getShader("tile_shadow_shader");
	tileShadowShader.uniformFloat("shading", FLOOR_WALL_SHADING * FLOOR_SHADING * 0.5f);
	for(const auto &pair : game.getLevel().getPushedTiles()) {
		const PushedTile &pushedTile = pair.second;
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 pos(pushedTile.translateX.value(), pushedTile.translateY.value(), 1.0f);
		pos.y -= 1.0f;
		transform = glm::translate(transform, pos);
		tileShadowShader.uniformMat4x4("transform", transform);
		VAOS->draw();
	}

	// Display player
	setupShaderCam("shadow_shader", w, h, DEFAULT_ZOOM, camMat);
	setupShaderCam("sprite_shader", w, h, DEFAULT_ZOOM, camMat);
	const Player &player = game.getPlayer();
	glm::vec2 playerPos = player.getDisplayPos();
	displaySprite(player.sprite, playerPos, game.getLevel());
}

int main() {
	// Initialize the window
	GLFWwindow *window = initWindow();

	Tile::initTiles("assets/tiles.impfile");
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
		float begin = glfwGetTime();

		game.update(dt);

		game.updateChunkVaos();
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		display(game, w, h);

		updateInputStates();
		glfwSwapBuffers(window);
		glfwPollEvents();

		gfx::outputErrors();

		dt = glfwGetTime() - begin;
	}

	// Clean up
	glfwTerminate();
}
