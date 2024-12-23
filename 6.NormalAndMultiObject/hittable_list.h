#pragma once

#include "common.h"
#include "hittable.h"


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
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }
	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool is_hit = false;

		// 对于一组hittable对象，只需找到最近的交点
		// 因此用上次循环中找到的最近的t值 动态修改判定范围的最大值,效率比较高
		for (const auto& object : objects)
		{
			if (object->hit(r, ray_t, temp_rec))
			{
				ray_t.max = temp_rec.t;
				rec = temp_rec;
				is_hit = true;
			}		
		}

		return is_hit;
	}
};