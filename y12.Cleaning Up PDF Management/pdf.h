#pragma once

#include"onb.h"

class pdf
{
public:
	virtual ~pdf() = default;

	// ���ɷ���pdf�ֲ��������������
	virtual vec3 generate() const = 0;

	// ���ݲ������򷵻ض�Ӧ��pdfֵ
	virtual double value(const vec3& direction) const = 0;
};

// �ڵ�λ������Ȳ���
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

// lambertian����
class cosine_pdf : public pdf
{
public:
	cosine_pdf(const vec3& w) : uvw(w) {}


	vec3 generate() const override
	{
		// �ȸ���z����������������ٰ�����ϵת�ɷ�����Ϊz�������ռ�(��������)
		// �ο�referrence/lambertian reflection.png
		return uvw.transform(random_cosine_direction());
	}

	// ����referrence/Lambertian Scatter PDF.jpg
	double value(const vec3& direction) const override
	{
		double cosine_theta = dot(unit_vector(direction), uvw.w());
		// ��ֹɢ�䷽����ƽ�����µ����
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

// ����ܶ�pdf���Բ�ͬpdf��Ȩ����Ȩ�ܺ�Ҫ����1
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