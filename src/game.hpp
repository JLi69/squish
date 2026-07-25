#pragma once

#include <string>
#include <glm/glm.hpp>

struct Sprite {
	std::string spriteTexId = "";
	// Transform
	glm::vec2 offset = glm::vec2(0.0f);
	glm::vec2 scale = glm::vec2(1.0f);
	// Shadow transform
	glm::vec2 shadowScale = glm::vec2(1.0f);
	glm::vec2 shadowOffset = glm::vec2(0.0f);
	bool flip = false;
	Sprite(const std::string &texId, glm::vec2 pos);
};

struct Player {
	Sprite sprite = Sprite("", glm::vec2(0.0f, 0.0f));
	// Position of the player
	int x = 0, y = 0;

	// Constructor
	Player(int px, int py);
};
