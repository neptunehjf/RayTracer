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

	// ������originΪ��㵽��ǰ������������
	virtual vec3 random(const point3& origin) const
	{
		return vec3(1, 0, 0);
	}

	// ����random���ɵ����������pdfֵ
	virtual double pdf_value(const point3& origin, const vec3& direction) const 
	{
		return 0.0;
	}


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
		// �����°�Χ��
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// �����ƶ���offset ,�൱��camera(����)�ƶ�-offset
		ray offset_r(r.origin() - offset, r.direction(), r.time());

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

// ��ת�࣬����������y����ת
// ʵ��˼·�������ƽ������һ�µ�
class rotate_y : public hittable
{
public:
	rotate_y(shared_ptr<hittable> object, double angle) : object(object)
	{
		double radian = degree_to_radian(angle);
		sin_theta = sin(radian);
		cos_theta = cos(radian);
		bbox = object->bounding_box();

		// Ĭ�ϰ�Χ����empty
		point3 min(inf, inf, inf);
		point3 max(-inf, -inf, -inf);

		// ���ڰ�Χ�е�8�����㣬ÿ���ֱ���תtheta��Ȼ��ÿ����ת�������min max��ֵ���������min max����һ���µİ�Χ��
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					// ѡ�񶥵�
					double x = (1 - i) * bbox.x.min + i * bbox.x.max;
					double y = (1 - j) * bbox.y.min + j * bbox.y.max;
					double z = (1 - k) * bbox.z.min + k * bbox.z.max;

					// ��ת����
					double x_rotate = x * cos_theta + z * sin_theta;
					double z_rotate = x * (-sin_theta) + z * cos_theta;

					// ����min,max��ֵ
					vec3 temp(x_rotate, y, z_rotate);
					for (int i = 0; i < 3; i++)
					{
						min[i] = fmin(min[i], temp[i]);
						max[i] = fmin(max[i], temp[i]);
					}
				}

		// �����°�Χ��
		bbox = aabb(min, max);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// ������ת��theta ,�൱��camera(����)��ת-theta
		// �����ת��ʽ(����referrence/rotation formula.jpg) �ɵ����½��
		auto origin = point3(r.origin().x() * cos_theta - r.origin().z() * sin_theta, 
			                 r.origin().y(), 
			                 r.origin().x() * sin_theta + r.origin().z() * cos_theta);

		auto direction = vec3(r.direction().x() * cos_theta - r.direction().z() * sin_theta,
					          r.direction().y(),
			                  r.direction().x() * sin_theta + r.direction().z() * cos_theta);
		
		ray rotate_r(origin, direction, r.time());

		// ����ת��Ĺ�����δ��ת���������hit�ж�
		if (object->hit(rotate_r, ray_t, rec))
		{
			// ���hit�ˣ�ʵ��������rec.p ��ת��+theta��hit��
			rec.p = point3(rec.p.x() * cos_theta + rec.p.z() * sin_theta,
			               rec.p.y(),
						   rec.p.x() * (-sin_theta) + rec.p.z() * cos_theta);

			// ��ת�Ļ�������ҲӦ�û�ԭ��+theta��
			rec.normal = point3(rec.normal.x() * cos_theta + rec.normal.z() * sin_theta,
				                rec.normal.y(),
				                rec.normal.x() * (-sin_theta) + rec.normal.z() * cos_theta);

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
	double sin_theta;
	double cos_theta;
	aabb bbox;
};