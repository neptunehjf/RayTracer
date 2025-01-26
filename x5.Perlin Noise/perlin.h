#pragma once

#include "common.h"

class perlin
{
public:
	perlin()
	{
		for (int i = 0; i < point_count; i++)
		{
			randfloat[i] = random_double();
		}
		#include "perlin.h"
		generate_perm(perm_x);
		generate_perm(perm_y);
		generate_perm(perm_z);
	}

	// ����λ�ü��������
	double noise(const point3& p) const
	{
		// 4�������仯��Ƶ�ʣ���ʱ��Ϊ4
		// &255����clampֵ��0~255֮�䣬bitwise&����Ӧ�û��clamp����Ч�ʸ���
		int i = int(4 * p.x()) & 255; 
		int j = int(4 * p.y()) & 255;
		int k = int(4 * p.z()) & 255;

		// ʹ���������Ǽӷ����˷���ϣ�����Ϊ�ӷ����˷����������㣬������ɼ���ģʽ�����Ҽӷ��˷���Ч�ʵ����������
		// ���ϣ�ʹ��������Ǹ��ŵ�ѡ��
		return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}


private:
	static const int point_count = 256;
	double randfloat[point_count];
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
};