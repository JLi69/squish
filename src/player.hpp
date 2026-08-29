#pragma once

#include "actor.hpp"
#include "particles.hpp"

const float DEFAULT_PLAYER_SCALE = 1.1f;
const glm::vec2 DEFAULT_PLAYER_OFFSET = glm::vec2(0.0f, 0.4f);

const int DEFAULT_HEALTH = 3;

class Player : public Actor {
	AnimationValue squishyAnimation;
	int health = DEFAULT_HEALTH;
	float playerDamageTimer = 0.0f;
public:	
	// Constructor
	Player(int px, int py);
	void update(float dt) override;
	int getHealth() const;
	void damage(int amt);
	bool isDead() const;
	void explode(ParticleList &particles);
	Color getColor() const;
	Color getMultColor() const;
};
