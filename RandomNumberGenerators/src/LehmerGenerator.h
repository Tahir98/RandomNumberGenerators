#pragma once 
#include "Random.h"
#include <chrono>

class LehmerGenerator : public Random {
private:
	uint32_t seed = 42;
	uint64_t M;
	uint32_t A;
	uint32_t Q;
	uint32_t R;
public:
	LehmerGenerator(bool useTimeAsSeed = true) {
		if (useTimeAsSeed) {
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

			auto time = now.time_since_epoch();
			auto time_cast = std::chrono::duration_cast<std::chrono::nanoseconds>(time);
			seed = (uint32_t)time_cast.count();
		}

		M = (uint64_t)std::pow(2, 32);
		A = 48271;
		Q = M / A;    // 44488
		R = M % A;    //  3399
	}

	LehmerGenerator(const uint32_t seed): seed(seed) {
		M = (uint64_t)std::pow(2, 32);
		A = 48271;
		Q = M / A;    // 44488
		R = M % A;    //  3399
	}

	~LehmerGenerator() {}


	//set initial value
	void setSeed(const uint32_t seed) override {
		this->seed = seed;
	}


	//It return 32bit random unsigned integer
	uint32_t nextUInt() override {
		uint32_t division = seed / Q;	
		uint32_t remainder = seed % Q;	

		int32_t s = remainder * A;
		int32_t t = division * R;
		int32_t res = s - t;

		if (res < 0)
			res += M;

		return seed = res;
	}

	//It return 32bit random unsigned integer, range[min, max]
	uint32_t nextUInt(const uint32_t min, const uint32_t max) override {
		if (min >= max) {
			RAND_LOG_ERROR("Error, Lagged Finonacci Generator: max must be bigger than min [{0},{1}]", min, max);
			return 0;
		}

		uint32_t division = seed / Q;
		uint32_t remainder = seed % Q;

		int32_t s = remainder * A;
		int32_t t = division * R;
		int32_t res = s - t;

		if (res < 0)
			res += M;

		seed = res;

		uint32_t diff = max - min + 1;

		return min + seed % diff;
	}
};