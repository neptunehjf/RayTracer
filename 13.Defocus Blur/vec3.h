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

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } // һԪ����������Ҫ����
	double operator[](int i) const { return e[i]; } //��Ϊ���ص���һ����ֵ���ͣ����Բ����޸ĳ�Ա��ֵ����˼�const����
	double& operator[](int i) { return e[i]; } //��Ϊ���ص���һ�����ã������п����޸ĳ�Ա������ֵ�����Բ��ܼ�const����

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
		return (*this) *= (1.0 / t); //��Ϊ�����Ѿ�������*=���������ֱ�Ӹ���
	}


	// ��3D�ռ䳤��
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
		double a = 1e-8; //1��10��-8�η�

		return (fabs(e[0] < a) && fabs(e[1] < a) && fabs(e[2] < a));
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

// ��vec3��һ����������Ҫ��Ϊ�˰Ѽ�������ɫ���ֿ�
using point3 = vec3;

// �������ߺ��������Ա����������һԪ���㣬���������ⲿ�����Ԫ���㡣
// ��Ա����Ĭ����inline�����������ⲿ��Ҫ�ֶ�ָ��

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

// ���� x ����
inline vec3 operator*(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// ���� x ����
inline vec3 operator*(double t, const vec3& v) 
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

// ���� x ����
inline vec3 operator*(const vec3& v, double t) 
{
	return t * v;
}

// ����  / ����
inline vec3 operator/(const vec3& v, double t) 
{
	return (1 / t) * v; // ���������*����
}

// �������
inline double dot(const vec3& u, const vec3& v) 
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// �������
inline vec3 cross(const vec3& u, const vec3& v) 
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		        u.e[2] * v.e[0] - u.e[0] * v.e[2],
		        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// ��λ������normalize
inline vec3 unit_vector(const vec3& v) 
{
	return v / v.length();
}

// ��rejection method��һ�������ڲ��ľ�������ĵ�λ����
// Ϊɶ�����壿��Ȼ3d�ռ��о��ȵ��������Ӧ�÷ֲ���������
// ��ֱ�Ӹ����������-1.0��1.0���죬�ɵ�һ���������ڵ��������
// ���������ڲ�������������Ǿ��ȵģ������Ҫ�ų������������ڶ���������Ĳ���
// �����㷨�൱���ų������Ƚϼ򵥣�ֻ���п��ܷ���reject,Ӱ�����Ч��
// �ο�referrence/rejection method.png
inline vec3 random_unit_vector()
{
	while (true)
	{
		vec3 v = vec3::random(-1.0, 1.0);
		double lensq = v.length_squared();
		// ��ֹdouble���Ȳ������µ�lensq����0�����³���Ϊ0��bug
		// ����double���ͣ�����1e-160������ʶ��
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

// ����������ָ��ƽ���ڲ�����ת������
inline vec3 random_on_hemisphere(const vec3& normal)
{
	vec3 v = random_unit_vector();

	if (dot(v, normal) > 0.0)
		return v;
	else
		return -v;
}

// �������䷽��ͷ��߷������淴�䷽��
// �ο�referrence/mirror reflection.jpg
// ����� ���� ����Ҫ�ǵ�λ����
inline vec3 reflect(const vec3& v, const vec3& normal)
{
	return v - 2 * dot(v, normal) * normal;
}

// �������䷽��ͷ��߷��������䷽��ǰ���Ǽ�������⣬����⣬���߶��ǵ�λ����
// �ο�referrence/refraction.jpg
// unit_v:����⣬�����ǵ�λ���� uint_n:���淨�ߣ������ǵ�λ���� ri:��������ʵ���������������
// ��dot��cos��������fminΪ1�����ƣ���sqrt��������fabs���ƣ���߽�׳��
inline vec3 refract(const vec3& unit_v, const vec3& uint_n, double ri)
{
	double cos_theta = fmin(dot(-unit_v, uint_n), 1.0);
	vec3 out_perp = ri * (unit_v + uint_n * cos_theta);
	vec3 out_para = -sqrt(fabs(1.0 - out_perp.length_squared())) * uint_n;
	return out_perp + out_para;
}

