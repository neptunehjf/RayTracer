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
		// x0 y1 z2
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	//  调用次数多(循环次数多 + 递归)，需要重点优化
	//  对于width 400 sample 100的渲染场景，实装后渲染时间由77秒优化到36秒，优化效果不是很明显，进一步研究发现，是因为在频繁调用的hit函数内部申请了两个变量来接收ray_t.min和ray_t.max的原因。用以下方式优化后的时间缩短为11秒。
	//	优化：
	//	1 hit内部不申请变量，而是把入参ray_t改为值传递，并直接修改ray_t的值，这是最重要的优化点
	//	2 提前计算好倒数，后续乘倒数来实现除法
	//	3 能加const修饰尽量加const
	bool hit(const ray& r, interval ray_t) const
	{
		const point3& o = r.origin();
		const vec3& d = r.direction();

		// 一个向量是否与aabb相交 分解为 一个向量在x,y,z坐标轴上的分量是否都分别与对应的slab相交
		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = get_interval_by_axis(axis);

			// axis direction inverse，提前计算倒数以提高性能
			// 加const修饰符以提高性能
			const double adinv = 1 / d[axis];

			// slab与交点对应的t
			double t0 = (ax.min - o[axis]) * adinv;
			double t1 = (ax.max - o[axis]) * adinv;

			// 光线沿坐标轴负方向传播时，t0和t1的大小是反的
			if (t0 > t1)
				swap(t0, t1);

			// slab与ray的t求交集
			
			// 新申请两块内存，运行时间是36秒(width = 400 sample = 100)，效率较低
			//const double t_min = max(t0, ray_t.min);
			//const double t_max = min(t1, ray_t.max);

			// 复用虚参ray_t的内存区域，运行时间是11秒(width = 400 sample = 100)，效率较高
			// 缺点是虚参无法传值出去
			ray_t.min = max(t0, ray_t.min);
			ray_t.max = min(t1, ray_t.max);

			// 如果交集的区间的min >= max，很显然不可能存在这样的交集，所以hit返回false
			if (ray_t.min >= ray_t.max)
				return false;

		}
		return true;
	}

	int get_longest_axis() const
	{
		// x0 y1 z2
		if (x.size() > y.size() && x.size() > z.size()) return 0;
			
		if (y.size() > x.size() && y.size() > z.size()) return 1;

		if (z.size() > x.size() && z.size() > y.size()) return 2;
	}

	// 声明和类相同类型的静态变量是合法的，不会导致递归定义
	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
