#pragma once

#include "Scene.h"

#include <vector>
#include <memory>

class SceneManager {
private:
	static std::vector<Scene*> scenes;
	friend class Scene;
public:
	SceneManager();
	~SceneManager();

	static void push(Scene* scene);
	static void SceneManager::push(Scene* scene, bool deletePreviousScene);

	static void pop();

	static void update(float delta);

	static bool top(Scene* scene);

	static void clearAll();
};


