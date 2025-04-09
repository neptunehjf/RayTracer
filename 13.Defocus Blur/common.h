#pragma once

#include <iostream>
#include <limits>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

// C++ Std Usings
using namespace std;

// Constants
const double inf = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degree_to_radian(double degree)
{
	return degree * pi / 180.0;
}

inline double random_double()
{
	// グローバルで単一の乱数生成器を使用するためstatic修飾       
	static uniform_real_distribution<double> distribution(0.0, 1.0);
	static mt19937 generator;
	return distribution(generator);
}

inline double random_double(double min, double max)
{
	// グローバルで単一の乱数生成器を使用するためstatic修飾           
	static uniform_real_distribution<double> distribution(min, max);
	static mt19937 generator;
	return distribution(generator);
}

// Common Headers
#include "vec3.h"
#include "color.h"
#include "ray.h"
