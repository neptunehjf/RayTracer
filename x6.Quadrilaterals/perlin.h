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

	// 基于位置计算的噪声
	// 把空间划分成一个个的立方体，每个立方体都有一个索引，在每个立方体内部，都有8个顶点
	// 通过计算点相对于这8个顶点的位置进行插值，相对于顶点的插值与离顶点的距离成反比
	// 
	// 位置ベースノイズ生成ロジック
	// 空間を立方体グリッドに分割し、各頂点の勾配ベクトルと
	// サンプル点の相対位置のドット積を用いた3次元補間を実施
	// 
	// 参照：referrence/Perlin Trilinear Interpolation.jpg
	double noise(const point3& p) const
	{
		double u = p.x() - floor(p.x());
		double v = p.y() - floor(p.y());
		double w = p.z() - floor(p.z());

		// Hermitian Smoothing 减轻马克纹
		// Hermite曲线在0和1处导数为0，且导数是一直变化的，因此相比线性函数更平滑，因此渲染效果更自然
		// 
		// Hermitian Smoothingによるマッハバンド低減
		// Hermite曲線は0と1で微分係数0（滑らかな接続）かつ連続的な変化特性を持つため、線形補間よりも自然なレンダリングを実現


		// 参照referrence/Hermite curve.png
		u = u * u * (3 - 2 * u); //3 * u * u - 2 * u * u * u により効率が高い
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
					// 使用异或而不是加法，乘法混合，是因为加法，乘法是线性运算，会产生可见的模式；并且加法乘法的效率低于异或运算
                    // 综上，使用异或混合是更优的选择
					// 
					// /* 勾配ベクトルテーブルから8頂点のベクトル取得 */
					/* XOR演算で非線形混合（パターン化防止＆演算効率向上） */
					// 
					// &255是想clamp值在0~255之间，bitwise&操作应该会比clamp操作效率更高
					//
					// &255で値0~255にをclampビット演算はclamp処理より高速化を意図
					c[di][dj][dk] = randvec[perm_x[(i + di) & 255] 
						                  ^ perm_y[(j + dj) & 255]
						                  ^ perm_z[(k + dk) & 255]];
				}

		return perlin_interpolation(c, u, v, w);
	}

	// 用湍流模拟自然界的多尺度现象，可以用于模拟自然界中的材质
	// 乱流効果生成（多重オクターブノイズ合成） 自然現象のマルチスケール特性を模倣
	// 
	// 参照 referrence/Turbulence.png
	double turbulence(const point3& p, int depth) const
	{
		double amplitude = 1.0;
		double frequency = 1.0;
		double noise_sum = 0.0;
		
		for (int i = 0; i < depth; i++)
		{
			noise_sum += amplitude * noise(frequency * p);
			amplitude *= 0.5;
			frequency *= 2;
		}
		
		return fabs(noise_sum);
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

	// Fisher-Yates 洗牌算法
	// 对大小为n的数组，每个元素被放到每个位置的概率都是1/n
	// 
	// Fisher-Yates シャッフルアルゴリズム
	// サイズnの配列に対して均等な確率分布を生成（各要素の配置確率1/n）
	// 
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

	static double perlin_interpolation(vec3 c[2][2][2], double u, double v, double w)
	{
		double accum = 0.0;

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					// 顶点指向点p的向量，用于和梯度向量求点积
					//
					// 頂点からサンプル点への相対ベクトル（勾配ベクトルとの内積用）
					vec3 weight_v = vec3(u - i, v - j, w - k);

					// 如果只用简单的随机数来插值，MIN和MAX的值一定会落在顶点上，会导致生成块状图案
					// 用梯度向量与weight_v的点积代替简单的随机数
					// 因为点积可正可负，所以在插值方向上的导数也可正可负
					// 这样一来MIN和MAX值就不一定落在顶点上了，生成的图案更自然
					// 
					// 参照 referrence/Perlin Gradient Vector.jpg

					/* 
				   - 単純乱数では極値が頂点に固定→ブロックパターン発生
				   - 勾配ベクトルとの内積利用で：
					 * 正負の値を取り得る（微分方向性の表現）
					 * 極値が頂点間で自由に発生（自然なパターン生成）
				   */

					accum += (dot(c[i][j][k], weight_v) 
						    * (i * u + (1 - i) * (1 - u))
						    * (j * v + (1 - j) * (1 - v))
						    * (k * w + (1 - k) * (1 - w)));
				}

		return accum;
	}
};