#pragma once

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// The default size of the game window
const int DEFAULT_WIDTH = 1200;
const int DEFAULT_HEIGHT = 675;

typedef int KeyCode;

enum InputState {
	RELEASED,
	JUST_PRESSED,
	PRESSED,
};

bool isPressed(InputState state);

// Kills the game and outputs an error message to stderr
void die(const char *msg);

GLFWwindow *initWindow();
void handleWindowResize(GLFWwindow *window, int newWidth, int newHeight);
glm::mat4 calculateWindowMat(float w, float h, float scale);
void updateInputStates();
InputState getKeyInputState(KeyCode keycode);
