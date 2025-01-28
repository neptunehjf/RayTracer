#pragma once

#include "common.h"

class perlin
{
public:
	perlin()
	{
		for (int i = 0; i < point_count; i++)
		{
			randvec[i] = unit_vector(vec3::random(-1.0, 1.0));
		}

		generate_perm(perm_x);
		generate_perm(perm_y);
		generate_perm(perm_z);
	}

	// ����λ�ü��������
	// �ѿռ仮�ֳ�һ�����������壬ÿ�������嶼��һ����������ÿ���������ڲ�������8������
	// ͨ��������������8�������λ�ý��в�ֵ������ڶ���Ĳ�ֵ���붥��ľ���ɷ���
	// ���� referrence/Perlin Trilinear Interpolation.jpg
	double noise(const point3& p) const
	{
		// &255����clampֵ��0~255֮�䣬bitwise&����Ӧ�û��clamp����Ч�ʸ���
		double u = p.x() - floor(p.x());
		double v = p.y() - floor(p.y());
		double w = p.z() - floor(p.z());

		// Hermitian Smoothing ���������
		// Hermite������0��1������Ϊ0���ҵ�����һֱ�仯�ģ����������Ժ�����ƽ���������ȾЧ������Ȼ
		// ����referrence/Hermite curve.png
		u = u * u * (3 - 2 * u); // ��ֱ��3 * u * u - 2 * u * u * u Ч�ʸ�
		v = v * v * (3 - 2 * v);
		w = w * w * (3 - 2 * w);

		int i = (int)floor(p.x());
		int j = (int)floor(p.y());
		int k = (int)floor(p.z());

		vec3 c[2][2][2] = {};

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					// ʹ���������Ǽӷ����˷���ϣ�����Ϊ�ӷ����˷����������㣬������ɼ���ģʽ�����Ҽӷ��˷���Ч�ʵ����������
                    // ���ϣ�ʹ��������Ǹ��ŵ�ѡ��
					c[di][dj][dk] = randvec[perm_x[(i + di) & 255] 
						                  ^ perm_y[(j + dj) & 255]
						                  ^ perm_z[(k + dk) & 255]];
				}

		return perlin_interpolation(c, u, v, w);
	}


private:
	static const int point_count = 256;
	vec3 randvec[point_count];
	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];

	static void generate_perm(int* p)
	{
		for (int i = 0; i < point_count; i++)
			p[i] = i;

		permute(p, point_count);
	}

	// Fisher-Yates ϴ���㷨
	// �Դ�СΪn�����飬ÿ��Ԫ�ر��ŵ�ÿ��λ�õĸ��ʶ���1/n
	// ���� referrence/Fisher-Yates.jpg
	static void permute(int* p, int n)
	{
		int tmp;
		int ri;
		for (int i = n - 1; i > 0; i--)
		{
			ri = random_int(0, i);
			tmp = p[i];
			p[i] = p[ri];
			p[ri] = tmp;
		}
	}

	static double perlin_interpolation(vec3 c[2][2][2], double u, double v, double w)
	{
		double accum = 0.0;

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					// ����ָ���p�����������ں��ݶ���������
					vec3 weight_v = vec3(u - i, v - j, w - k);

					// ���ֻ�ü򵥵����������ֵ��MIN��MAX��ֵһ�������ڶ����ϣ��ᵼ�����ɿ�״ͼ��
					// ���ݶ�������weight_v�ĵ������򵥵������
					// ��Ϊ��������ɸ��������ڲ�ֵ�����ϵĵ���Ҳ�����ɸ�
					// ����һ��MIN��MAXֵ�Ͳ�һ�����ڶ������ˣ����ɵ�ͼ������Ȼ
					// ���� referrence/Perlin Gradient Vector.jpg
					accum += (dot(c[i][j][k], weight_v) 
						    * (i * u + (1 - i) * (1 - u))
						    * (j * v + (1 - j) * (1 - v))
						    * (k * w + (1 - k) * (1 - w)));
				}

		return accum;
	}
};