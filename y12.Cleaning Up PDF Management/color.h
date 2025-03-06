#pragma once

#include "interval.h"

// ��vec3��һ����������Ҫ��Ϊ�˰Ѽ�������ɫ���ֿ�
using color = vec3;

double linear_to_gamma(double color_componet)
{
	if (color_componet > 0.0)
	{
		// ��׼��gamma2.2������򻯳�gamma2
		// ���� gamma�ռ����ɫֵ�����Կռ����ɫֵ�� 1/2�η���Ҳ��ƽ����
		return sqrt(color_componet);
	}

	return 0.0;
}

// дһ����ɫֵ��out
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// ����acne ֵ��NaN���������Ϊ0
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	// GammaУ��
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// �������ֵ0.999��Ϊ�˱�����256���µ����(���ֽ���ɫ)
	const interval range(0.000, 0.999); 
	// [0.0, 1.0] ==> [0, 255]
	int ir = int(256 * range.clamp(r));
	int ig = int(256 * range.clamp(g));
	int ib = int(256 * range.clamp(b));

	// �����ض���output.ppm��ֱ����cout�������
	out << ir << ' ' << ig << ' ' << ib << '\n';
}