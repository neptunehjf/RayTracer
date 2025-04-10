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
	// 但是实际上这种情况也应该算是有交点(grazing angle)，所以稍微扩充一下AABB的大小，使(t_min <= t_max)的结果为true
	// 
	// rayがslabと完全に一致する場合、t_minとt_maxがNaNとなり(t_min <= t_max)はfalseになる
	// 実際はこのケースも交差と見なすべき（グレージングケース）のため、AABBを微小拡張して(t_min <= t_max)を成立させる
	// 
	// 参照 referrence/aabb.jpg
	interval expand(double delta) const
	{
		double padding = delta / 2;
		return interval(min - padding, max + padding);
	}

	// 声明和类相同类型的静态变量是合法的，不会导致递归定义
	// クラス自身と同じ型の静的メンバ変数の宣言は有効であり、再帰的定義を引き起こさない
	static const interval empty, universe;
};

const interval interval::empty = interval(inf, -inf);
const interval interval::universe = interval(-inf, inf);