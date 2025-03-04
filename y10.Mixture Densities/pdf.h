#pragma once

#include"onb.h"

class pdf
{
public:
	virtual ~pdf() = default;

	// 生成符合pdf分布的随机采样方向
	virtual vec3 generate() const = 0;

	// 根据采样方向返回对应的pdf值
	virtual double value(const vec3& direction) const = 0;
};

// 在单位球体均匀采样
class sphere_pdf : public pdf
{
public:
	sphere_pdf() {}


	vec3 generate() const override
	{
		return random_unit_vector();
	}

	double value(const vec3& direction) const override
	{
		return 1 / (4 * pi);
	}
};

// lambertian采样
class cosine_pdf : public pdf
{
public:
	cosine_pdf(const vec3& w) : uvw(w) {}


	vec3 generate() const override
	{
		return uvw.transform(random_cosine_direction());
	}

	// 参照referrence/Lambertian Scatter PDF.jpg
	double value(const vec3& direction) const override
	{
		double cosine_theta = dot(unit_vector(direction), uvw.w());
		// 防止散射方向在平面以下的情况
		return fmax(0, cosine_theta / pi);
	}
private:
	onb uvw;
};