#include "enemies.hpp"

const int DIRECTION_X[] = { -1, 1,  0, 0 };
const int DIRECTION_Y[] = {  0, 0, -1, 1 };

Snake::Snake(int px, int py) {
	initEnemy(px, py, "snake");

	sprite.shadowScale = glm::vec2(1.0f, 0.4f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.12f);	
}

void Snake::updateDir(const Level &level, const Player &player) {
	if(translationAnimationActive)
		return;

	int dirIndex = -1;
	uint32_t minDist = UINT32_MAX;
	for(int i = 0; i < 4; i++) {
		int tilex = x + DIRECTION_X[i],
			tiley = y + DIRECTION_Y[i];
		uint32_t dist = level.getDistToPlayer(tilex, tiley);
		if(dist < minDist) {
			dirIndex = i;
			minDist = dist;
		}
	}

	if(dirIndex == -1)
		setDir(0, 0);
	else
		setDir(DIRECTION_X[dirIndex], DIRECTION_Y[dirIndex]);

	if(dirx < 0)
		sprite.flip = true;
	else if(dirx > 0)
		sprite.flip = false;
}
