#pragma once

#include "vec3.h"
#include "ray.h"

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t;
};

// 抽象类，具体放到子类实现
class hittable
{
public:
	virtual ~hittable() = default;

	// 纯虚函数，具体放到子类实现
	virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};