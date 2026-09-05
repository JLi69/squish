#pragma once

#include "level.hpp"
#include "enemies.hpp"
#include <memory>

struct GeneratedLevel {
	Level level = Level(0, 0, 1, 1);
	std::vector<std::unique_ptr<Enemy>> enemies;
	void spawnEnemy(std::unique_ptr<Enemy> enemy);
};

struct Room {
	int x, y;
	int halfw, halfh;
	Room(int posX, int posY, int halfWidth, int halfHeight);
	bool contains(int px, int py) const;
	bool intersects(const Room &other) const;
};

void createRoom(Level &level, Room room, const std::string &wallTile);

// For debug purposes: generates test level
GeneratedLevel genTestLevel();
// Generates a cave level
GeneratedLevel genCaveLevel(unsigned int seed);
