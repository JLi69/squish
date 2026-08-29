#include "enemies.hpp"
#include "random_utils.hpp"
#include <cmath>

const glm::vec2 DEFAULT_ENEMY_OFFSET = glm::vec2(0.0f, 0.3f);

const int DIRECTION_X[] = { -1, 1,  0, 0 };
const int DIRECTION_Y[] = {  0, 0, -1, 1 };

void Enemy::setDir(int dx, int dy) {
	dirx = dx;
	diry = dy;
}

void Enemy::update(float dt) {
	Actor::update(dt);

	if(attackOffsetX.time <= 0.0f || attackOffsetY.time <= 0.0f)
		attackAnimationActive = false;

	if(attackAnimationActive) {
		attackOffsetX.update(dt);
		attackOffsetY.update(dt);
	}
	
	squishyAnimation.update(dt);
	sprite.scale.y = squishyAnimation.value();
	sprite.offset.y = default_offset.y + (sprite.scale.y - default_scale) / 2.0f;
}

void Enemy::moveEnemy(Level &level) {
	if(!level.getWallTile(x + dirx, y + diry).isEmpty() || level.isBlocked(x + dirx, y + diry))
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

int Enemy::getDirX() const {
	return dirx;
}

int Enemy::getDirY() const {
	return diry;
}

bool Enemy::uncollide(Level &level) {
	if(!isInsideTile(level))
		return true;
	if(translationAnimationActive) {
		x += dirx;
		y += diry;
		glm::vec2 displayPos = getDisplayPos();
		float timeX = translateX.length * translateX.time;
		float timeY = translateY.length * translateY.time;
		activateTranslationAnimation(displayPos.x, float(x), displayPos.y, float(y));
		translateX.length = timeX;
		translateY.length = timeY;
		dirx = 0;
		diry = 0;
		return !isInsideTile(level);
	}
	moveEnemy(level);
	return !isInsideTile(level);
}

bool Enemy::isInsideTile(const Level &level) const {
	if(!level.getWallTile(x, y).isEmpty())
		return true;
	if(level.isBlocked(x, y))
		return true;
	for(int dx = -1; dx <= 1; dx++) {
		for(int dy = -1; dy <= 1; dy++) {
			std::pair<int, int> pos = { x + dx, y + dy };
			if(!level.getPushedTiles().count(pos))
				continue;
			const PushedTile &pushedTile = level.getPushedTiles().at(pos);
			if(pushedTile.destinationX == x && pushedTile.destinationY == y)
				return true;
		}
	}
	return false;
}

void Enemy::squish(ParticleList &particles) const {
	int count = rand() % 6 + 10;
	glm::vec2 center = glm::vec2(float(x), float(y)) + sprite.offset;
	float floory = float(y) + sprite.shadowOffset.y - 0.06f;
	addBloodParticles(particles, count, center, bloodColor, floory);
}

bool Enemy::canAttackPlayer(const Player &player) {
	int dist = std::abs(player.x - x) + std::abs(player.y - y);
	return dist <= 1;
}

void Enemy::attackPlayer(Player &player) {
	if(player.isDead()) {
		attackAnimationActive = false;
		return;
	}

	attackOffsetX = AnimationValue(0.0f, (float(player.x) - float(x)) * 0.35f, 0.15f);
	attackOffsetX.loop = true;
	attackOffsetX.time = 0.01f;
	attackOffsetY = AnimationValue(0.0f, (float(player.y) - float(y)) * 0.5f, 0.15f);
	attackOffsetY.loop = true;
	attackOffsetY.time = 0.01f;
	attackAnimationActive = true;
	player.damage(damage);
}

glm::vec2 Enemy::getDisplayPos() const {
	if(attackAnimationActive) {
		glm::vec2 offset = glm::vec2(attackOffsetX.value(), attackOffsetY.value());
		return Actor::getDisplayPos() + offset;
	}
	return Actor::getDisplayPos();
}

Slime::Slime(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("slime", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.3f, 0.65f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.02f);

	default_offset = DEFAULT_ENEMY_OFFSET;
	default_scale = 0.9f;

	sprite.offset = default_offset;
	sprite.scale = glm::vec2(default_scale, default_scale);

	squishyAnimation = AnimationValue(default_scale - 0.1f, default_scale + 0.1f, 0.5f);
	squishyAnimation.time = randf_range(0.0f, 1.0f);
	squishyAnimation.loop = true;

	moveEnemyTimer = Timer(1.0f, true);
	moveEnemyTimer.currentTime = randf_range(0.0f, 1.0f);
	attackTimer = Timer(0.75f, true);

	bloodColor = colors::SLIME_GREEN;
}

void Slime::updateDir(const Level &level, const Player &player) {
	int distx = std::abs(player.x - x);
	int disty = std::abs(player.y - y);

	if(distx > 10 || disty > 10 || (distx == 0 && disty == 0)) {
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
