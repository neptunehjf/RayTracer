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

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } // 一元减法，不需要参数
	double operator[](int i) const { return e[i]; } //因为返回的是一个数值类型，所以不会修改成员的值，因此加const修饰
	double& operator[](int i) { return e[i]; } //因为返回的是一个引用，所以有可能修改成员变量的值，所以不能加const修饰

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
		return (*this) *= (1.0 / t); //因为上面已经重载了*=，这里可以直接复用
	}


	// 求3D空间长度
	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return (e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
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

// 给vec3起一个别名，主要是为了把几何与颜色区分开
using point3 = vec3;

// 向量工具函数。类成员函数重载了一元运算，这里在类外部定义二元运算。
// 成员函数默认是inline，但是在类外部需要手动指定

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

// 向量 x 向量
inline vec3 operator*(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// 标量 x 向量
inline vec3 operator*(double t, const vec3& v) 
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

// 向量 x 标量
inline vec3 operator*(const vec3& v, double t) 
{
	return t * v;
}

// 向量  / 标量
inline vec3 operator/(const vec3& v, double t) 
{
	return (1 / t) * v; // 复用上面的*即可
}

// 向量点乘
inline double dot(const vec3& u, const vec3& v) 
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// 向量叉乘
inline vec3 cross(const vec3& u, const vec3& v) 
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		        u.e[2] * v.e[0] - u.e[0] * v.e[2],
		        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// 求单位向量，normalize
inline vec3 unit_vector(const vec3& v) 
{
	return v / v.length();
}

// 用rejection method求一个球体内部的均匀随机的单位向量
// 为啥是球体？显然3d空间中均匀的随机向量应该分布在球体内
// 先直接根据随机向量-1.0，1.0构造，可得一个立方体内的随机向量
// 但立方体内部的随机向量不是均匀的，因此需要排除掉在立方体内而在球体外的部分
// 这种算法相当于排除法，比较简单，只是有可能反复reject,影响采样效率
// 参考referrence/rejection method.png
inline vec3 random_unit_vector()
{
	while (true)
	{
		vec3 v = vec3::random(-1.0, 1.0);
		double lensq = v.length_squared();
		// 防止double精度不够导致的lensq等于0，导致除数为0的bug
		// 对于double类型，大于1e-160都可以识别
		if (1e-160 < lensq && lensq <= 1.0)
		{
			return v / sqrt(lensq);
		}
	}
}

// 如果随机向量指向平面内部，则反转该向量
inline vec3 random_on_hemisphere(const vec3& normal)
{
	vec3 v = random_unit_vector();

	if (dot(v, normal) > 0.0)
		return v;
	else
		return -v;
}