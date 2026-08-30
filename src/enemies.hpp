#pragma once

#include "actor.hpp"
#include "level.hpp"
#include "player.hpp"
#include "particles.hpp"
#include "colors.hpp"
#include "timer.hpp"

const glm::vec2 DEFAULT_ENEMY_OFFSET = glm::vec2(0.0f, 0.3f);

class Enemy : public Actor {
protected:
	AnimationValue squishyAnimation;
	glm::vec2 default_offset = glm::vec2(0.0f);
	float default_scale = 1.0f;	
	int damage = 1;

	int dirx = 0, diry = 0;
	Color bloodColor = colors::RED;
	
	AnimationValue attackOffsetX, attackOffsetY;
public:
	Timer moveEnemyTimer;
	Timer attackTimer;
	bool attackAnimationActive = false;

	void setDir(int dx, int dy);
	void update(float dt) override;
	virtual void moveEnemy(Level &level);
	virtual void updateDir(const Level &level, const Player &player);

	int getDirX() const;
	int getDirY() const;

	bool uncollide(Level &level);
	bool isInsideTile(const Level &level) const;

	void squish(ParticleList &particles) const;

	virtual bool canAttackPlayer(const Player &player);
	virtual void attackPlayer(Player &player);

	glm::vec2 getDisplayPos() const override;
};

class Slime : public Enemy {
public:
	Slime(int px, int py);
	void updateDir(const Level &level, const Player &player) override;
};
