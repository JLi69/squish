#include "generate_level.hpp"
#include <random>
#include <stack>

const int CAVE_SIZE = 96;
const int ROOM_DIST = 5;
const int MARGIN = 16;

struct Room {
	int x, y;
	int halfw, halfh;
	Room(int posX, int posY, int halfWidth, int halfHeight) {
		x = posX;
		y = posY;
		halfw = halfWidth;
		halfh = halfHeight;
	}

	bool contains(int px, int py) const {
		return px >= x - halfw && px <= x + halfw && py >= y - halfh && py <= y + halfh;
	}

	bool intersects(const Room &other) const {
		return x + halfw + 2 >= other.x - other.halfw - 2 &&
				x - halfw - 2 <= other.x + other.halfw + 2 &&
				y + halfh + 2 >= other.y - other.halfh - 2 &&
				y - halfh - 2 <= other.y + other.halfh + 2;
	}
};

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
	genLevel.spawnEnemy(std::make_unique<Slime>(0, -3));
	genLevel.level = testLevel;

	return genLevel;
}

static void createRoom(Level &level, Room room, const std::string &wallTile) {
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

static void createDungeonRoom(
	Level &level,
	Room room,
	const std::string &wallTile, 
	const std::string &floorTile
) {
	int left = room.x - room.halfw - 1,
		right = room.x + room.halfw + 1,
		top = room.y + room.halfh + 1,
		bottom = room.y - room.halfh - 1;
	const int doorX[] = { left, right, room.x, room.x };
	const int doorY[] = { room.y, room.y, bottom, top };
	for(int x = left; x <= right; x++) {
		for(int y = bottom; y <= top; y++) {
			if(x == left || x == right || y == bottom || y == top) {
				level.setWallTile(x, y, wallTile);	
				continue;
			}
			level.setWallTile(x, y, "empty");
			level.setFloorTile(x, y, floorTile);
		}
	}

	for(int i = 0; i < 4; i++) {
		level.setFloorTile(doorX[i], doorY[i], floorTile);
		for(int dx = -1; dx <= 1; dx++) {
			for(int dy = -1; dy <= 1; dy++) {
				if(level.getWallTile(doorX[i] + dx, doorY[i] + dy).tileId == tile("brick").tileId)
					continue;
				level.setWallTile(doorX[i] + dx, doorY[i] + dy, "empty");
			}
		}
		level.setWallTile(doorX[i], doorY[i], "empty");
	}
}

static bool addNewRooms(
	int x, int y, int halfw, int halfh,
	std::stack<Room> &rooms, 
	const std::set<std::pair<int, int>> &visited,
	int index
) {
	const int diffX[] = { ROOM_DIST, -ROOM_DIST, 0, 0 };
	const int diffY[] = { 0, 0, ROOM_DIST, -ROOM_DIST };

	// Attempt to find an open room space
	for(int i = 0; i < 4; i++) {
		int dx = diffX[(index + i) % 4],
			dy = diffY[(index + i) % 4];
		if(x + dx < -CAVE_SIZE + MARGIN + halfw || x + dx > CAVE_SIZE - MARGIN - halfw)
			continue;
		if(y + dy < -CAVE_SIZE + MARGIN + halfh || y + dy > CAVE_SIZE - MARGIN - halfh)
			continue;
		if(visited.count({ x + dx, y + dy }))
			continue;
		int adjCount = 0;
		for(int j = 0; j < 4; j++)
			adjCount += visited.count({ x + dx + diffX[j], y + dy + diffY[j] });
		if(adjCount >= 2)
			continue;
		rooms.push(Room(x + dx, y + dy, halfw, halfh));
		return true;
	}

	return false;
}

GeneratedLevel genCaveLevel(unsigned int seed) {
	fprintf(stderr, "Generating cave level with seed: %u.\n", seed);

	GeneratedLevel genLevel;
	genLevel.level = Level(-CAVE_SIZE, -CAVE_SIZE, CAVE_SIZE, CAVE_SIZE);
	Level &level = genLevel.level;

	for(int x = -CAVE_SIZE; x <= CAVE_SIZE; x++) {
		for(int y = -CAVE_SIZE; y <= CAVE_SIZE; y++) {
			level.setFloorTile(x, y, "dirt");
			level.setWallTile(x, y, "stone");
		}
	}

	std::set<std::pair<int, int>> visited;
	std::stack<Room> rooms;
	std::vector<Room> emptyRooms;
	rooms.push(Room(0, 0, 3, 3));
	emptyRooms.push_back(rooms.top());

	std::mt19937 random;
	random.seed(seed);

	// Create the rooms
	while(!rooms.empty()) {
		Room room = rooms.top();
		int halfw = random() % 4 + 3,
			halfh = random() % 4 + 3;
		if(visited.count({ room.x, room.y })) {	
			if(!addNewRooms(room.x, room.y, halfw, halfh, rooms, visited, random() % 4))
				rooms.pop();
			continue;
		}
		createRoom(level, room, "dirt");
		visited.insert({ room.x, room.y });
		if((rooms.size() > 2 && random() % 3 == 0) || rooms.size() >= 3) {
			if(random() % 8 == 0)
				emptyRooms.push_back(room);
			rooms.pop();
		}
		if(!addNewRooms(room.x, room.y, halfw, halfh, rooms, visited, random() % 4)) {
			if(random() % 8 == 0)
				emptyRooms.push_back(room);
			rooms.pop();
		}
	}

	for(int x = -CAVE_SIZE; x <= CAVE_SIZE; x++) {
		for(int y = -CAVE_SIZE; y <= CAVE_SIZE; y++) {
			bool canPlaceTile = true;
			for(const auto &room : emptyRooms)
				if(room.contains(x, y))
					canPlaceTile = false;
			if(!canPlaceTile)
				continue;
			if(!level.getWallTile(x, y).isEmpty())
				continue;
			if(random() % 4 == 0 && (x + y) % 2 == 0)
				level.setWallTile(x, y, "dirt");
			if(random() % 3 == 0 && (y % 3 == 0 || x % 3 == 0))
				level.setWallTile(x, y, "dirt");
			if(x % 3 == 0 && y % 3 == 0 && random() % 32 == 0)
				level.setWallTile(x, y, "crate");
		}
	}

	std::vector<Room> dungeonRooms;
	dungeonRooms.push_back(Room(0, 0, 3, 3));
	for(const auto &room : emptyRooms) {
		if(random() % 5 == 0 && room.halfw > 2 && room.halfh > 2) {
			bool intersects = false;
			for(const auto &prevRoom : dungeonRooms)
				intersects = intersects || room.intersects(prevRoom);

			if(!intersects) {
				dungeonRooms.push_back(room);
				createDungeonRoom(level, room, "brick", "stone_floor");
			}
		}

		int crateCount = random() % 4;
		for(int i = 0; i < crateCount; i++) {
			int x = random() % (room.halfw * 2 + 1) + room.x - room.halfw,
				y = random() % (room.halfh * 2 + 1) + room.y - room.halfh;
			if(level.getWallTile(x, y).tileId == tile("brick").tileId)
				continue;
			if(x == room.x && y == room.y)
				continue;
			level.setWallTile(x, y, "crate");
		}
	}

	std::set<std::pair<int, int>> cannotSpawn;
	for(int x = -CAVE_SIZE; x <= CAVE_SIZE; x++) {
		for(int y = -CAVE_SIZE; y <= CAVE_SIZE; y++) {
			if(!level.getWallTile(x, y).isEmpty())
				continue;
			if(cannotSpawn.count({ x, y }))
				continue;
			if(abs(x) <= 8 && abs(y) <= 8)
				continue;
			if(random() % 80 == 0) {
				genLevel.spawnEnemy(std::make_unique<Slime>(x, y));
				for(int dx = -1; dx <= 1; dx++)
					for(int dy = -1; dy <= 1; dy++)
						cannotSpawn.insert({ x + dx, y + dy });
			}
		}
	}

	return genLevel;
}

void GeneratedLevel::spawnEnemy(std::unique_ptr<Enemy> enemy) {
	enemies.push_back(std::move(enemy));
}
