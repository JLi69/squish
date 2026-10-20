#pragma once

#include "enemies/enemies.hpp"

class Bomb : public Enemy {
	Timer explosionTimer;
public:
	Bomb(int px, int py);
	void attackPlayer(Player &player) override {}
	void update(float dt) override;
	void squish(ParticleList &particles) const override;
	void interactWithPlayer(Player &player) override;
	void interactWithEnemies(EnemyList &enemies) override;
};
