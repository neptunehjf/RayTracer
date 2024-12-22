#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

// hittable_list��һϵ��hittable����ļ��ϣ��������������Ͽ���һ������Ļ���ҲӦ�ü̳�hittable���������
// ���屻�����hittable�ˣ�û����״�����hittable_list������Ҳû�취 �ж�hit
// hittable_list���Կ��ɶ��ڶ������hit�ж���һ����ܣ�����hit�ж�������������״����
class hittable_list : public hittable
{
public:
	// hittable��shared_ptrָ��ļ���
	// ʹ��shared_ptr�������ڴ氲ȫ��
	vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { objects.push_back(object); }

	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool is_hit = false;

		// ����һ��hittable����ֻ���ҵ�����Ľ���
		// ������ϴ�ѭ�����ҵ��������tֵ ��̬�޸��ж���Χ�����ֵ,Ч�ʱȽϸ�
		double closest_t = ray_tmax;

		for (const auto& object : objects)
		{
			is_hit = object->hit(r, ray_tmin, closest_t, temp_rec);
			if (is_hit)
			{
				closest_t = temp_rec.t;
				rec = temp_rec;
			}		
		}

		return is_hit;
	}
};