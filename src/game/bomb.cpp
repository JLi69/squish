#include "bomb.hpp"

const float BOMB_EXPLOSION_TIME = 2.0f;

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
		int distx = abs(enemy->x - x),
			disty = abs(enemy->y - y);
		if(distx > 1 || disty > 1)
			continue;
		enemy->health -= 4;
	}
}

void Bomb::squish(ParticleList &particles) const {
	glm::vec2 center = glm::vec2(float(x), float(y)) + sprite.offset;
	addExplosion(particles, 24, center, colors::SLIME_GREEN);
}
