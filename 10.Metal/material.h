﻿#pragma once

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
	//
	// 散乱特性を計算する仮想関数
	// ray_in: 入射光線
	// rec: 交点情報（材質情報を含む）
	// attenuation: エネルギー減衰率（表面での反射率）
	// ray_out: 散乱後の光線
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
		// 根据lambertian reflection模型算出的随机反射方向，和normal夹角越小，概率越
		// 
		// 参考referrence/lambertian reflection.png
		// 
		// Lambertian反射モデルに基づくランダム反射方向計算
		// 法線方向との角度が小さいほど確率が高くなる
		vec3 dir = rec.normal + random_unit_vector();

		// ゼロベクトル回避
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
		// 参照：referrence/fuzzy reflection.png
		// fuzz:金属反射后再进行一次方向随机，使金属看起来有磨砂效果
		// 注意，这里需要根据fuzz的值来确定效果的程度，所以其他的因子(dir向量，fuzz的向量)要是归一化的，fuzz的值才有意义
		// 
		// fuzz: 金属反射後に方向ベクトルをランダム化して表面のつや消し効果を表現
		// 注意: 効果の強さをfuzz値で制御するため、元の反射方向(dir)と乱れベクトルは
		// 単位ベクトルに正規化されている必要があります（物理的に意味のある計算のため）
		dir = unit_vector(dir) + fuzz * random_unit_vector();
		ray_out = ray(rec.p, dir);
		attenuation = albedo;

		return true;
	}

private:
	color albedo;
	double fuzz;
};