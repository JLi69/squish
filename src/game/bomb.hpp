#pragma once

#include "enemies/enemies.hpp"

class Bomb : public Enemy {
public:
	Bomb(int px, int py);
	void attackPlayer(Player &player) override {}
};
