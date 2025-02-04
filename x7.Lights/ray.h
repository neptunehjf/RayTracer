#pragma once

#include "common.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction), tm(0.0) {}
	ray(const point3& origin, const vec3 direction, double time) : o(origin), d(direction), tm(time) {}

	const point3& origin() const { return o; } //��Ϊreturn�������ã����Է���ֵҲҪ��const���ܱ�֤�����޸�
	const vec3& direction() const { return d; }

	// ���ray��ʱ��t��λ��
	point3 at(double t) const
	{
		return o + t * d;
	}

	// ��ȡ��ǰʱ��
	double get_time() const
	{
		return tm;
	}

private:
	point3 o; // ��ʼ��
	vec3 d;   // d�Ĵ�С�൱���ٶ�(��������������λ���йأ�������΢�۵Ĺ��ߵ��ٶȣ����ߵ��ٶȿ���Ϊ�������)

	// camera�������ߵ�ʱ�̣�����ģ�ⲻͬʱ���˶������λ�ñ仯
	// ֻ��Ϊ�˱���������˶����Ǻ�������ʱ�䣬ҪԶԶ����΢�۹��ߴ�����ʱ�䣬���۹��߷������ٴΣ���������Ϊ�ǹ̶���
	double tm = 0.0; 
};