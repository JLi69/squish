#include "bomb.hpp"

const float BOMB_EXPLOSION_TIME = 2.0f;
const float POWER = 2.0f;

Bomb::Bomb(int px, int py) {
	initEnemy(px, py, "goo_bomb", 0.9f);

	moveEnemyTimer.active = false;
	default_offset += glm::vec2(0.05f, -0.1f);
	sprite.offset = default_offset;

	sprite.shadowScale = glm::vec2(1.0f, 0.3f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.225f);

	sprite.rotation = -20.0f;

	bloodColor = colors::SLIME_GREEN;

	gooAmt = 0.0f;
	damage = 0;
	setDir(0, 0);
	minGooCoin = 0;
	maxGooCoin = 0;

	explosionTimer = Timer(BOMB_EXPLOSION_TIME, false);
}

void Bomb::update(float dt) {
	Enemy::update(dt);
	// Explode
	if(explosionTimer.update(dt))
		health = 0;

	if(explosionTimer.currentTime < 1.5f) {
		float time = 1.5f - explosionTimer.currentTime;
		float sine = sinf(4.0f * powf(powf(3.0 * M_PI / 2.0, 1.0 / POWER) + time, POWER));
		float sample_point = (sine + 1.0) / 2.0;
		sprite.color = (1.0f - sample_point) * colors::BLACK + sample_point * colors::SLIME_GREEN * 0.5f;
	}
}

void Bomb::interactWithPlayer(Player &player) {
	if(health > 0)
		return;

	// Explode, damage player
	int distx = abs(player.x - x),
		disty = abs(player.y - y);
	if(distx > 1 || disty > 1)
		return;
	player.damage(4);
}

void Bomb::interactWithEnemies(EnemyList &enemies) {
	if(health > 0)
		return;

	// Explode, damage nearby enemies
	for(auto &enemy : enemies) {
		if(enemy == nullptr)
			continue;
		int distx = abs(enemy->x - x),
			disty = abs(enemy->y - y);
		if(distx > 1 || disty > 1)
			continue;
		enemy->health -= 4;
	}
}

void Bomb::squish(ParticleList &particles) const {
	glm::vec2 center = glm::vec2(float(x), float(y)) + sprite.offset;
	center.y += 0.1f;
	addExplosion(particles, 24, center, colors::SLIME_GREEN);
}

void Bomb::interactWithLevel(const Level &level, std::stack<LevelUpdate> &levelUpdates) {
	if(health > 0)
		return;

	for(int ix = x - 1; ix <= x + 1; ix++)
		for(int iy = y - 1; iy <= y + 1; iy++)
			levelUpdates.push(LevelUpdate(ix, iy, LevelUpdateType::DESTROY));
}
