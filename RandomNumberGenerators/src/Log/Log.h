#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>


class Log {
public:
	static std::shared_ptr<spdlog::logger> randLogger;
	static std::shared_ptr<spdlog::logger> appLogger;
	static void init();
};


#define RAND_LOG_WARN(...)   Log::randLogger->warn(__VA_ARGS__)
#define RAND_LOG_ERROR(...)  Log::randLogger->error(__VA_ARGS__)
#define RAND_LOG_INFO(...)   Log::randLogger->info(__VA_ARGS__)
#define RAND_LOG_TRACE(...)  Log::randLogger->trace(__VA_ARGS__)


#define APP_LOG_WARN(...)   Log::appLogger->warn(__VA_ARGS__)
#define APP_LOG_ERROR(...)  Log::appLogger->error(__VA_ARGS__)
#define APP_LOG_INFO(...)   Log::appLogger->info(__VA_ARGS__)
#define APP_LOG_TRACE(...)  Log::appLogger->trace(__VA_ARGS__)