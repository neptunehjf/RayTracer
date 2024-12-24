#pragma once

#include <iostream>
#include <limits>
#include <cmath>
#include <vector>
#include <memory>

// C++ Std Usings
using namespace std;

// Constants
const double inf = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
double degree_to_radian(double degree)
{
	return degree * pi / 180.0;
}

// Common Headers
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "interval.h"