#pragma once

#include "vec3.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction) {}

	const point3& origin() const { return o; } //��Ϊreturn�������ã����Է���ֵҲҪ��const���ܱ�֤�����޸�
	const vec3& direction() const { return d; }

	// ���ray��ʱ��t��λ��
	point3 at(double t) const
	{
		return o + t * d;
	}

private:
	point3 o;
	vec3 d;
};