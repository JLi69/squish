#include "particles.hpp"
#include "random_utils.hpp"

void Particle::update(float dt) {
	// Do nothing
}

bool Particle::finished() const {
	return false;
}

BloodParticle::BloodParticle(glm::vec2 pos, glm::vec4 col, float y) {
	sprite = Sprite("particle", glm::vec2(0.0f));
	position = pos;
	color = col;
	floory = y;
	sprite.rotation = randf_range(0.0f, 360.0f);
	float scale = randf_range(0.15f, 0.25f);
	sprite.scale = glm::vec2(scale);
	sprite.shadowScale = glm::vec2(scale * 1.3f, 0.15f);
	sprite.shadowOffset = glm::vec2(0.0f, -scale / 2.0f * 0.9f);

	defaultShadowScale = sprite.shadowScale;
	defaultSpriteScale = sprite.scale;
}

void BloodParticle::update(float dt) {
	if(glm::length(vel) <= 0.0f && currentScale > 0.0f) {
		currentScale -= dt;
		sprite.shadowScale = currentScale * defaultShadowScale;
		sprite.scale = currentScale * defaultSpriteScale;
		currentScale = fmax(currentScale, 0.0f);
		sprite.offset.y = -(defaultSpriteScale.y - currentScale * defaultSpriteScale.y) / 2.0f;
		return;
	}

	position += vel * dt;
	if(glm::length(vel) > 0.0f)
		vel.y -= 32.0f * dt;
	position.y = fmax(floory, position.y);
	if(position.y <= floory)
		vel = glm::vec2(0.0f);
}

bool BloodParticle::finished() const {
	return currentScale <= 0.0f;
}

void addBloodParticles(
	ParticleList &particles,
	int count,
	glm::vec2 center,
	Color bloodColor,
	float floory
) {
	for(int i = 0; i < count; i++) {
		float angle = randf_range(0.0f, 2.0f * M_PI);
		float dist = randf_range(0.0f, 0.3f);
		glm::vec2 offset = glm::vec2(cos(angle), sin(angle)) * dist;
		std::unique_ptr<BloodParticle> particle = std::make_unique<BloodParticle>(
			center + offset,
			bloodColor,
			floory
		);
		particle->vel = glm::vec2(cos(angle) * 0.5f, sin(angle)) * 6.0f;
		particles.push_back(std::move(particle));
	}
}
