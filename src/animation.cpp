#include "animation.hpp"
#include <algorithm>

AnimationValue::AnimationValue() {
	start = 0.0f;
	end = 0.0f;
	time = 0.0f;
	length = 1.0f;
}

AnimationValue::AnimationValue(float startValue, float endValue, float lengthValue) {
	start = startValue;
	end = endValue;
	time = 0.0f;
	length = lengthValue;
}

float AnimationValue::value() const {
	return (end - start) * std::clamp(time, 0.0f, 1.0f) + start;
}

void AnimationValue::update(float dt) {
	time += dt * 1.0f / length * direction;
	if(loop) {
		if(direction > 0.0f && time >= 1.0f)
			direction = -1.0f;
		else if(direction < 0.0f && time <= 0.0f)
			direction = 1.0f;
	}
	time = std::clamp(time, 0.0f, 1.0f);
}
