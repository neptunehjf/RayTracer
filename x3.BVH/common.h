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
	// 这里全局只需1个种子，1个伪随机数列，因此加static修饰   
	static mt19937 generator;
	// 因为范围是固定的(0.0, 1.0)，只初始化一次就好
	static uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

inline double random_double(double min, double max)
{
	// 这里全局只需1个种子，1个伪随机数列，因此加static修饰   
	static mt19937 generator;
	// 因为范围是可变的，所以可能需要多次初始化，不能加static修饰！
	uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

// Common Headers
#include "vec3.h"
#include "color.h"
#include "ray.h"
