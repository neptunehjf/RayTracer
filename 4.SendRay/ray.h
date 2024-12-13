#pragma once

#include "vec3.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction) {}

	const point3& origin() const { return o; } //因为return的是引用，所以返回值也要加const才能保证不被修改
	const vec3& direction() const { return d; }

	// 获得ray在时间t的位置
	point3 at(double t) const
	{
		return o + t * d;
	}

private:
	point3 o;
	vec3 d;
};