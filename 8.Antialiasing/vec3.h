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

	// һԪ����������Ҫ����
	// �g헥ޥ��ʥ������ӣ�������Ҫ��
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); } 

	// ��Ϊ���ص���һ����ֵ���ͣ����Բ����޸ĳ�Ա��ֵ����˼�const����
	// �������������棨���Ф������ʤ�����const���
	double operator[](int i) const { return e[i]; } 

	//��Ϊ���ص���һ�����ã������п����޸ĳ�Ա������ֵ�����Բ��ܼ�const����
	// �Ƕ������հ棨���Љ���ο����Ԥ����뤿��const��ʤ���
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
		return (*this) *= (1.0 / t); // �ȴ�� *= ��������
	}


	// �٥��ȥ���L��Ӌ��
	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return (e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}

	
};

// �׺�Ӌ���åݥ���ȥ����פ΄e�����x
using point3 = vec3;

// �������ߺ��������Ա����������һԪ���㣬���������ⲿ�����Ԫ���㡣
// ��Ա����Ĭ����inline�����������ⲿ��Ҫ�ֶ�ָ��
// 
// �٥��ȥ��å�`�ƥ���ƥ��v��Ⱥ�����饹�����v���Ǥυg������Ӥ򥪩`�Щ`��`�ɤ���
// ��������Ӥϥ��饹�ⲿ�Ƕ��x���Ƥ��ޤ�
// �����v���ϰ��a�Ĥ�inline�Q���Ȥʤ�ޤ��������饹�ⲿ���x�Ǥ���ʾ�Ĥ�inlineָ������Ҫ�Ǥ�
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
	return (1 / t) * v; // �ȴ�� *= ��������
}

// �������
// �ڷeӋ��
inline double dot(const vec3& u, const vec3& v) 
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

// �������
// ��eӋ��
inline vec3 cross(const vec3& u, const vec3& v) 
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		        u.e[2] * v.e[0] - u.e[0] * v.e[2],
		        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// ��Ҏ�����gλ�٥��ȥ뻯��
inline vec3 unit_vector(const vec3& v) 
{
	return v / v.length();
}