#pragma once

#include "common.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction) {}

	//��Ϊreturn�������ã����Է���ֵҲҪ��const���ܱ�֤�����޸�
    // ԭ��β��դ�const��ȡ�ã����շ�ȴ�Τ�������ֹ��
	const point3& origin() const { return o; }

	const vec3& direction() const { return d; }

	// ���ray��ʱ��t��λ��
	// �r��t�ˤ�����ray��λ�����ˤ�Ӌ��
	point3 at(double t) const
	{
		return o + t * d;
	}

private:
	point3 o;
	vec3 d;
};