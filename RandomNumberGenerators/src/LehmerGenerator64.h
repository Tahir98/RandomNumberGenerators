#pragma once

#include <cstdint>
#include <chrono>
#include "Log/Log.h"
#include "Utility/BitConverter.h"

class LehmerGenerator64 {
private:
	uint64_t seed = 42; // default initial value
	uint64_t M;
	uint64_t A;
	uint64_t Q;
	uint64_t R;
public:
	LehmerGenerator64(bool useTimeAsSeed = true) {
		if (useTimeAsSeed) {
			std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

			auto time = now.time_since_epoch();
			auto time_cast = std::chrono::duration_cast<std::chrono::nanoseconds>(time);
			seed = time_cast.count();
		}

		M = (uint64_t)std::pow(2, 63) - 1 + (uint64_t)std::pow(2, 63);
		A = 48271;
		Q = M / A;
		R = M % A;
	}

	LehmerGenerator64(const uint64_t seed) : seed(seed) {
		M = (uint64_t)std::pow(2, 63) - 1 + (uint64_t)std::pow(2, 63);
		A = 48271;
		Q = M / A;
		R = M % A;
	}

	~LehmerGenerator64() {}


	//Set the initial value
	void setSeed(const uint64_t seed) {
		this->seed = seed;
	}

	//It returns 64bit random unsigned integer
	uint64_t nextUInt64() {
		uint64_t division = seed / Q;
		uint64_t remainder = seed % Q;

		int64_t s = remainder * A;
		int64_t t = division * R;
		int64_t res = s - t;

		if (res < 0)
			res += M;

		return seed = res;
	}

	//It returns 64bit random unsigned integer, range[min,max]
	uint64_t nextUInt64(const uint64_t min, const uint64_t max) {
		if (min >= max) {
			RAND_LOG_ERROR("Error, Lagged Finonacci Generator: max must be bigger than min [{0},{1}]", min, max);
			return 0;
		}

		uint64_t division = seed / Q;
		uint64_t remainder = seed % Q;

		int64_t s = remainder * A;
		int64_t t = division * R;
		int64_t res = s - t;

		if (res < 0)
			res += M;

		seed = res;

		uint64_t diff = max - min + 1;

		return min + seed % diff;
	}

	//It returns 64bit random signed integer
	int64_t nextInt64() {
		int64_t value = (int64_t)nextUInt64();
		return value;
	}

	//It returns 64bit random signed integer, range[min,max]
	int64_t nextInt64(const int64_t min, const int64_t max) {
		if (min >= max) {
			RAND_LOG_ERROR("Error, Lagged Finonacci Generator: max must be bigger than min [{0},{1}]", min, max);
			return 0;
		}

		int64_t value = (int64_t)nextUInt64();
		int64_t diff = max - min + 1;

		return min + value % diff;
	}

	// It returns 32 bit floating point, range[0,1)
	float nextFloat() {
		uint64_t rand64 = nextUInt64();
		uint32_t rand = (uint32_t)(rand64 & 0x00000000FFFFFFFF);

		return BitConverter::Ran32Flt1(rand);
	}

	// It returns 64 bit floating point, range[0,1)
	long double nextDouble() {
		int64_t rand64 = nextInt64();
		int32_t rand1 = (int32_t)(rand64 & 0x00000000FFFFFFFF);
		int32_t rand2 = (int32_t)((rand64 >> 32) & 0x00000000FFFFFFFF);

		return BitConverter::Ran32Dbl1(rand1,rand2);
	}
};