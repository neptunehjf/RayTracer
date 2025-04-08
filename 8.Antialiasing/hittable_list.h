#pragma once

#include "common.h"
#include "hittable.h"


// hittable_list��һϵ��hittable����ļ��ϣ��������������Ͽ���һ������Ļ���ҲӦ�ü̳�hittable���������
// ���屻�����hittable�ˣ�û����״�����hittable_list������Ҳû�취 �ж�hit
// hittable_list���Կ��ɶ��ڶ������hit�ж���һ����ܣ�����hit�ж�������������״����
//
// hittable_list���}����hittable������뼯����
// ����ȫ��Ȥ��ƥҥå��ж��C�ܤ�@�Ф��뤬������Ĥ���״�ж��ς����Υ��֥������Ȥ�ί�j
class hittable_list : public hittable
{
public:
	// hittable��shared_ptrָ��ļ���
	// ʹ��shared_ptr�������ڴ氲ȫ��
	// 
	// hittable�Υ��ީ`�ȥݥ�������
	// shared_ptr���äˤ��������ΰ�ȫ������
	vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }
	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool is_hit = false;

		// ����һ��hittable����ֻ���ҵ�����Ľ���
		// ������ϴ�ѭ�����ҵ��������tֵ ��̬�޸��ж���Χ�����ֵ,Ч�ʱȽϸ�
		//
		// ȫ���֥������Ȥˌ������nͻ�ж���gʩ
		// �����������Ҋ�Ĥ��뤿�ᡢ�kҊ����t�����ж������ӵĸ���
		for (const auto& object : objects)
		{
			if (object->hit(r, ray_t, temp_rec))
			{
				ray_t.max = temp_rec.t;
				rec = temp_rec;
				is_hit = true;
			}		
		}

		return is_hit;
	}
};