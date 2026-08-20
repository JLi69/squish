#pragma once

#include <glm/glm.hpp>
#include <string>

struct Sprite {
	std::string spriteTexId = "";
	// Transform
	glm::vec2 offset = glm::vec2(0.0f);
	glm::vec2 scale = glm::vec2(1.0f);
	// Shadow transform
	glm::vec2 shadowScale = glm::vec2(1.0f);
	glm::vec2 shadowOffset = glm::vec2(0.0f);
	bool flip = false;
	bool drawShadow = true;
	Sprite(const std::string &texId, glm::vec2 pos) {
		spriteTexId = texId;
		offset = pos;
	}
};
