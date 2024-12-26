#pragma once

#include "interval.h"

// 给vec3起一个别名，主要是为了把几何与颜色区分开
using color = vec3;

// 写一个颜色值到out
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// 这里最大值0.999是为了避免结果256导致的溢出(单字节颜色)
	const interval range(0.000, 0.999); 
	// [0.0, 1.0] ==> [0, 255]
	int ir = int(256 * range.clamp(r));
	int ig = int(256 * range.clamp(g));
	int ib = int(256 * range.clamp(b));

	// 配置重定向到output.ppm，直接用cout输出即可
	out << ir << ' ' << ig << ' ' << ib << '\n';
}