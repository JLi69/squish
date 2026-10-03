#include "app.hpp"
#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

static std::unordered_map<KeyCode, InputState> inputStates; 

bool isPressed(InputState state) {
	return state == JUST_PRESSED || state == RELEASED;
}

void die(const char *msg) {
	fprintf(stderr, "%s", msg);
	exit(1);
}

void handleWindowResize(GLFWwindow *window, int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight);
}

void handleKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS)
		inputStates[key] = InputState::JUST_PRESSED;
	else if(action == GLFW_RELEASE)
		inputStates[key] = InputState::RELEASED;
}

glm::mat4 calculateWindowMat(float w, float h, float scale) {
	glm::mat4 windowMat = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(
			float(DEFAULT_HEIGHT) / float(DEFAULT_WIDTH) * DEFAULT_WIDTH / w, 
			DEFAULT_HEIGHT / h, 
			1.0f
		)
	);

	if(w * DEFAULT_HEIGHT / DEFAULT_WIDTH > h) {
		windowMat = glm::scale(
			windowMat,
			glm::vec3(w / DEFAULT_WIDTH, w / DEFAULT_WIDTH, 1.0f)
		);
	}
	else {
		windowMat = glm::scale(
			windowMat,
			glm::vec3(h / DEFAULT_HEIGHT, h / DEFAULT_HEIGHT, 1.0f)
		);
	}

	windowMat = glm::scale(windowMat, glm::vec3(scale, scale, 1.0f));
	
	return windowMat;
}

GLFWwindow *initWindow() {
	if(!glfwInit())
		die("Failed to initialize GLFW!");

	// Initialize the window
	GLFWwindow *window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Squish", NULL, NULL);
	if(!window)
		die("Failed to create window!");
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, handleWindowResize);
	glfwSetKeyCallback(window, handleKeyInput);

	// Initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		die("Failed to load glad!");

	return window;
}

void updateInputStates() {
	for(auto &inputState : inputStates) {
		KeyCode keycode = inputState.first;
		InputState state = inputState.second;
		if(state == InputState::JUST_PRESSED)
			inputState.second = InputState::PRESSED;
	}
}

InputState getKeyInputState(KeyCode keycode) {
	if(!inputStates.count(keycode))
		return InputState::RELEASED;
	return inputStates[keycode];
}
