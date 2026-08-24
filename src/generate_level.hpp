#pragma once

#include "level.hpp"
#include "enemies.hpp"
#include <memory>

struct GeneratedLevel {
	Level level = Level(0, 0, 1, 1);
	std::vector<std::unique_ptr<Enemy>> enemies;
	void spawnEnemy(std::unique_ptr<Enemy> enemy);
};

// For debug purposes: generates test level
GeneratedLevel genTestLevel();
// Generates a cave level
GeneratedLevel genCaveLevel(unsigned int seed);
