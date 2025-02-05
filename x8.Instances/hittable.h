#pragma once

#include "common.h"
#include "aabb.h"

// 因为material.h已经包含了hittable.h，为了防止互相包含，这里用只声明的方式
class material;

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t = 0.0;
	shared_ptr<material> mat;
	double u;
	double v;

	// 判断ray是从物体外部还是内部打过来的
	// 当前情况下，在几何阶段做判断结果并存储可能是一种比较高效的选择
	bool front_face = true; 

	// 根据ray和外部法线的方向判断当前光线是从内部还是外部来的，以及求实际的normal
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// 要确保传入的outward_normal是单位向量

		if (dot(r.direction(), outward_normal) > 0)
		{
			// 如果ray方向和外部法线的方向一致，ray来自物体内部
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// 如果ray方向和外部法线的方向相反，ray来自物体外部
			front_face = true;
			normal = outward_normal;
		}
	}
};

// 抽象类，具体放到子类实现
class hittable
{
public:
	virtual ~hittable() = default;

	// 纯虚函数，具体放到子类实现
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	// 返回物体的包围盒
	virtual aabb bounding_box() const = 0;
};

// 平移类，用于物体的平移
// 1 对实际物体的的hit判断封装了一层，用光线(camera)反向位移的方式来对位移后的物体做hit判断
// 用光线(camera)位移的方式，可以避免对复杂形状的实际物体进行位移操作
// 2 但是对于实际物体的包围盒，因为包围盒形状简单，直接对包围盒做位移操作，这样不需要在每次hit都位移，只需初始化物体的时候位移。效率更高
// 综上，对于实际物体，不位移实际物体，用光反向位移进行hit判断；对于包围盒，位移包围盒，用本来的光线进行hit判断
class translate : public hittable
{
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset)
	{
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 物体移动了offset ,相当于camera(光线起始点)移动-offset
		ray offset_r(r.origin() + offset, r.direction(), r.time());

		// 用移动后的光线与未移动的物体进行hit判断
		if (object->hit(offset_r, ray_t, rec))
		{
			// 如果hit了，实际上是在rec.p + offset处hit的
			rec.p += offset;
			return true;
		}
		else
			return false;

	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};