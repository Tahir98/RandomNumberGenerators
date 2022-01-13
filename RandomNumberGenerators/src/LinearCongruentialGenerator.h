#pragma once
#include "Random.h"
#include <chrono>

class LinearCongruentialGenerator : public Random{
private:
	uint32_t value;
	uint32_t a, c, m; // Seed = (a * Seed + c) % m
	
public:
	LinearCongruentialGenerator(){
		value = (uint32_t)42 * (uint32_t)m_Pow(7, 5);

		a = 0x343FD;
		c = 0x269EC3;
		m = (uint32_t)(std::pow(2, 32) - 1);
	}
	
	LinearCongruentialGenerator(bool useTimeAsSeed) {
		if (useTimeAsSeed) {
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

			auto time = now.time_since_epoch();
			auto time_cast = std::chrono::duration_cast<std::chrono::nanoseconds>(time);
			value = (uint32_t)time_cast.count();
		}
		else {
			value = 42 * (uint32_t)m_Pow(7, 5);
		}

		//Default values
		a = 0x343FD;
		c = 0x269EC3;
		m = (uint32_t)(std::pow(2, 32) - 1);
	}

	LinearCongruentialGenerator(const uint32_t seed) : value(seed) {
		//Default values
		a = 0x343FD; 
		c = 0x269EC3;
		m = (uint32_t)(std::pow(2, 32) - 1);
	}

	//set initial value
	void setSeed(const uint32_t seed) override {
		value = seed;
	}

	// X1 = (a * X0 + c) % m
	void setParameters(const uint32_t a, const uint32_t c, const uint32_t m) {
		this->a = a;
		this->c = c;
		this->m = m;
	}

	//It returns 32 bit unsigned integer
	uint32_t nextUInt() override {
		value = (uint32_t)((a * value + c) % m);
		return value;
	}

	//It returns 32 bit unsigned integer, range[min,max]
	uint32_t nextUInt(const uint32_t min, const uint32_t max) override {
		if (min >= max) {
			RAND_LOG_ERROR("Linear Congruential Generator: Max must be bigger than min");
			return 0;
		}

		value = (uint32_t)((a * value + c) % m);

		uint32_t diff = max - min + 1;
		return (uint32_t)(min + value % diff);
	}

private:

	int32_t m_Pow(int32_t base, uint32_t power) {
		if (power == 0)
			return 1;

		int32_t result = 1;
		for (unsigned int i = 0; i < power; i++) {
			result *= base;
		}

		return result;
	}

};