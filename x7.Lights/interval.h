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

	// ray��slab�غϵĳ��ϣ�t_min��t_max����NaN������(t_min <= t_max)�Ľ����Ϊfalse
	// ����ʵ�����������ҲӦ�������н���(�������������� grazing angle)��������΢����һ��AABB�Ĵ�С��ʹ(t_min <= t_max)�Ľ��Ϊtrue
	// ����ο� referrence/aabb.jpg �������3
	// 
	// ��һ�������ͼԪΪ����������ʱ��û�����������Ҫ���䣬������ܻ�����ѧ����
	interval expand(double delta) const
	{
		double padding = delta / 2;
		return interval(min - padding, max + padding);
	}

	// <����>������ͬ���͵ľ�̬�����ǺϷ��ģ�<����>��������ڴ棬��˲��ᵼ�µݹ鶨��
	static const interval empty, universe;
};

// <����>������ͬ���͵ľ�̬�����������ⲿ���еģ��ڴ˴�������ڴ�
const interval interval::empty = interval(inf, -inf);
const interval interval::universe = interval(-inf, inf);