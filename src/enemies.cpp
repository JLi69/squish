#include "enemies.hpp"

const glm::vec2 DEFAULT_ENEMY_OFFSET = glm::vec2(0.0f, 0.1f);

const int DIRECTION_X[] = { -1, 1,  0, 0 };
const int DIRECTION_Y[] = {  0, 0, -1, 1 };

void Enemy::setDir(int dx, int dy) {
	dirx = dx;
	diry = dy;
}

void Enemy::update(float dt) {
	Actor::update(dt);
	
	squishyAnimation.update(dt);
	sprite.scale.y = squishyAnimation.value();
	sprite.offset.y = default_offset.y + (sprite.scale.y - default_scale) / 2.0f;
}

bool Enemy::updateMoveTimer(float dt) {
	if(translationAnimationActive)
		return false;

	moveEnemyTimer -= dt;
	
	if(moveEnemyTimer <= 0.0f) {
		moveEnemyTimer = moveEnemyInterval;
		return true;
	}

	return false;
}

void Enemy::moveEnemy(Level &level) {
	if(!level.getWallTile(x + dirx, y + diry).isEmpty())
		return;
	float prevx = float(x);
	float prevy = float(y);
	x += dirx;
	y += diry;
	activateTranslationAnimation(prevx, float(x), prevy, float(y));
}

void Enemy::updateDir(const Level &level, const Player &player) {
	// Do nothing
}

Slime::Slime(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("slime", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.3f, 0.65f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.22f);

	default_offset = DEFAULT_ENEMY_OFFSET;
	default_scale = 0.9f;

	sprite.offset = default_offset;
	sprite.scale = glm::vec2(default_scale, default_scale);

	squishyAnimation = AnimationValue(default_scale - 0.1f, default_scale + 0.1f, 0.5f);
	squishyAnimation.time = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	squishyAnimation.loop = true;

	moveEnemyInterval = 1.0f;
	moveEnemyTimer = moveEnemyInterval;
}

void Slime::updateDir(const Level &level, const Player &player) {
	int distx = std::abs(player.x - x);
	int disty = std::abs(player.y - y);

	if(distx > 16 || disty > 16 || (distx == 0 && disty == 0)) {
		setDir(0, 0);
		return;
	}

	if(dirx == 0 && diry == 0) {
		int index = rand() % 4;
		setDir(DIRECTION_X[index], DIRECTION_Y[index]);
		return;
	}
	
	if(y == player.y && x < player.x && distx <= 8)
		setDir(1, 0);
	else if(y == player.y && x > player.x && distx <= 8)
		setDir(-1, 0);
	else if(x == player.x && y < player.y && disty <= 8)
		setDir(0, 1);
	else if(x == player.x && y > player.y && disty <= 8)
		setDir(0, -1);

	if(level.getWallTile(x + dirx, y + diry).isEmpty())
		return;

	int index = rand() % 4;
	setDir(DIRECTION_X[index], DIRECTION_Y[index]);
}
