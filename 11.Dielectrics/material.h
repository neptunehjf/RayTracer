#pragma once

#include "hittable.h"

class material
{
public:
	virtual ~material() = default;

	//不同的材质的散射特性不同，因此这里只定义全虚函数，具体由各材质来实现
	// ray_in 入射光线
	// rec 交点信息，包含了具体的材质类型
	// attenuation 能量衰减(对于物体表面吸收的能量来说)，就是被反射能量比例
	// ray_out 出射光线（散射光线）
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
		// 根据lambertian reflection模型算出的随机反射方向，和normal夹角越小，概率越大
		// 参考referrence/lambertian reflection.png
		vec3 out_dir = rec.normal + random_unit_vector();

		//防止dir是0向量的情况
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
		// fuzz:金属反射后再进行一次方向随机，使金属看起来有磨砂效果
		// 参考：referrence/fuzzy reflection.png
		// 注意，这里需要根据fuzz的值来确定效果的程度，所以其他的因子(dir向量，fuzz的向量)要是归一化的，fuzz的值才有意义
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

		// 全内反射，参考referrence/total internal reflection.png
		// Schlick's approximation，模拟Fresnel现象，这里暂时用简单的random_double()引入随机性
		if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double())
		{
			out_dir = reflect(unit_v, rec.normal);
		}
		else
		{
			// 折射
			// 这里向量入参皆为单位向量
			out_dir = refract(unit_v, rec.normal, ri);
		}

		ray_out = ray(rec.p, out_dir);

		// 不吸收能量，全部反射或者折射
		attenuation = color(1.0, 1.0, 1.0); 

		return true;
	}

private:
	// 被折射介质的内外的相对折射率
	// 注意空气在介质外的时候，分母可忽略，因为系数是1.0
	double refraction_index;

	// 求反射率。根据Fresnel现象，grazing角度观察，反光越强
	static double reflectance(double cos_theta, double ri) 
	{
		// Schlick's approximation 参考referrence/schlick approximation.png
		double r0 = (1 - ri) / (1 + ri);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cos_theta), 5);
	}
};