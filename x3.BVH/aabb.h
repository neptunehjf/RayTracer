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
		// x0 y1 z2
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	//  ���ô�����(ѭ�������� + �ݹ�)����Ҫ�ص��Ż�
	//  ����width 400 sample 100����Ⱦ������ʵװ����Ⱦʱ����77���Ż���36�룬�Ż�Ч�����Ǻ����ԣ���һ���о����֣�����Ϊ��Ƶ�����õ�hit�����ڲ���������������������ray_t.min��ray_t.max��ԭ�������·�ʽ�Ż����ʱ������Ϊ11�롣
	//	�Ż���
	//	1 hit�ڲ���������������ǰ����ray_t��Ϊֵ���ݣ���ֱ���޸�ray_t��ֵ����������Ҫ���Ż���
	//	2 ��ǰ����õ����������˵�����ʵ�ֳ���
	//	3 �ܼ�const���ξ�����const
	bool hit(const ray& r, interval ray_t) const
	{
		const point3& o = r.origin();
		const vec3& d = r.direction();

		// һ�������Ƿ���aabb�ཻ �ֽ�Ϊ һ��������x,y,z�������ϵķ����Ƿ񶼷ֱ����Ӧ��slab�ཻ
		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = get_interval_by_axis(axis);

			// axis direction inverse����ǰ���㵹�����������
			// ��const���η����������
			const double adinv = 1 / d[axis];

			// slab�뽻���Ӧ��t
			double t0 = (ax.min - o[axis]) * adinv;
			double t1 = (ax.max - o[axis]) * adinv;

			// �����������Ḻ���򴫲�ʱ��t0��t1�Ĵ�С�Ƿ���
			if (t0 > t1)
				swap(t0, t1);

			// slab��ray��t�󽻼�
			
			// �����������ڴ棬����ʱ����36��(width = 400 sample = 100)��Ч�ʽϵ�
			//const double t_min = max(t0, ray_t.min);
			//const double t_max = min(t1, ray_t.max);

			// �������ray_t���ڴ���������ʱ����11��(width = 400 sample = 100)��Ч�ʽϸ�
			// ȱ��������޷���ֵ��ȥ
			ray_t.min = max(t0, ray_t.min);
			ray_t.max = min(t1, ray_t.max);

			// ��������������min >= max������Ȼ�����ܴ��������Ľ���������hit����false
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

	// ����������ͬ���͵ľ�̬�����ǺϷ��ģ����ᵼ�µݹ鶨��
	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
