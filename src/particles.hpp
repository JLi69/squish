#pragma once

#include "sprite.hpp"
#include <vector>
#include <memory>

struct Particle {
	Sprite sprite = Sprite("", glm::vec2(0.0f));
	glm::vec2 position = glm::vec2(0.0f);
	glm::vec4 color = glm::vec4(1.0f);
	float floory = 0.0f;

	virtual void update(float dt);
	virtual bool finished() const;
};

struct BloodParticle : Particle {
	float currentScale = 1.0f;
	glm::vec2 defaultShadowScale;
	glm::vec2 defaultSpriteScale;
	glm::vec2 vel = glm::vec2(0.0f);
	BloodParticle(glm::vec2 pos, glm::vec4 col, float y);
	void update(float dt) override;
	bool finished() const override;
};

typedef std::vector<std::unique_ptr<Particle>> ParticleList;
