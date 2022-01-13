#pragma once

//In the application project, rather than every time including every header that you will use, 
//you can just include this header to reach other headers

//Pseudo random number generator algorithms
#include "src/MiddleSquareMethod.h"
#include "src/LinearCongruentialGenerator.h"
#include "src/MersenneTwister.h"
#include "src/MersenneTwister64.h"
#include "src/LaggedFibonacciGenerator.h"
#include "src/AddWithCarry.h"
#include "src/LehmerGenerator.h"
#include "src/LehmerGenerator64.h"
#include "src/Xorshift.h"
#include "src/Xorshift64.h"
#include "src/MultiplyWithCarry.h"
#include "src/LaggedFibonacciGenerator64.h"

//Logging class
#include "src/Log/Log.h"