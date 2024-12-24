#pragma once

#include "common.h"

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t;

	// �ж�ray�Ǵ������ⲿ�����ڲ��������
	// ��ǰ����£��ڼ��ν׶����жϽ�����洢������һ�ֱȽϸ�Ч��ѡ��
	bool front_face = true; 

	// ����ray���ⲿ���ߵķ����жϵ�ǰ�����Ǵ��ڲ������ⲿ���ģ��Լ���ʵ�ʵ�normal
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// Ҫȷ�������outward_normal�ǵ�λ����

		if (dot(r.direction(), outward_normal) > 0)
		{
			// ���ray������ⲿ���ߵķ���һ�£�ray���������ڲ�
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// ���ray������ⲿ���ߵķ����෴��ray���������ⲿ
			front_face = true;
			normal = outward_normal;
		}
	}
};

// �����࣬����ŵ�����ʵ��
class hittable
{
public:
	virtual ~hittable() = default;

	// ���麯��������ŵ�����ʵ��
	virtual bool hit(const ray& r, interval& ray_t, hit_record& rec) const = 0;
};