#include "enemies.hpp"
#include "random_utils.hpp"

const int DIRECTION_X[] = { -1, 1,  0, 0 };
const int DIRECTION_Y[] = {  0, 0, -1, 1 };

Slime::Slime(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("slime", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.3f, 0.65f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.02f);

	default_offset = DEFAULT_ENEMY_OFFSET;
	default_scale = 0.9f;

	sprite.offset = default_offset;
	sprite.scale = glm::vec2(default_scale, default_scale);

	squishyAnimation = AnimationValue(default_scale - 0.1f, default_scale + 0.1f, 0.5f);
	squishyAnimation.time = randf_range(0.0f, 1.0f);
	squishyAnimation.loop = true;

	moveEnemyTimer = Timer(1.0f, true);
	moveEnemyTimer.currentTime = randf_range(0.0f, 1.0f);
	attackTimer = Timer(0.75f, true);

	bloodColor = colors::SLIME_GREEN;
}

void Slime::updateDir(const Level &level, const Player &player) {
	int distx = std::abs(player.x - x);
	int disty = std::abs(player.y - y);

	if(distx > 10 || disty > 10 || (distx == 0 && disty == 0)) {
		setDir(0, 0);
		return;
	}

	if(dirx == 0 && diry == 0) {
		int index = rand() % 4;
		setDir(DIRECTION_X[index], DIRECTION_Y[index]);
		return;
	}
	
	if(y == player.y && x < player.x && distx <= 8)
		setDir(1, 0);
	else if(y == player.y && x > player.x && distx <= 8)
		setDir(-1, 0);
	else if(x == player.x && y < player.y && disty <= 8)
		setDir(0, 1);
	else if(x == player.x && y > player.y && disty <= 8)
		setDir(0, -1);

	if(level.getWallTile(x + dirx, y + diry).isEmpty())
		return;

	int index = rand() % 4;
	setDir(DIRECTION_X[index], DIRECTION_Y[index]);
}
