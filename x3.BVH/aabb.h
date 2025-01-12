#pragma once

#include "common.h"

// Axis-Aligned Bounding Box
// ����ο� referrence/aabb.jpg
class aabb
{
public:
	// aabb��3��slab�����Ӧ�����������
	interval x, y, z;

	// Ĭ�Ϲ��� ��aabb  ��ΪintervalĬ���ǿ�(inf, -inf)
	aabb() {} 
	
	// ��intervalֱ�ӹ���aabb
	aabb(const interval& x, const interval& y, const interval& z) :
		x(x), y(y), z(z) {}

	// ���������ӹ���aabb�����ֻ���ṩ���ݣ�����Ҫ����
	aabb(const point3& a, const point3& b)
	{
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
	}

	// ��2����Χ�й���1����Χ��
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

		// һ�������Ƿ���aabb�ཻ �ֽ�Ϊ һ��������x,y,z�������ϵķ����Ƿ񶼷ֱ����Ӧ��slab�ཻ
		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = get_interval_by_axis(axis);

			// axis direction inverse����ǰ���㵹�����������
			double adinv = 1 / d[axis];

			double t0 = (ax.min - o[axis]) * adinv;
			double t1 = (ax.max - o[axis]) * adinv;

			// �����������Ḻ���򴫲�ʱ��t0��t1�Ĵ�С�Ƿ���
			if (t0 > t1)
				swap(t0, t1);

			// ray��slab��interval(����)�󽻼�
			double t_min, t_max;
			t_min = max(t0, ax.min);
			t_max = min(t1, ax.max);

			// ��������������min >= max������Ȼ�����ܴ��������Ľ���������hit����false
			if (t_min >= t_max)
				return false;
		}
		return true;
	}

};
