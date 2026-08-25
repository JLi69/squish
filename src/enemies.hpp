#pragma once

#include "actor.hpp"
#include "level.hpp"
#include "player.hpp"
#include "particles.hpp"
#include "colors.hpp"

class Enemy : public Actor {
protected:
	AnimationValue squishyAnimation;
	glm::vec2 default_offset = glm::vec2(0.0f);
	float default_scale = 1.0f;
	float moveEnemyInterval = 1.0f, moveEnemyTimer = 1.0f;

	int dirx = 0, diry = 0;
	Color bloodColor = colors::RED;
public:
	void setDir(int dx, int dy);
	void update(float dt) override;
	// Returns true when the enemy should move
	bool updateMoveTimer(float dt);
	virtual void moveEnemy(Level &level);
	virtual void updateDir(const Level &level, const Player &player);

	int getDirX() const;
	int getDirY() const;

	bool uncollide(Level &level);
	bool isInsideTile(const Level &level) const;

	void squish(ParticleList &particles) const;
};

class Slime : public Enemy {
public:
	Slime(int px, int py);
	void updateDir(const Level &level, const Player &player) override;
};
