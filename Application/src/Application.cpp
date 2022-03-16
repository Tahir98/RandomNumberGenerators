#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scenes/DrawMenu.h" 

#include "Utility/Properties.h"
#include <ImGui/ImGuiLayer.h>
#include "Scene/SceneManager.h"
#include "Scenes/TerrainGeneration.h"
#include "Scenes/TerrainGeneration.h"
#include "RandomNumberGenerators.h"
#include "Scenes/Performance.h"
#include "Scenes/RNGs.h"
#include "Scenes/Cryptography.h"
#include "Scenes/Distribution.h"
//

int main() {
	Log::init();

	if (glfwInit() != GLFW_TRUE) {
		APP_LOG_ERROR("Failed to initialize glfw");
		return 0;
	}

	APP_LOG_INFO("glfw initialized successfully");

	Window::properties.width = 1920;
	Window::properties.height = 1080;
	Window::properties.name = "Random Number Generators";
	Window::properties.fullscreen = true;
	Window::properties.vsync = true;
	Window::properties.setFps = false;

	glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(Window::properties.width, Window::properties.height, Window::properties.name.c_str(), Window::properties.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	if (window == nullptr) {
		APP_LOG_ERROR("Failed to create window");
		return 0;
	}

	APP_LOG_INFO("Window created, name:{0}, {1},{2}, Fullscreen:{3},Vsync:{4}", Window::properties.name, Window::properties.width, Window::properties.height, Window::properties.fullscreen, Window::properties.vsync);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	
	glfwSwapInterval(Window::properties.vsync ? 1 : 0);
	glfwSetWindowUserPointer(window, &Window::properties);

	ImGuiLayer::init(window);

	float prev = 0, now = 0;
	float delta;

	//SceneManager::push(new PerformanceScene("Deneme",window));
	//SceneManager::push(new RNG("RNG", window));
	SceneManager::push(new TerrainGeneration("First Example", window));
	//SceneManager::push(new Cryptography("Cryptography", window));
	//SceneManager::push(new Distribution("Cryptography", window));


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor( 1, 1, 1, 1);

		glfwPollEvents();
		ImGuiLayer::beginFrame();
		DrawMenu::begin(window);
		if (prev == 0) {
			delta = 1;
			prev = now = (float)glfwGetTime();
		}
		else {
			prev = now;
			now = (float)glfwGetTime();
			delta = now - prev;
		}
		
		if (Window::properties.setFps) {
			std::string title = Window::properties.name + ", FPS: " + std::to_string(1.0f / delta);
			glfwSetWindowTitle(window,title.c_str());
		}
		else {
			glfwSetWindowTitle(window, Window::properties.name.c_str());
		}
		
		SceneManager::update(delta);
		DrawMenu::end();
		ImGuiLayer::renderFrame();
		glfwSwapBuffers(window);
	}

	SceneManager::clearAll();

	ImGuiLayer::terminate();
	glfwDestroyWindow(window);
	APP_LOG_INFO("Window destoreyed, name:{0}, {1},{2}, Fullscreen:{3},Vsync:{4}", Window::properties.name, Window::properties.width, Window::properties.height, Window::properties.fullscreen, Window::properties.vsync);
	glfwTerminate();
	APP_LOG_INFO("glfw terminated");
	
	return 0;
}