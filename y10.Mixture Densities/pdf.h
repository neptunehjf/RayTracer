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