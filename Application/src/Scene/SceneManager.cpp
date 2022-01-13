#include "SceneManager.h"
#include "Log/Log.h"


std::vector<Scene*> SceneManager::scenes;

SceneManager::SceneManager() {

}

SceneManager::~SceneManager() {
	for (unsigned int i = 0; i < scenes.size(); ) {
		scenes[scenes.size() - 1]->onDestroy();
		delete scenes[scenes.size() - 1];
		scenes.pop_back();
	}
}

void SceneManager::push(Scene* scene) {
	if(scenes.size() > 0)
		scenes[scenes.size() - 1]->onSuspend();

	if (scenes.size() == 0)
		Scene::count = 0;

	scenes.push_back(scene);
	scene->id = Scene::count++;
	scene->onCreate();
}

void SceneManager::push(Scene* scene,bool deletePreviousScene) {
	if (scenes.size() > 0)
		scenes[scenes.size() - 1]->onSuspend();

	if (scenes.size() == 0)
		Scene::count = 0;

	scenes.push_back(scene);
	scene->id = Scene::count++;
	scene->onCreate();
	
	if (deletePreviousScene && scenes.size() >= 2) {
		scenes[scenes.size() - 2]->onDestroy();
		delete scenes[scenes.size() - 2];
		scenes.erase(scenes.begin() + scenes.size() - 2);
	}
	
}

void SceneManager::pop() {
	scenes[scenes.size() - 1]->onDestroy();
	delete scenes[scenes.size() - 1];
	scenes.pop_back();

	scenes[scenes.size() - 1]->onResume();
}

void SceneManager::update(float delta) {
	scenes[scenes.size() - 1]->onUpdate(delta);
}

bool SceneManager::top(Scene* scene) {
	int index = -1;
	for (unsigned int i = 0; i < scenes.size() - 2; i++) {
		if (scene->id == scenes[i]->id) {
			index = i;
			break;
		}
	}

	if (index != -1) {
		scenes[scenes.size() - 1]->onSuspend();

		scenes.erase(scenes.begin() + index);
		scenes.push_back(scene);
		scenes[scenes.size() - 1]->onResume();
		return true;
	}
	else {
		APP_LOG_ERROR("Scene Manager Error: Scene not found, name: {0}", scene->name);
		return false;
	}

	return true;
}

void SceneManager::clearAll() {
	for(unsigned int i = 0; i < scenes.size(); i++) {
		scenes[i]->onDestroy();
		APP_LOG_INFO("{0} scene deleted", scenes[i]->name);
		delete scenes[i];
	}

	scenes.clear();
}
