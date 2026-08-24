#pragma once

#include "actor.hpp"
#include "level.hpp"
#include "player.hpp"

class Enemy : public Actor {
protected:
	AnimationValue squishyAnimation;
	glm::vec2 default_offset = glm::vec2(0.0f);
	float default_scale = 1.0f;
	float moveEnemyInterval = 1.0f, moveEnemyTimer = 1.0f;

	int dirx = 0, diry = 0;

public:
	void setDir(int dx, int dy);
	void update(float dt) override;
	// Returns true when the enemy should move
	bool updateMoveTimer(float dt);
	virtual void moveEnemy(Level &level);
	virtual void updateDir(const Level &level, const Player &player);
};

class Slime : public Enemy {
public:
	Slime(int px, int py);
	void updateDir(const Level &level, const Player &player) override;
};
