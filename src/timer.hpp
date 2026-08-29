#pragma once

struct Timer {
	float currentTime, interval;
	bool loop;
	Timer();
	Timer(float intervalAmt, bool shouldLoop);
	// Returns true whenever the timer triggers
	bool update(float dt);
};
