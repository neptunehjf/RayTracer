#pragma once

#include "common.h"

// Axis-Aligned Bounding Box
// 具体参考 referrence/aabb.jpg
class aabb
{
public:
	// aabb的3对slab，与对应的坐标轴对齐
	interval x, y, z;

	// 默认构造 空aabb  因为interval默认是空(inf, -inf)
	aabb() {} 
	
	// 用interval直接构造aabb
	aabb(const interval& x, const interval& y, const interval& z) :
		x(x), y(y), z(z) {}

	// 用两个点间接构造aabb，入参只需提供数据，不需要排序
	aabb(const point3& a, const point3& b)
	{
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
	}

	// 用2个包围盒构造1个包围盒
	aabb(const aabb& bbox1, const aabb& bbox2)
	{
		x = interval(bbox1.x, bbox2.x);
		y = interval(bbox1.y, bbox2.y);
		z = interval(bbox1.z, bbox2.z);
	}

	const interval& get_interval_by_axis(int n) const
	{
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool hit(const ray& r, const interval& ray_t) const
	{
		const point3& o = r.origin();
		const vec3& d = r.direction();

		// 一个向量是否与aabb相交 分解为 一个向量在x,y,z坐标轴上的分量是否都分别与对应的slab相交
		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = get_interval_by_axis(axis);

			// axis direction inverse，提前计算倒数以提高性能
			double adinv = 1 / d[axis];

			double t0 = (ax.min - o[axis]) * adinv;
			double t1 = (ax.max - o[axis]) * adinv;

			// 光线沿坐标轴负方向传播时，t0和t1的大小是反的
			if (t0 > t1)
				swap(t0, t1);

			// ray与slab的interval(区间)求交集
			double t_min, t_max;
			t_min = max(t0, ax.min);
			t_max = min(t1, ax.max);

			// 如果交集的区间的min >= max，很显然不可能存在这样的交集，所以hit返回false
			if (t_min >= t_max)
				return false;
		}
		return true;
	}

};
