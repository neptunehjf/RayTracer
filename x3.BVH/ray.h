#pragma once

#include "common.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction), tm(0.0) {}
	ray(const point3& origin, const vec3 direction, double time) : o(origin), d(direction), tm(time) {}

	const point3& origin() const { return o; } //因为return的是引用，所以返回值也要加const才能保证不被修改
	const vec3& direction() const { return d; }

	// 获得ray在时间t的位置
	point3 at(double t) const
	{
		return o + t * d;
	}

	// 获取当前时刻
	double get_time() const
	{
		return tm;
	}

private:
	point3 o; // 起始点
	vec3 d;   // d的大小相当于速度(宏观上与物体相对位置有关，并不是微观的光线的速度，光线的速度可认为是无穷大)

	// camera发出射线的时刻，用于模拟不同时刻运动物体的位置变化
	// 只是为了表现物体的运动，是宏观世界的时间，要远远大于微观光线传播的时间，无论光线反弹多少次，都可以认为是固定的
	double tm = 0.0; 
};