#include "game.hpp"
#include <algorithm>

void HudData::updateDisplayGooBar(float dt, float gooBarProgress) {
	const float MIN_GOO_BAR_UPDATE_SPEED = 0.1f;
	if(displayGooBarProgress < gooBarProgress) {
		float diff = gooBarProgress - displayGooBarProgress;
		float speed = std::max(diff, MIN_GOO_BAR_UPDATE_SPEED) * 3.0f;
		displayGooBarProgress += std::min(dt * speed, diff);
	}
	else if(displayGooBarProgress > gooBarProgress) {
		float diff = displayGooBarProgress - gooBarProgress;
		float speed = std::max(diff, MIN_GOO_BAR_UPDATE_SPEED) * 3.0f;
		displayGooBarProgress -= std::min(dt * speed, diff);
	}

	displayGooBarProgress = std::clamp(displayGooBarProgress, 0.0f, 1.0f);
}

void Game::updateHud(float dt) {
	hud.updateDisplayGooBar(dt, player.getGooBarProgress());
}
