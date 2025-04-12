#pragma once

#include "common.h"


class vec3
{
public:
	double e[3];

	vec3() : e{0, 0, 0} {}
	vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	// 一元减法，不需要参数
	// 単項マイナス演算子（引数不要）
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } 

	// 因为返回的是一个数值类型，所以不会修改成员的值，因此加const修饰
	// 定数アクセス版（メンバを変更しないためconst修飾）
	double operator[](int i) const { return e[i]; } 

	//因为返回的是一个引用，所以有可能修改成员变量的值，所以不能加const修饰
	// 非定数参照版（メンバ変更の可能性があるためconst修飾なし）
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(double t)
	{
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(double t)
	{
		return (*this) *= (1.0 / t); // 既存の *= を再利用
	}


	// ベクトルの長さ計算
	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return (e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}

	bool near_zero() const
	{
		double a = 1e-8;

		return (fabs(e[0]) < a) && (fabs(e[1]) < a) && (fabs(e[2]) < a);
	}

	static vec3 random()
	{
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max)
	{
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
	
};

// 幾何計算用ポイントタイプの別名定義
using point3 = vec3;

// 向量工具函数。类成员函数重载了一元运算，这里在类外部定义二元运算。
// 成员函数默认是inline，但是在类外部需要手动指定
// 
// ベクトル用ユーティリティ関数群。クラスメンバ関数では単項演算子をオーバーロードし、
// 二項演算子はクラス外部で定義しています
// メンバ関数は暗黙的にinline扱いとなりますが、クラス外部定義では明示的なinline指定が必要です
inline ostream& operator<<(ostream& out, const vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) 
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) 
{
	return t * v;
}

inline vec3 operator/(const vec3& v, double t) 
{
	return (1 / t) * v; // 既存の *= を再利用
}

// 向量点乘
// 内積計算
inline double dot(const vec3& u, const vec3& v) 
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// 向量叉乘
// 外積計算
inline vec3 cross(const vec3& u, const vec3& v) 
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		        u.e[2] * v.e[0] - u.e[0] * v.e[2],
		        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// 正規化（単位ベクトル化）
inline vec3 unit_vector(const vec3& v) 
{
	return v / v.length();
}

// 用rejection method求一个球体内部的均匀随机的单位向量
// 为啥是球体？显然3d空间中均匀的随机向量应该分布在球体内
// 先直接根据随机向量-1.0，1.0构造，可得一个立方体内的随机向量
// 但立方体内部的随机向量不是均匀的，因此需要排除掉在立方体内而在球体外的部分
// 这种算法相当于排除法，比较简单，只是有可能反复reject,影响采样效率
// 
// 参照: referrence/rejection method.png
// 
// 棄却法（rejection method）による球体内部の一様ランダム単位ベクトル生成
// なぜ球体なのか？3D空間で一様なランダムベクトルは球体内に分布する必要があるため
// まず-1.0から1.0の範囲でランダムベクトルを生成（立方体内のランダム点）
// ただし立方体内の点は一様分布ではないため、球体外の点を棄却
// このアルゴリズムは実装が簡単だが、リジェクトが繰り返される可能性がありサンプリング効率に影響

inline vec3 random_unit_vector()
{
	while (true)
	{
		vec3 v = vec3::random(-1.0, 1.0);
		double lensq = v.length_squared();
		// 防止double精度不够导致的lensq等于0，导致除数为0的bug
		// 对于double类型，大于1e-160都可以识别
		// 
		// 参照: referrence/lambertian reflection.png
		// 
		// double型の精度不足によるlensqのゼロ発生を防止
		// double型では1e-160以上を検出可能
		if (1e-160 < lensq && lensq <= 1.0)
		{
			return v / sqrt(lensq);
		}
	}
}

inline vec3 random_on_unit_disk()
{
	while (true)
	{
		vec3 v = vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0.0);

		if (v.length_squared() <= 1.0)
			return v;
	}
}

// 如果随机向量指向平面内部，则反转该向量
// 
// ランダムベクトルが平面の内側を向く場合、ベクトルを反転
inline vec3 random_on_hemisphere(const vec3& normal)
{
	vec3 v = random_unit_vector();

	if (dot(v, normal) > 0.0)
		return v;
	else
		return -v;
}

// 入射方向と法線方向から鏡面反射方向を計算
// 参照: referrence/mirror reflection.jpg
// 入射光 法线 不需要是单位向量
inline vec3 reflect(const vec3& v, const vec3& normal)
{
	return v - 2 * dot(v, normal) * normal;
}

// 根据入射方向和法线方向求折射方向，前提是假设入射光，折射光，法线都是单位向量
// 参考referrence/refraction.jpg
// unit_v:入射光，必须是单位向量 uint_n:表面法线，必须是单位向量 ri:被折射介质的内外的相对折射率
// 对dot求cos操作加了fmin为1的限制，对sqrt操作加了fabs限制，提高健壮性
inline vec3 refract(const vec3& unit_v, const vec3& uint_n, double ri)
{
	double cos_theta = fmin(dot(-unit_v, uint_n), 1.0);
	vec3 out_perp = ri * (unit_v + uint_n * cos_theta);
	vec3 out_para = -sqrt(fabs(1.0 - out_perp.length_squared())) * uint_n;
	return out_perp + out_para;
}

// z軸ベースのランダムベクトルを生成
// 参照referrence/cos_cube_cos_sample.png 
inline vec3 random_cosine_direction() 
{
	auto r1 = random_double();
	auto r2 = random_double();

	auto phi = 2 * pi * r1;
	auto x = std::cos(phi) * std::sqrt(r2);
	auto y = std::sin(phi) * std::sqrt(r2);
	auto z = std::sqrt(1 - r2);

	return vec3(x, y, z);
}
