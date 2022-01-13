#pragma once

#include <GLFW/glfw3.h>
#include "Math/Math.h" 

class InputHandler {
	GLFWwindow* window;
	Vec2 mousPos;
public:
	static void init();

	InputHandler(GLFWwindow* window);
	~InputHandler();

	bool getKeyState(int key);
	bool getMouseButtonState(int button);

	Vec2 getMousePos();
};


