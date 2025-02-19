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

// return double [0.0, 1.0)
inline double random_double()
{
	// ����ȫ��ֻ��1�����ӣ�1��α������У���˼�static����   
	static mt19937 generator;
	// ��Ϊ��Χ�ǹ̶��İ뿪��Χ[0.0, 1.0)��ֻ��ʼ��һ�ξͺ�
	static uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

// return double [min, max)
inline double random_double(double min, double max)
{
	// ����ȫ��ֻ��1�����ӣ�1��α������У���˼�static����   
	static mt19937 generator;
	// ��Ϊ��Χ�ǿɱ�İ뿪��Χ[min, max)�����Կ�����Ҫ��γ�ʼ�������ܼ�static���Σ�
	uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

// return int [min, max]
inline int random_int(int min, int max)
{
	// eg: 0.0~0.999 => 0.0~1.999 => 0~1
	return (int)(random_double(min, max + 1));
}

/**************Monte Carlo�������****************/
struct sample {
	double x;   // ������
	double p_x; // �ò�����ĸ���pdf(x)
};

inline bool compare_by_x(const sample& s1, const sample& s2)
{
	return s1.x < s2.x;
}
/**************Monte Carlo�������****************/

// Common Headers
#include "vec3.h"
#include "color.h"
#include "ray.h"
