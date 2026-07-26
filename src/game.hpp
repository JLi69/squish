#pragma once

#include <string>
#include <glm/glm.hpp>
#include <stack>
#include "level.hpp"
#include "tilemap_gfx.hpp"

struct Sprite {
	std::string spriteTexId = "";
	// Transform
	glm::vec2 offset = glm::vec2(0.0f);
	glm::vec2 scale = glm::vec2(1.0f);
	// Shadow transform
	glm::vec2 shadowScale = glm::vec2(1.0f);
	glm::vec2 shadowOffset = glm::vec2(0.0f);
	bool flip = false;
	bool drawShadow = true;
	Sprite(const std::string &texId, glm::vec2 pos);
};

struct AnimationValue {
	bool loop = false;
	float direction = 1.0f;
	float start, end;
	// Ranges from 0.0 to 1.0
	float time;
	// Length of animation in seconds
	float length;
	AnimationValue();
	AnimationValue(float startValue, float endValue, float lengthValue);
	float value() const;
	void update(float dt);
};

struct Player {
	Sprite sprite = Sprite("", glm::vec2(0.0f, 0.0f));
	// Position of the player
	int x = 0, y = 0;
	AnimationValue squishyAnimation;
	// Animation for moving
	AnimationValue translateX, translateY;
	bool translationAnimationActive = false;

	// Constructor
	Player(int px, int py);

	void update(float dt);
	void activateTranslationAnimation(float x1, float x2, float y1, float y2);
};

class Game {
	Level level = Level(-8, -8, 8, 8);
	Player player = Player(0, 0);
	TileVaos tileVaos;

	void movePlayer();
	// Returns true if a block was able to be succesfully pushed
	bool pushBlocks(int prevx, int prevy, int &x, int &y);

	std::stack<std::pair<int, int>> chunksToUpdate;
public:
	Level &getLevel();
	Player &getPlayer();
	const TileVaos &getTileVaos() const;
	void initTestLevel();

	void update(float dt);
	void updateChunkVaos();
};

bool canPush(const Level &level, int x, int y, int dirx, int diry);
