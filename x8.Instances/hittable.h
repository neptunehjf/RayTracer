#pragma once

#include "common.h"
#include "aabb.h"

// ��Ϊmaterial.h�Ѿ�������hittable.h��Ϊ�˷�ֹ���������������ֻ�����ķ�ʽ
class material;

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t = 0.0;
	shared_ptr<material> mat;
	double u;
	double v;

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
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	// ��������İ�Χ��
	virtual aabb bounding_box() const = 0;
};

// ƽ���࣬���������ƽ��
// 1 ��ʵ������ĵ�hit�жϷ�װ��һ�㣬�ù���(camera)����λ�Ƶķ�ʽ����λ�ƺ��������hit�ж�
// �ù���(camera)λ�Ƶķ�ʽ�����Ա���Ը�����״��ʵ���������λ�Ʋ���
// 2 ���Ƕ���ʵ������İ�Χ�У���Ϊ��Χ����״�򵥣�ֱ�Ӷ԰�Χ����λ�Ʋ�������������Ҫ��ÿ��hit��λ�ƣ�ֻ���ʼ�������ʱ��λ�ơ�Ч�ʸ���
// ���ϣ�����ʵ�����壬��λ��ʵ�����壬�ùⷴ��λ�ƽ���hit�жϣ����ڰ�Χ�У�λ�ư�Χ�У��ñ����Ĺ��߽���hit�ж�
class translate : public hittable
{
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset)
	{
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// �����ƶ���offset ,�൱��camera(������ʼ��)�ƶ�-offset
		ray offset_r(r.origin() + offset, r.direction(), r.time());

		// ���ƶ���Ĺ�����δ�ƶ����������hit�ж�
		if (object->hit(offset_r, ray_t, rec))
		{
			// ���hit�ˣ�ʵ��������rec.p + offset��hit��
			rec.p += offset;
			return true;
		}
		else
			return false;

	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};