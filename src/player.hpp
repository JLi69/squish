#pragma once

#include "actor.hpp"

const float DEFAULT_PLAYER_SCALE = 1.1f;
const glm::vec2 DEFAULT_PLAYER_OFFSET = glm::vec2(0.0f, 0.4f);

class Player : public Actor {
	AnimationValue squishyAnimation;
public:	
	// Constructor
	Player(int px, int py);
	void update(float dt) override;
};
