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

	// 基于位置计算的噪声
	double noise(const point3& p) const
	{
		// 4是噪声变化的频率，暂时设为4
		// &255是想clamp值在0~255之间，bitwise&操作应该会比clamp操作效率更高
		int i = int(4 * p.x()) & 255; 
		int j = int(4 * p.y()) & 255;
		int k = int(4 * p.z()) & 255;

		// 使用异或而不是加法，乘法混合，是因为加法，乘法是线性运算，会产生可见的模式；并且加法乘法的效率低于异或运算
		// 综上，使用异或混合是更优的选择
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

	// Fisher-Yates 洗牌算法
	// 对大小为n的数组，每个元素被放到每个位置的概率都是1/n
	// 参照 referrence/Fisher-Yates.jpg
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