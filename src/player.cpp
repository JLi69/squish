#include "player.hpp"

const float PLAYER_DAMAGE_FLASH_TIME = 0.5f;

Player::Player(int px, int py) {
	x = px;
	y = py;
	sprite = Sprite("player", glm::vec2(0.0f, 0.0f));
	
	sprite.shadowScale = glm::vec2(1.2f, 0.5f);
	sprite.shadowOffset = glm::vec2(0.0f, -0.1f);

	sprite.offset = DEFAULT_PLAYER_OFFSET;
	sprite.scale = glm::vec2(DEFAULT_PLAYER_SCALE, DEFAULT_PLAYER_SCALE);

	squishyAnimation = AnimationValue(
		DEFAULT_PLAYER_SCALE - 0.05f,
		DEFAULT_PLAYER_SCALE + 0.05f,
		PLAYER_SQUISH_ANIMATION_LENGTH
	);
	squishyAnimation.loop = true;
}

void Player::update(float dt) {
	Actor::update(dt);

	playerDamageTimer = std::max(playerDamageTimer - dt, 0.0f);
	
	squishyAnimation.update(dt);
	sprite.scale.y = squishyAnimation.value();
	sprite.offset.y = DEFAULT_PLAYER_OFFSET.y + (sprite.scale.y - DEFAULT_PLAYER_SCALE) / 2.0f;
}

int Player::getHealth() const {
	return health;
}

void Player::damage(int amt) {
	health = std::max(health - amt, 0);
	playerDamageTimer = PLAYER_DAMAGE_FLASH_TIME;
}

bool Player::isDead() const {
	return health <= 0;
}

void Player::explode(ParticleList &particles) {
	glm::vec2 center = glm::vec2(float(x), float(y)) + sprite.offset;
	float floory = float(y) + sprite.shadowOffset.y - 0.06f;
	addBloodParticles(particles, 24, center, colors::SLIME_GREEN, floory);
}

Color Player::getColor() const {
	float t = playerDamageTimer / PLAYER_DAMAGE_FLASH_TIME;
	return colors::RED * t + colors::BLACK * (1.0f - t);
}

Color Player::getMultColor() const {
	float t = playerDamageTimer / PLAYER_DAMAGE_FLASH_TIME;
	return Color(1.0f, 0.4f, 0.4f, 1.0f) * t + colors::WHITE * (1.0f - t);
}
