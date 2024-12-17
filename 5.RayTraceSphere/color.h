#pragma once

#include <iostream>
#include "vec3.h"

// ��vec3��һ����������Ҫ��Ϊ�˰Ѽ�������ɫ���ֿ�
using color = vec3;

// дһ����ɫֵ��out
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// [0.0, 1.0] ==> [0, 255]
	int ir = int(255.999 * r);
	int ig = int(255.999 * g);
	int ib = int(255.999 * b);

	// �����ض���output.ppm��ֱ����cout�������
	out << ir << ' ' << ig << ' ' << ib << '\n';
}