#include <cstdlib>
#include <cstring>
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
		float z = getZFromY(pushedTile.translateY.value() - 0.5f, topy, boty);
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

	for(const auto &enemy : game.getEnemies()) {
		if(enemy == nullptr)
			continue;
		glm::vec2 displayPos = enemy->getDisplayPos();
		displaySprite(enemy->sprite, displayPos, game.getLevel());
	}

	setupShaderCam("flat_sprite_shader", w, h, DEFAULT_ZOOM, camMat);
	for(const auto &particle : game.getParticles()) {
		if(particle == nullptr)
			continue;
		displayParticle(*particle, game.getLevel());
	}

	// Display HUD
	float zoom = 1.0f;
	if(w < float(DEFAULT_WIDTH) * 0.5f && h < float(DEFAULT_HEIGHT) * 0.5f)
		zoom = 0.4f;
	else if(w < float(DEFAULT_WIDTH) * 0.75f && h < float(DEFAULT_HEIGHT) * 0.75f)
		zoom = 0.75f;
	else if(w > float(DEFAULT_WIDTH) * 1.25f && h > float(DEFAULT_HEIGHT) * 1.25f)
		zoom = 1.4f;
	glm::vec4 topLeft = glm::vec4(
		-float(w) / 2.0f * 1.0f / zoom + 32.0f,
		float(h) / 2.0f * 1.0f / zoom - 32.0f,
		0.0f,
		1.0f
	);
	setupShaderForUi("flat_sprite_shader", w, h, zoom);
	for(int i = 0; i < player.getHealth(); i++) {
		glm::vec4 pos = topLeft + glm::vec4(48.0f, 0.0f, 0.0f, 0.0f) * float(i);
		float scale = 1.0f;
		if(i == player.getHealth() - 1 && player.getHealth() > 1)
			scale = sin(game.getTime() * M_PI * 0.75f) * 0.1f + 1.1f;
		else if(i == player.getHealth() - 1 && player.getHealth() == 1)
			scale = sin(game.getTime() * M_PI) * 0.2f + 1.1f;
		scale *= 48.0f;
		displayIcon("heart", Transform(pos, glm::vec2(scale)));
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

	bool useTestLevel = false;
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--test") == 0)
			useTestLevel = true;
	}

	// Initialize the window
	GLFWwindow *window = initWindow();

	Tile::initTiles("assets/tiles.impfile");
	VAOS->genSimple();
	// Setup shaders
	SHADERS->importFromFile("assets/shaders.impfile");	
	// Setup textures
	TEXTURES->importFromFile("assets/textures.impfile");

	Game game = Game();
	if(useTestLevel) {
		game.initTestLevel();
	}
	else {
		game.initCaveLevel();
	}

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
