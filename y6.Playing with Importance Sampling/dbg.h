#pragma once

size_t unhit_count = 0;
size_t hit_count = 0;
size_t total_bounce = 0;

/**************Monte Carlo采样相关****************/
struct sample {
	double x;   // 采样点
	double p_x; // 该采样点的概率pdf(x)
};

inline bool compare_by_x(const sample& s1, const sample& s2)
{
	return s1.x < s2.x;
}

inline double icd_uniform(double d)
{
	return 2 * d;
}

inline double pdf_uniform(double x)
{
	return 0.5;
}

inline double icd_linear(double d)
{
	return sqrt(4.0 * d);
}

inline double pdf_linear(double x)
{
	return 0.5 * x;
}

inline double icd_quadratic(double d)
{
	return pow(8 * d, 1.0 / 3.0);
}

inline double pdf_quadratic(double x)
{
	return 3.0 / 8.0 * x * x;
}

inline double pdf_sphere_uniform(const vec3& d) 
{
	return 1 / (4 * pi);
}

/**************Monte Carlo采样相关****************/