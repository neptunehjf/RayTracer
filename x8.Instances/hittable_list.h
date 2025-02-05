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

	void add(shared_ptr<hittable> object) 
	{ 
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// hit_record temp_rec;
		bool is_hit = false;

		// 对于一组hittable对象，只需找到最近的交点
		// 因此用上次循环中找到的最近的t值 动态修改判定范围的最大值,效率比较高
		for (const auto& object : objects)
		{
			if (object->hit(r, ray_t, rec))
			{
				// 因为ray_t改为值传递可以有更大的优化空间，此处的优化舍弃
				// 而且在bvh结构中已经有类似的优化了，如果left节点 hit的话，对于right节点的ray.max也会更新为rec.t
				// ray_t.max = temp_rec.t; 

				//rec = temp_rec;
				is_hit = true;
			}		
		}

		return is_hit;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	aabb bbox; // 默认构造 是空aabb，无需初始化
};