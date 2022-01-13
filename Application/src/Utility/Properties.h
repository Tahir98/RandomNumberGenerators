#pragma once
#include <string>

struct Properties {
	int width, height;
	std::string name;
	bool fullscreen;
	bool vsync;
	bool setFps;
};

class Window {
public:
	static Properties properties;
};
