#pragma once

#include <stack>
#include <glm/glm.hpp>
#include "level.hpp"
#include "tilemap_gfx.hpp"
#include "player.hpp"

struct Camera2D {
	const float MIN_FOLLOW_DIST = 0.05f;
	glm::vec2 pos = glm::vec2(0.0f);
	float zoom = 1.0f;
	float followSpeed = 6.0f;
	glm::mat4 getMat() const;
	void follow(float dt, glm::vec2 followPos);
};

class Game {
	Level level = Level(-8, -8, 8, 8);
	Player player = Player(0, 0);
	TileVaos tileVaos;
	Camera2D camera = Camera2D();

	void movePlayer();
	// Returns true if a block was able to be succesfully pushed
	bool pushBlocks(int prevx, int prevy, int &x, int &y);

	std::stack<std::pair<int, int>> chunksToUpdate;
public:
	Level &getLevel();
	Player &getPlayer();
	const TileVaos &getTileVaos() const;
	void initTestLevel();
	void initCaveLevel();

	void update(float dt);
	void updateChunkVaos();

	Camera2D &getCamera();
};

bool canPush(const Level &level, int x, int y, int dirx, int diry);
