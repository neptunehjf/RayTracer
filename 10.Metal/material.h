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
		vec3 dir = rec.normal + random_unit_vector();

		//防止dir是0向量的情况
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
	metal(const color& albedo) : albedo(albedo) {}

	bool scatter(const ray& ray_in, const hit_record& rec,
		color& attenuation, ray& ray_out) const override
	{
		vec3 dir = reflect(ray_in.direction(), rec.normal);
		ray_out = ray(rec.p, dir);
		attenuation = albedo;

		return true;
	}

private:
	color albedo;
};