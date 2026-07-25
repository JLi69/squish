#include "game.hpp"

Sprite::Sprite(const std::string &texId, glm::vec2 pos) {
	spriteTexId = texId;
	offset = pos;
}

Player::Player(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("player", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.4f, 0.7f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.1f);

	sprite.offset = glm::vec2(0.0f, 0.4f);
	sprite.scale = glm::vec2(1.2f, 1.2f);
}
