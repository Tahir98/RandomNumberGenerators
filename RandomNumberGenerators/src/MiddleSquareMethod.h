#pragma once
#include "Random.h"
#include <chrono>
#define MAX_DIGIT_COUNT 10

class MiddleSquareMethod : public Random {
private:
	uint32_t value;
public:
	MiddleSquareMethod (bool useTimeAsSeed = true){
		if (useTimeAsSeed) {
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

			auto time = now.time_since_epoch();
			auto time_cast = std::chrono::duration_cast<std::chrono::nanoseconds>(time);
			value = time_cast.count();
		}
		else {
			value = 1234554321;
		}
	}

	MiddleSquareMethod (const uint32_t seed) {
		value = seed;
	}

	~MiddleSquareMethod() {

	}

	//Set seed for algorithm
	void setSeed(const uint32_t seed) override {
		value = seed;
	}

	
	uint32_t nextUInt() override {
		uint64_t squared = value * (uint64_t)value;
		int count = digitCount(squared);

		uint64_t temp = squared;
		for (int i = 0; i < MAX_DIGIT_COUNT / 2; i++) {
			temp /= 10;
		}

		uint32_t digits[MAX_DIGIT_COUNT];

		for (int i = 0; i < MAX_DIGIT_COUNT; i++) {
			digits[i] = (uint32_t)(temp - temp/10);
			temp /= 10;
		}

		value = 0;
		for (int i = 0; i < MAX_DIGIT_COUNT; i++) {
			value = value * 10 + digits[MAX_DIGIT_COUNT - 1 - i];	
		}

		return value;
	}

	// this method produces 32bit random unsigned integer, range[min,max]
	uint32_t nextUInt(const uint32_t min, const uint32_t max) override {
		if (min >= max) {
			RAND_LOG_ERROR("Middle Square Method: Max must be bigger than min");
			return 0;
		}

		uint64_t mul_value = value * value;
		int count = digitCount(mul_value);

		uint64_t temp = mul_value;
		for (int i = 0; i < MAX_DIGIT_COUNT / 2; i++) {
			temp /= 10;
		}

		uint32_t digits[MAX_DIGIT_COUNT];

		for (int i = 0; i < MAX_DIGIT_COUNT; i++) {
			digits[i] = (uint32_t)(temp - temp / 10);
			temp /= 10;
		}

		value = 0;
		for (int i = 0; i < MAX_DIGIT_COUNT; i++) {
			value = value * 10 + digits[MAX_DIGIT_COUNT - 1 - i];
		}

		//std::cout << "New value: " << value << std::endl;
		uint32_t diff = max - min + 1;
		return (uint32_t)(min + value % diff);
	}

private:
	//Some utility functions
	uint32_t digitCount(uint32_t value) {
		uint32_t count = 0;

		while (true) {
			if (value == 0)
				break;
			count++;
			value /= 10;
		}

		return count;
	}

	uint32_t digitCount(uint64_t value) {
		uint32_t count = 0;

		while (true) {
			if (value == 0)
				break;
			count++;
			value /= 10;
		}

		return count;
	}
};