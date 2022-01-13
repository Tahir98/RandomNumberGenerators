#include "Log.h"

std::shared_ptr<spdlog::logger> Log::randLogger;
std::shared_ptr<spdlog::logger> Log::appLogger;

void Log::init() {
	randLogger = spdlog::stdout_color_mt("Random Number Generator");
	randLogger->set_pattern("%^[%H:%M:%S] [%n] %v%$");
	randLogger->set_level(spdlog::level::trace);

	appLogger = spdlog::stdout_color_mt("Application");
	appLogger->set_pattern("%^[%H:%M:%S] [%n] %v%$");
	appLogger->set_level(spdlog::level::trace);
}



