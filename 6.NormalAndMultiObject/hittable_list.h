#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

// hittable_list是一系列hittable物体的集合，但如果把这个集合看作一个整体的话，也应该继承hittable物体的性质
// 物体被抽象成hittable了，没有形状，因此hittable_list不关心也没办法 判定hit
// hittable_list可以看成对于多个物体hit判定的一个框架，具体hit判定交给各几何形状子类
class hittable_list : public hittable
{
public:
	// hittable的shared_ptr指针的集合
	// 使用shared_ptr可提升内存安全性
	vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { objects.push_back(object); }

	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool is_hit = false;

		// 对于一组hittable对象，只需找到最近的交点
		// 因此用上次循环中找到的最近的t值 动态修改判定范围的最大值,效率比较高
		double closest_t = ray_tmax;

		for (const auto& object : objects)
		{
			is_hit = object->hit(r, ray_tmin, closest_t, temp_rec);
			if (is_hit)
			{
				closest_t = temp_rec.t;
				rec = temp_rec;
			}		
		}

		return is_hit;
	}
};