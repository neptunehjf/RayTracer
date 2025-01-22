#pragma once

#include "common.h"
#include "hittable.h"


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
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) 
	{ 
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// hit_record temp_rec;
		bool is_hit = false;

		// ����һ��hittable����ֻ���ҵ�����Ľ���
		// ������ϴ�ѭ�����ҵ��������tֵ ��̬�޸��ж���Χ�����ֵ,Ч�ʱȽϸ�
		for (const auto& object : objects)
		{
			if (object->hit(r, ray_t, rec))
			{
				// ��Ϊray_t��Ϊֵ���ݿ����и�����Ż��ռ䣬�˴����Ż�����
				// ������bvh�ṹ���Ѿ������Ƶ��Ż��ˣ����left�ڵ� hit�Ļ�������right�ڵ��ray.maxҲ�����Ϊrec.t
				// ray_t.max = temp_rec.t; 

				//rec = temp_rec;
				is_hit = true;
			}		
		}

		return is_hit;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	aabb bbox; // Ĭ�Ϲ��� �ǿ�aabb�������ʼ��
};