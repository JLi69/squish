#include "particles.hpp"
#include "random_utils.hpp"

void Particle::update(float dt) {
	time += dt;
	position += vel * dt;
	if(glm::length(vel) > 0.0f)
		vel.y -= gravity * dt;

	if(floorAffected) {
		position.y = fmax(floory, position.y);
		if(position.y <= floory)
			vel = glm::vec2(0.0f);
	}
}

bool Particle::finished() const {
	return false;
}

SplatParticle::SplatParticle(glm::vec2 pos, glm::vec4 col, float y) {
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

void SplatParticle::update(float dt) {
	Particle::update(dt);

	if(glm::length(vel) <= 0.0f && currentScale > 0.0f) {
		currentScale -= dt;
		currentScale = fmax(currentScale, 0.0f);
		sprite.shadowScale = currentScale * defaultShadowScale;
		sprite.scale = currentScale * defaultSpriteScale;
		sprite.offset.y = -(defaultSpriteScale.y - currentScale * defaultSpriteScale.y) / 2.0f;
		return;
	}	
}

bool SplatParticle::finished() const {
	return currentScale <= 0.0f;
}

ExplosionParticle::ExplosionParticle(glm::vec2 pos, Color col) {
	sprite = Sprite("particle", glm::vec2(0.0f));
	sprite.onTop = true;
	sprite.drawShadow = false;
	position = pos;
	color = col;
	floory = 0.0f;
	sprite.rotation = randf_range(0.0f, 360.0f);
	float scale = randf_range(0.6f, 1.0f);
	sprite.scale = glm::vec2(scale);
	sprite.shadowScale = glm::vec2(scale * 1.3f, 0.15f);
	sprite.shadowOffset = glm::vec2(0.0f, -scale / 2.0f * 0.9f);

	defaultShadowScale = sprite.shadowScale;
	defaultSpriteScale = sprite.scale;

	gravity = 0.0f;
	floorAffected = false;

	if(randf() < 0.5f)
		rotationDir = -1.0f;
}

void ExplosionParticle::update(float dt) {
	Particle::update(dt);

	sprite.rotation += 180.0f * rotationDir * dt;
	if(8.0f * dt > 1.0f)
		vel = glm::vec3(0.0f);
	else
		vel -= vel * 12.0f * dt;
	currentScale = ((-time * (time - 0.75f)) * 10.0f + 1.0f) * defaultSpriteScale.x;
	currentScale = fmax(currentScale, 0.0f);
	sprite.scale = currentScale * defaultSpriteScale;
}

bool ExplosionParticle::finished() const {
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
		std::unique_ptr<SplatParticle> particle = std::make_unique<SplatParticle>(
			center + offset,
			bloodColor,
			floory
		);
		particle->vel = glm::vec2(cos(angle) * 0.5f, sin(angle)) * 6.0f;
		particles.push_back(std::move(particle));
	}
}

void addExplosion(ParticleList &particles, int count, glm::vec2 center, Color color) {
	for(int i = 0; i < count; i++) {
		float angle = randf_range(0.0f, 2.0f * M_PI);
		float dist = randf_range(0.0f, 0.2f);
		glm::vec2 offset = glm::vec2(cos(angle), sin(angle)) * dist;
		Color modifiedCol = color * Color(randf_range(1.0f, 2.0f), 1.0f, 1.0f, 1.0f);
		std::unique_ptr<ExplosionParticle> particle = std::make_unique<ExplosionParticle>(
			center + offset,
			modifiedCol
		);
		particle->vel = glm::vec2(cos(angle), sin(angle)) * 10.0f;
		particles.push_back(std::move(particle));
	}
}

void addTileParticles(ParticleList &particles, int count, glm::vec2 center, Tile tile) {
	for(int i = 0; i < count; i++) {
		float dx = randf_range(-0.9f, -0.1f);
		float dy = randf_range(-0.4f, 0.4f);
		glm::vec2 offset = glm::vec2(dx, dy);
		std::unique_ptr<SplatParticle> particle = std::make_unique<SplatParticle>(
			center + offset,
			Color(0.9f, 0.9f, 0.9f, 1.0f),
			center.y + offset.y - 0.6f
		);
		
		particle->texture = "tiles";
		particle->textureOffset = tile.getTexOffset();	
		glm::vec2 texOffset = glm::vec2(randf_range(0.0f, 0.6f), randf_range(0.0f, 0.5f)) / 16.0f;
		particle->textureOffset /= 16.0f;
		particle->textureOffset += texOffset;
		particle->textureScale /= (16.0f * 2.0f);
		
		particle->vel = glm::normalize(glm::vec2(dx + 0.5f, randf_range(0.2f, 0.3f))) * 2.0f;
		particles.push_back(std::move(particle));
	}
}
