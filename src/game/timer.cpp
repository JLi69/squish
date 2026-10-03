#include "timer.hpp"

Timer::Timer() {
	currentTime = 0.0f;
	interval = 0.0f;
	loop = false;
}

Timer::Timer(float intervalAmt, bool shouldLoop) {
	currentTime = intervalAmt;
	interval = intervalAmt;
	loop = shouldLoop;
}

bool Timer::update(float dt) {
	if(currentTime < 0.0f && !loop)
		return false;
	currentTime -= dt;
	if(currentTime > 0.0f)
		return false;
	if(loop)
		currentTime = interval;
	return true;
}
