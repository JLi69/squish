#pragma once

#include "tilemap_gfx.hpp"
#include "game.hpp"

const float DEFAULT_ZOOM = 0.175f;

// Returns the z index of a sprite based on the maximum and minimum y coordinates
float getZFromY(float y, float topy, float boty);

// Activates the shader that we want to use and also feeds in the window matrix
// as a uniform to the shader
void setupShader(const std::string &shader, int w, int h, float zoom);
// Displays the tiles in a level
void displayLevel(const TileVaos &tileVaos);
// Displays a sprite at a position (along with a shadow beneath the sprite)
void displaySprite(const Sprite &sprite, glm::vec2 pos, const Level &level);
