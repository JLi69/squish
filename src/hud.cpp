#include "game.hpp"
#include <algorithm>

void HudData::updateDisplayGooBar(float dt, float gooBarProgress) {
	const float MIN_GOO_BAR_UPDATE_SPEED = 0.05f;
	if(displayGooBarProgress < gooBarProgress) {
		float diff = gooBarProgress - displayGooBarProgress;
		float speed = std::max(diff, MIN_GOO_BAR_UPDATE_SPEED) * 4.0f;
		displayGooBarProgress += std::min(dt * speed, diff);
	}
	else if(displayGooBarProgress > gooBarProgress) {
		float diff = displayGooBarProgress - gooBarProgress;
		float speed = std::max(diff, MIN_GOO_BAR_UPDATE_SPEED) * 4.0f;
		displayGooBarProgress -= std::min(dt * speed, diff);
	}

	displayGooBarProgress = std::clamp(displayGooBarProgress, 0.0f, 1.0f);
}

void HudData::updateGooCoinDisplay(float dt, unsigned int gooCoinCount) {
	float diff = float(gooCoinCount) - gooCoinDisplayNum;
	if(std::abs(diff) < 0.1f) {
		gooCoinDisplayNum = float(gooCoinCount);
		return;
	}
	
	float speed = std::max(std::abs(diff) * 2.0f, 12.0f);
	if(diff > 0.0f)
		gooCoinDisplayNum += speed * dt;
	else
		gooCoinDisplayNum -= speed * dt;

	if(diff < 0.0f && gooCoinDisplayNum < float(gooCoinCount))
		gooCoinDisplayNum = float(gooCoinCount);
	else if(diff > 0.0f && gooCoinDisplayNum > float(gooCoinCount))
		gooCoinDisplayNum = float(gooCoinCount);
}

void Game::updateHud(float dt) {
	hud.updateDisplayGooBar(dt, player.getGooBarProgress());
	hud.updateGooCoinDisplay(dt, player.gooCoins);

	hud.gooBombScale = std::max(1.0f, hud.gooBombScale - dt * 0.75f);
	hud.gooCoinScale = std::max(1.0f, hud.gooCoinScale - dt * 0.75f);
}
