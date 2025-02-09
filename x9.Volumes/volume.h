#pragma once

#include "common.h"
#include "hittable.h"
#include "texture.h"
#include "interval.h"

class volume : public hittable
{
public:
	volume(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex) :
		   boundary(boundary), neg_inv_density(-1 / density), mat(make_shared<isotropic>(tex))
	{}

	volume(shared_ptr<hittable> boundary, double density, const color& albedo) :
		boundary(boundary), neg_inv_density(-1 / density), mat(make_shared<isotropic>(albedo))
	{}

	// �����ܶȺ͸��������volume�ڲ�hit��λ��
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		hit_record rec1, rec2;

		// ����referrence/volume_scatter1.jpg
		// �����������volume�߽��Ƿ���2������
		if (!boundary->hit(r, interval::universe, rec1))
			return false;

		interval intv = interval(rec1.t + 0.0001, inf);

		if (!boundary->hit(r, intv, rec2))
			return false;

		// ��������universe����Ľ��㣬ʵ�ʻ�Ҫ���ǹ�����Ч����
		// ������Ч���䷶Χ��volume�߽��ڲ��������Ҫ�ѽ������Ϊ�����ڵ�ֵ
		if (rec1.t < ray_t.min)
			rec1.t = ray_t.min;
		if (rec2.t > ray_t.max)
			rec2.t = ray_t.max;

		// ���ϼ����Ǹ��� rec1.t < rec2.t ��Ϊǰ��ģ����Բ������򷵻�false
		if (rec1.t >= rec2.t)
			return false;

		// ��һ�������ڹ��߷������������ѹ���ԭ����Ϊ��һ������
		if (rec1.t < 0)
			rec1.t = 0;

		// �����ٶȵı���
		double ray_speed = r.direction().length();
		// ������scatter������£�������volume�ڲ��Ĵ�������
		double distance = ray_speed * (rec2.t - rec1.t);
		// ���߷���scatter������£�����volume�ڲ��Ĵ�������
		// ����referrence/volume_scatter2.png
		double hit_distance = neg_inv_density * log(random_double());

		if (hit_distance < distance)
			return false;

		rec.t = rec1.t + hit_distance / ray_speed;
		rec.p = r.at(rec.t);
		rec.front_face = true;       // �����ģ�����ֵ������
		rec.normal = vec3(1, 0, 0);  // �����ģ�����ֵ������
		rec.mat = mat;
	}

	aabb bounding_box() const override
	{
		return boundary->bounding_box();
	}

private:
	shared_ptr<hittable> boundary; // ָ��һ��hittable������Ϊvolume�ı߽�
	double neg_inv_density;        // volume�ܶȵĵ����ĸ������������referrence/volume_scatter2.png
	shared_ptr<material> mat;      // ����Ĳ���
};