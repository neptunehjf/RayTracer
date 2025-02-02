#pragma once
#include "common.h"

class interval
{
public:
	double min, max;

	interval() : min(inf), max(-inf) {}
	interval(double min, double max) : min(min), max(max) {}

	interval(const interval& a, const interval& b)
	{
		min = std::min(a.min, b.min);
		max = std::max(a.max, b.max);
	}

	double size() const { return max - min; }

	double contains(double x) const { return (min <= x && x <= max); }

	double surrounds(double x) const { return (min < x && x < max); }

	double clamp(double x) const
	{
		if (x < min)
			return min;
		else if (x > max)
			return max;
		else
			return x;
	}

	// ray与slab重合的场合，t_min和t_max都是NaN，所以(t_min <= t_max)的结果必为false
	// 但是实际上这种情况也应该算是有交点(对于是球体的情况 grazing angle)，所以稍微扩充一下AABB的大小，使(t_min <= t_max)的结果为true
	// 具体参考 referrence/aabb.jpg 特殊情况3
	// 
	// 另一种情况是图元为面的情况，这时候没有体积，就需要扩充，否则可能会有数学错误
	interval expand(double delta) const
	{
		double padding = delta / 2;
		return interval(min - padding, max + padding);
	}

	// <声明>和类相同类型的静态变量是合法的，<声明>不会分配内存，因此不会导致递归定义
	static const interval empty, universe;
};

// <定义>和类相同类型的静态变量是在类外部进行的，在此处会分配内存
const interval interval::empty = interval(inf, -inf);
const interval interval::universe = interval(-inf, inf);