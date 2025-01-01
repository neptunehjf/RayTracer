#pragma once

#include "hittable.h"

class material
{
public:
	virtual ~material() = default;

	//��ͬ�Ĳ��ʵ�ɢ�����Բ�ͬ���������ֻ����ȫ�麯���������ɸ�������ʵ��
	// ray_in �������
	// rec ������Ϣ�������˾���Ĳ�������
	// attenuation ����˥��(��������������յ�������˵)�����Ǳ�������������
	// ray_out ������ߣ�ɢ����ߣ�
	virtual bool scatter(const ray& ray_in, const hit_record& rec,
		                 color& attenuation, ray& ray_out) const
	{
		return false;
	}
};

class diffuse : public material
{
public:
	diffuse(const color& albedo) : albedo(albedo) {}

	bool scatter(const ray& ray_in, const hit_record& rec,
		color& attenuation, ray& ray_out) const override
	{
		// ����lambertian reflectionģ�������������䷽�򣬺�normal�н�ԽС������Խ��
		// �ο�referrence/lambertian reflection.png
		vec3 dir = rec.normal + random_unit_vector();

		//��ֹdir��0���������
		if (dir.near_zero())
			dir = rec.normal;

		ray_out = ray(rec.p, dir);
		attenuation = albedo;

		return true;
	}

private:
	color albedo;
};

class metal : public material
{
public:
	metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	bool scatter(const ray& ray_in, const hit_record& rec,
		color& attenuation, ray& ray_out) const override
	{
		vec3 dir = reflect(ray_in.direction(), rec.normal);
		// fuzz:����������ٽ���һ�η��������ʹ������������ĥɰЧ��
		// �ο���referrence/fuzzy reflection.png
		// ע�⣬������Ҫ����fuzz��ֵ��ȷ��Ч���ĳ̶ȣ���������������(dir������fuzz������)Ҫ�ǹ�һ���ģ�fuzz��ֵ��������
		dir = unit_vector(dir) + fuzz * random_unit_vector();
		ray_out = ray(rec.p, dir);
		attenuation = albedo;

		return true;
	}

private:
	color albedo;
	double fuzz;
};