#include "enemies.hpp"

Snake::Snake(int px, int py) {
	initEnemy(px, py, "snake", 0.9f);

	sprite.shadowScale = glm::vec2(1.0f, 0.4f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.12f);	
}

void Snake::updateDir(const Level &level, const Player &player) {
	if(translationAnimationActive)
		return;

	uint32_t minDist = UINT32_MAX;
	for(int i = 0; i < 4; i++) {
		int tilex = x + enemies::DIRECTION_X[i],
			tiley = y + enemies::DIRECTION_Y[i];
		uint32_t dist = level.getDistToPlayer(tilex, tiley);
		if(dist < minDist)
			minDist = dist;
	}

	// Get the possible directions the enemy can move in
	std::vector<int> directions;
	for(int i = 0; i < 4; i++) {
		int tilex = x + enemies::DIRECTION_X[i],
			tiley = y + enemies::DIRECTION_Y[i];
		uint32_t dist = level.getDistToPlayer(tilex, tiley);
		if(dist == minDist && dist != UINT32_MAX)
			directions.push_back(i);
	}

	int dirIndex = -1;
	if(directions.empty()) {
		setDir(0, 0);
		return;
	}
	else {
		dirIndex = directions.at(0);

		// Attempt to see if we can go in the 'preferred' direction
		// We alternate between up and down in order to have the enemy
		// move in a more natural 'diagonal' pattern
		for(const int index : directions) {
			if(currentStep % 2 == enemies::PRIORITIZE_VERTICAL) {
				// Vertical
				if(enemies::DIRECTION_Y[index] == 0)
					continue;
				dirIndex = index;
			}
			else if(currentStep % 2 == enemies::PRIORITIZE_HORIZONTAL) {
				// Horizontal
				if(enemies::DIRECTION_X[index] == 0)
					continue;
				dirIndex = index;
			}
		}
	}
	
	setDir(enemies::DIRECTION_X[dirIndex], enemies::DIRECTION_Y[dirIndex]);

	if(dirx < 0)
		sprite.flip = true;
	else if(dirx > 0)
		sprite.flip = false;
}
