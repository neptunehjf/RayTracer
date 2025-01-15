#pragma once

#include "hittable_list.h"
#include <algorithm>
#include "dbg.h"

// Bounding Volume Hierarchies ��Χ�в㼶�ṹ
// �������ṹ��ÿ����Χ�п���һ���ڵ㣬��ѯ������hit������ҲҪ�̳�hittable
// ��ѯЧ����O(log n)��ԶԶ�������Բ�ѯ��O(n)
// ����ο�referrence/bvh.jpg
class bvh_node : public hittable
{
public:
	// ֱ�Ӵ��뿽����Ϊ�˷����޸�hittable_list��ֵ�ֲ�Ӱ��ԭ����ֵ
	bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

	bvh_node(vector<shared_ptr<hittable>>& objects, size_t start, size_t end)
	{
		size_t axis = random_int(0, 2);

		auto comparator = (axis == 0) ? box_compare_x :
					      (axis == 1) ? box_compare_y : box_compare_z;

		size_t objects_num = end - start;

		// ���ֻ��һ����Ҳ����һ������ģ���ֹ�սڵ�
		if (objects_num == 1)
			left = right = objects[start];
		else if(objects_num == 2)
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			sort(objects.begin() + start, objects.begin() + end, comparator);
			size_t mid = start + objects_num / 2;
			left = make_shared<bvh_node>(objects, start, mid);
			right = make_shared<bvh_node>(objects, mid, end);
		}

		bbox = aabb(left->bounding_box(), right->bounding_box());
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// ���δ���б��ڵ㣬ֱ�ӷ���false
		if (!bbox.hit(r, ray_t))
		{
			unhit_count++;
			return false;
		}
		hit_count++;
			
		// ��������˱��ڵ㣬�������������ӽڵ�
		bool left_hit = left->hit(r, ray_t, rec);
		// ����Ѿ���������ڵ㣬���Ѿ���ѯ�����·�Χ����ڵ㣬��˸���ray��t_maxΪrec.t�����ٲ���Ҫ�Ĳ�ѯ��Χ
		interval iv = interval(ray_t.min, left_hit ? rec.t : ray_t.max);
		bool right_hit = right->hit(r, iv, rec);

		return left_hit || right_hit;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	aabb bbox;
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;

	static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis)
	{
		interval a_interval_by_axis = a->bounding_box().get_interval_by_axis(axis);
		interval b_interval_by_axis = b->bounding_box().get_interval_by_axis(axis);
		return a_interval_by_axis.min < b_interval_by_axis.min;
	}

	static bool box_compare_x(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 0);
	}

	static bool box_compare_y(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 1);
	}

	static bool box_compare_z(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 2);
	}
};