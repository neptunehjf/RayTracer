﻿#pragma once
#include "common.h"

class interval
{
public:
	double min, max;

	interval() : min(inf), max(-inf) {}
	interval(double min, double max) : min(min), max(max) {}

	double size() const { return max - min; }

	double contains(double x) const { return (min <= x && x <= max); }

	double surrounds(double x) const { return (min < x && x < max); }

	double clamp(double x) const
	{
		if (x < min)
			return min;
		else if (x > max)
			return max;
	}

	// 声明和类相同类型的静态变量是合法的，不会导致递归定义
	// クラス自身と同じ型の静的メンバ変数の宣言は有効であり、再帰的定義を引き起こさない
	static const interval empty, universe;
};

const interval interval::empty = interval(inf, -inf);
const interval interval::universe = interval(-inf, inf);