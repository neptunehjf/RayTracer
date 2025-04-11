#pragma once

#include <iostream>
#include <limits>
#include <cmath>
#include <vector>
#include <memory>
#include <random>
#include <time.h>

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
	static mt19937 generator;
	
	// 値域が固定(0.0, 1.0)のため、初期化は1回限りでいい static修飾
	static uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

inline double random_double(double min, double max)
{
	// グローバルで単一の乱数生成器を使用するためstatic修飾 
	static mt19937 generator;
	// 値域が変動可能のため複数回の初期化が必要であり、static修飾子は使用禁止‌
	uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

// return int [min, max]
inline int random_int(int min, int max)
{
	// eg: 0.0~0.999 => 0.0~1.999 => 0~1
	return (int)(random_double(min, max + 1));
}

// Common Headers
#include "vec3.h"
#include "color.h"
#include "ray.h"
