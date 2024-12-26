#pragma once

#include "interval.h"

// ��vec3��һ����������Ҫ��Ϊ�˰Ѽ�������ɫ���ֿ�
using color = vec3;

// дһ����ɫֵ��out
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// �������ֵ0.999��Ϊ�˱�����256���µ����(���ֽ���ɫ)
	const interval range(0.000, 0.999); 
	// [0.0, 1.0] ==> [0, 255]
	int ir = int(256 * range.clamp(r));
	int ig = int(256 * range.clamp(g));
	int ib = int(256 * range.clamp(b));

	// �����ض���output.ppm��ֱ����cout�������
	out << ir << ' ' << ig << ' ' << ib << '\n';
}