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
		vec3 out_dir = rec.normal + random_unit_vector();

		//��ֹdir��0���������
		if (out_dir.near_zero())
			out_dir = rec.normal;

		ray_out = ray(rec.p, out_dir);
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
		vec3 out_dir = reflect(ray_in.direction(), rec.normal);
		// fuzz:����������ٽ���һ�η��������ʹ������������ĥɰЧ��
		// �ο���referrence/fuzzy reflection.png
		// ע�⣬������Ҫ����fuzz��ֵ��ȷ��Ч���ĳ̶ȣ���������������(dir������fuzz������)Ҫ�ǹ�һ���ģ�fuzz��ֵ��������
		out_dir = unit_vector(out_dir) + fuzz * random_unit_vector();
		ray_out = ray(rec.p, out_dir);
		attenuation = albedo;

		return true;
	}

private:
	color albedo;
	double fuzz;
};

class dielectrics : public material
{
public:
	dielectrics(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& ray_in, const hit_record& rec,
		color& attenuation, ray& ray_out) const override
	{
		double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

		vec3 unit_v = unit_vector(ray_in.direction());
		double cos_theta = fmin(dot(-unit_v, rec.normal), 1.0);
		double sin_theta = sqrt(1 - cos_theta * cos_theta);

		vec3 out_dir;

		// ȫ�ڷ��䣬�ο�referrence/total internal reflection.png
		// Schlick's approximation��ģ��Fresnel����������ʱ�ü򵥵�random_double()���������
		if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double())
		{
			out_dir = reflect(unit_v, rec.normal);
		}
		else
		{
			// ����
			// ����������ν�Ϊ��λ����
			out_dir = refract(unit_v, rec.normal, ri);
		}

		ray_out = ray(rec.p, out_dir);

		// ������������ȫ�������������
		attenuation = color(1.0, 1.0, 1.0); 

		return true;
	}

private:
	// ��������ʵ���������������
	// ע������ڽ������ʱ�򣬷�ĸ�ɺ��ԣ���Ϊϵ����1.0
	double refraction_index;

	// �����ʡ�����Fresnel����grazing�Ƕȹ۲죬����Խǿ
	static double reflectance(double cos_theta, double ri) 
	{
		// Schlick's approximation �ο�referrence/schlick approximation.png
		double r0 = (1 - ri) / (1 + ri);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cos_theta), 5);
	}
};