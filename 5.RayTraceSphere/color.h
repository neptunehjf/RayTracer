#pragma once

#include <iostream>
#include "vec3.h"

// 给vec3起一个别名，主要是为了把几何与颜色区分开
using color = vec3;

// 写一个颜色值到out
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// [0.0, 1.0] ==> [0, 255]
	int ir = int(255.999 * r);
	int ig = int(255.999 * g);
	int ib = int(255.999 * b);

	// 配置重定向到output.ppm，直接用cout输出即可
	out << ir << ' ' << ig << ' ' << ib << '\n';
}