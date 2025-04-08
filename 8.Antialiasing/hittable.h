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
	//
	// ray��������ⲿ/�ڲ��ɤ��餫���nͻ��������ʾ���ե饰
	// �׺�Ӌ����A���ж��Y���򱣳֤��뤳�Ȥτ��ʵĤ��OӋ�x�k
	bool front_face = true; 

	// ����ray���ⲿ���ߵķ����жϵ�ǰ�����Ǵ��ڲ������ⲿ���ģ��Լ���ʵ�ʵ�normal
	// ray��������򤭷��������nͻ����򤭤��ж�������������O��
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// Ҫȷ�������outward_normal�ǵ�λ����
		// ���򤭷����υgλ�٥��ȥ�Ǥ����Ҫ����

		if (dot(r.direction(), outward_normal) > 0)
		{
			// ���ray������ⲿ���ߵķ���һ�£�ray���������ڲ�
			// �쥤��������򤭷�����ͬ���� �� �����ڲ�������nͻ
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// ���ray������ⲿ���ߵķ����෴��ray���������ⲿ
			// �쥤��������򤭷������淽�� �� �����ⲿ������nͻ
			front_face = true;
			normal = outward_normal;
		}
	}
};

// �����࣬����ŵ�����ʵ��
// ������ץ��饹���gװ�ϥ��֥��饹���Ф���
class hittable
{
public:
	virtual ~hittable() = default;

	// ���麯��������ŵ�����ʵ��
	// ���������v�������֥��饹�ǌgװ��횣�
	virtual bool hit(const ray& r, interval& ray_t, hit_record& rec) const = 0;
};