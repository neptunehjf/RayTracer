#pragma once

#include "common.h"

class onb
{
public:
	onb(const vec3& n)
	{
		axis[2] = unit_vector(n);
		vec3 temp;
		// ����һ����ʱ�Ĳ���nƽ�е�����z
		if (fabs(axis[2].x()) > 0.9)
			temp = vec3(0, 1, 0);
		else
			temp = vec3(1, 0, 0);
		// cross�����x y��ķ�����һ���ܶ�Ӧ�ϣ�����ûӰ�죬��Ϊ�������ֻ��z��н����
		// ������ʱ������n�����n��ֱ������v
		axis[1] = unit_vector(cross(axis[2], temp));
		// u = w x v
		axis[0] = cross(axis[2], axis[1]);
	}

	const vec3& u() const { return axis[0]; }
	const vec3& v() const { return axis[1]; }
	const vec3& w() const { return axis[2]; }

	// ����ռ�ת��
	// ע�����ﲻ����normalize
	vec3 transform(const vec3& v) const 
	{
		return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
	}

private:
	vec3 axis[3];
};