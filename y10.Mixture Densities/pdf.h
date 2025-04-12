#pragma once

#include"onb.h"

class pdf
{
public:
	virtual ~pdf() = default;

    // PDF分布に従うランダムサンプリング方向を生成
	virtual vec3 generate() const = 0;

	// サンプリング方向に対応するPDF値を返す
	virtual double value(const vec3& direction) const = 0;
};

// 単位球体上で均一サンプリングを実行
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

// lambertianサンプリング
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
		// サーフェス法線方向と反対側の散乱方向を防止
		return fmax(0, cosine_theta / pi);
	}
private:
	onb uvw;
};

class hittable_pdf : public pdf 
{
public:
	hittable_pdf(const hittable& objects, const point3& origin)
		: objects(objects), origin(origin)
	{}

	vec3 generate() const override 
	{
		return objects.random(origin);
	}

	double value(const vec3& direction) const override 
	{
		return objects.pdf_value(origin, direction);
	}

private:
	const hittable& objects;
	point3 origin;
};

// 混合密度pdf，对不同pdf加权，加权总和要等于1
// 複合密度PDF：異なるPDFに重み付け、総和が1に等しいことを保証
class mixture_pdf : public pdf 
{
public:
	mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) 
	{
		p[0] = p0;
		p[1] = p1;
	}

	vec3 generate() const override
	{
		if (random_double() < 0.5)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

	double value(const vec3& direction) const override 
	{
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}

private:
	shared_ptr<pdf> p[2];
};