#include "bomb.hpp"

Bomb::Bomb(int px, int py) {
	initEnemy(px, py, "goo_bomb", 0.9f);

	moveEnemyTimer.active = false;
	default_offset += glm::vec2(0.05f, -0.1f);
	sprite.offset = default_offset;

	sprite.shadowScale = glm::vec2(1.0f, 0.3f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.225f);

	sprite.rotation = -20.0f;

	bloodColor = colors::SLIME_GREEN;

	gooAmt = 0.0f;
	damage = 0;
	setDir(0, 0);
	minGooCoin = 0;
	maxGooCoin = 0;
}
