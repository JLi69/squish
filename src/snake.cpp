#include "enemies.hpp"
#include "random_utils.hpp"

Snake::Snake(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("snake", glm::vec2(0.0f, 0.0f));

	sprite.shadowScale = glm::vec2(1.0f, 0.4f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.12f);

	default_offset = DEFAULT_ENEMY_OFFSET;
	default_scale = 0.9f;

	sprite.offset = default_offset;
	sprite.scale = glm::vec2(default_scale, default_scale);

	squishyAnimation = AnimationValue(default_scale - 0.05f, default_scale + 0.05f, 0.5f);
	squishyAnimation.time = randf_range(0.0f, 1.0f);
	squishyAnimation.loop = true;

	moveEnemyTimer = Timer(1.0f, true);
	moveEnemyTimer.currentTime = randf_range(0.0f, 1.0f);
	attackTimer = Timer(0.75f, true);
}

void Snake::updateDir(const Level &level, const Player &player) {
	// TODO
	setDir(0, 0);
}
