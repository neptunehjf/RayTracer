#pragma once

#include "interval.h"

// 给vec3起一个别名，主要是为了把几何与颜色区分开
using color = vec3;

double linear_to_gamma(double color_componet)
{
	if (color_componet > 0.0)
	{
		// 标准是gamma2.2，这里简化成gamma2
		// 所以 gamma空间的颜色值是线性空间的颜色值的 1/2次方，也即平方根
		return sqrt(color_componet);
	}

	return 0.0;
}

// 写一个颜色值到out
void write_color(ostream& out, const color& c)
{
	double r = linear_to_gamma(c.x());
	double g = linear_to_gamma(c.y());
	double b = linear_to_gamma(c.z());

	// 这里最大值0.999是为了避免结果256导致的溢出(单字节颜色)
	const interval range(0.000, 0.999); 
	// [0.0, 1.0] ==> [0, 255]
	int ir = int(256 * range.clamp(r));
	int ig = int(256 * range.clamp(g));
	int ib = int(256 * range.clamp(b));

	// 配置重定向到output.ppm，直接用cout输出即可
	out << ir << ' ' << ig << ' ' << ib << '\n';
}