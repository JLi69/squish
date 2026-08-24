#pragma once

#include "sprite.hpp"
#include "animation.hpp"
#include <glm/glm.hpp>

class Actor {
public:
	Sprite sprite = Sprite("", glm::vec2(0.0f, 0.0f));
	// Position
	int x = 0, y = 0;
	// Animation for moving
	AnimationValue translateX, translateY;
	bool translationAnimationActive = false;

	virtual void update(float dt);
	void activateTranslationAnimation(float x1, float x2, float y1, float y2);
	glm::vec2 getDisplayPos() const;
};
