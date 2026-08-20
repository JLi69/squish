#pragma once

#include "sprite.hpp"
#include "animation.hpp"
#include <glm/glm.hpp>

const float DEFAULT_PLAYER_SCALE = 1.1f;
const glm::vec2 DEFAULT_PLAYER_OFFSET = glm::vec2(0.0f, 0.4f);

struct Player {
	Sprite sprite = Sprite("", glm::vec2(0.0f, 0.0f));
	// Position of the player
	int x = 0, y = 0;
	AnimationValue squishyAnimation;
	// Animation for moving
	AnimationValue translateX, translateY;
	bool translationAnimationActive = false;

	// Constructor
	Player(int px, int py);

	void update(float dt);
	void activateTranslationAnimation(float x1, float x2, float y1, float y2);
	glm::vec2 getDisplayPos() const;
};
