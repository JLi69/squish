#include "player.hpp"

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
	if(translationAnimationActive) {
		translateX.update(dt);
		translateY.update(dt);
	}
	if(translateX.time >= 1.0f || translateY.time >= 1.0f)
		translationAnimationActive = false;
	
	squishyAnimation.update(dt);
	sprite.scale.y = squishyAnimation.value();
	sprite.offset.y = DEFAULT_PLAYER_OFFSET.y + (sprite.scale.y - DEFAULT_PLAYER_SCALE) / 2.0f;
}

void Player::activateTranslationAnimation(float x1, float x2, float y1, float y2) {
	translateX = AnimationValue(x1, x2, TRANSLATION_ANIMATION_LENGTH);
	translateY = AnimationValue(y1, y2, TRANSLATION_ANIMATION_LENGTH);
	translationAnimationActive = true;
}

glm::vec2 Player::getDisplayPos() const {
	if(translationAnimationActive)
		return glm::vec2(translateX.value(), translateY.value());
	return glm::vec2(float(x), float(y));
}
