#pragma once

// In seconds
const float PLAYER_SQUISH_ANIMATION_LENGTH = 0.5f;
const float TRANSLATION_ANIMATION_LENGTH = 0.125f;

struct AnimationValue {
	bool loop = false;
	float direction = 1.0f;
	float start, end;
	// Ranges from 0.0 to 1.0
	float time;
	// Length of animation in seconds
	float length;
	AnimationValue();
	AnimationValue(float startValue, float endValue, float lengthValue);
	float value() const;
	void update(float dt);
};
