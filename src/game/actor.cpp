#include "actor.hpp"

void Actor::activateTranslationAnimation(float x1, float x2, float y1, float y2) {
	translateX = AnimationValue(x1, x2, TRANSLATION_ANIMATION_LENGTH);
	translateY = AnimationValue(y1, y2, TRANSLATION_ANIMATION_LENGTH);
	translationAnimationActive = true;
}

glm::vec2 Actor::getDisplayPos() const {
	if(translationAnimationActive)
		return glm::vec2(translateX.value(), translateY.value());
	return glm::vec2(float(x), float(y));
}

void Actor::update(float dt) {
	if(translationAnimationActive) {
		translateX.update(dt);
		translateY.update(dt);
	}
	if(translateX.time >= 1.0f || translateY.time >= 1.0f)
		translationAnimationActive = false;
}
