#include "generate_level.hpp"

Room::Room(int posX, int posY, int halfWidth, int halfHeight) {
	x = posX;
	y = posY;
	halfw = halfWidth;
	halfh = halfHeight;
}

bool Room::contains(int px, int py) const {
	return px >= x - halfw && px <= x + halfw && py >= y - halfh && py <= y + halfh;
}

bool Room::intersects(const Room &other) const {
	return x + halfw + 2 >= other.x - other.halfw - 2 &&
			x - halfw - 2 <= other.x + other.halfw + 2 &&
			y + halfh + 2 >= other.y - other.halfh - 2 &&
			y - halfh - 2 <= other.y + other.halfh + 2;
}

GeneratedLevel genTestLevel() {
	GeneratedLevel genLevel;

	Level testLevel(-8, -8, 8, 8);

	// Generate walls and floors
	for(int x = -4; x <= 4; x++) {
		for(int y = -4; y <= 4; y++) {
			testLevel.setFloorTile(x, y, "stone_floor");
			if((x >= -2 && x <= 0) && y == 2) {
				testLevel.setWallTile(x, y, "brick");
				continue;
			}
			if(abs(x) == 4 || abs(y) == 4) {
				testLevel.setWallTile(x, y, "brick");
				continue;
			}
		}
	}
	testLevel.setWallTile(2, -2, "brick");

	// Add a pushable crate
	testLevel.setWallTile(-2, -2, "crate");
	testLevel.setWallTile(-1, 0, "crate");

	genLevel.spawnEnemy(std::make_unique<Slime>(-3, 0));
	genLevel.spawnEnemy(std::make_unique<Snake>(3, 0));
	genLevel.level = testLevel;

	return genLevel;
}

void createRoom(Level &level, Room room, const std::string &wallTile) {
	int left = room.x - room.halfw - 1,
		right = room.x + room.halfw + 1,
		top = room.y + room.halfh + 1,
		bottom = room.y - room.halfh - 1;
	for(int x = left; x <= right; x++) {
		for(int y = bottom; y <= top; y++) {
			if(x == left || x == right || y == bottom || y == top) {
				if(level.getWallTile(x, y).isEmpty())
					continue;
				level.setWallTile(x, y, wallTile);
				continue;
			}
			level.setWallTile(x, y, "empty");
		}
	}
}

void GeneratedLevel::spawnEnemy(std::unique_ptr<Enemy> enemy) {
	enemies.push_back(std::move(enemy));
}
