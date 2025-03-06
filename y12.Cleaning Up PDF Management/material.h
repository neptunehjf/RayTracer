#pragma once

#include "hittable.h"
#include "texture.h"
#include "pdf.h"

// ����ɢ���������
class scatter_record
{
public:
	color attenuation; // ɢ��˥������ʾɢ������µ�����(��ɫ)
	shared_ptr<pdf> pdf_ptr; // pdfָ�룬������ɢ����������Ͷ�Ӧ��pdfֵ
	bool no_pdf;    // ��pdf��־λ�����þ��淴�䣬�����ɢ�䷽��̶������
	ray no_pdf_ray; // ���淴�䣬����ȵĹ̶�ɢ�䷽��
};


class material
{
public:
	virtual ~material() = default;

	//��ͬ�Ĳ��ʵ�ɢ�����Բ�ͬ���������ֻ����ȫ�麯���������ɸ�������ʵ��
	// ray_in �������
	// rec ������Ϣ�������˾���Ĳ�������
	// attenuation ����˥��(��������������յ�������˵)�����Ǳ�������������
	// scatter ������ߣ�ɢ����ߣ�
	virtual bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const
	{
		return false;
	}

	// ��Դ���ʣ�Ĭ�ϲ�����(��ɫ)
	// ��ɫ��Դһ���ò�������u,v,p��Ҫʵ��һЩ����Ч��(�������ͼ�Ĺ�Դ)�Ļ����ܻ��õ�
	virtual color emit(const hit_record& rec, double u, double v, const point3& p) const
	{
		return color(0, 0, 0);
	}

	// ��ȡ��ǰ��Ⱦ��ɢ�䷽���pdf��Ҳ�������յ�����Ŀ��
	virtual double scatter_pdf(const ray& ray_in, const hit_record& rec, const ray& scatter) const
	{
		return 0;
	}
};

class diffuse : public material
{
public:
	diffuse(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
	diffuse(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const override
	{
		// ����lambertian reflectionģ�������������䷽�򣬺�normal�н�ԽС������Խ��
		// �ο�referrence/lambertian reflection.png
		//vec3 out_dir = rec.normal + random_unit_vector();

		// �ȸ���z����������������ٰ�����ϵת�ɷ�����Ϊz�������ռ�(��������)
		// �ο�referrence/lambertian reflection.png
		//onb uvw(rec.normal);
		//vec3 out_dir = uvw.transform(random_cosine_direction());

		//��ֹdir��0���������
		//if (out_dir.near_zero())
		//	out_dir = rec.normal;

		// ��Ϊ�����˶��ĺ��ʱ��Զ���ڹ��ߴ�����΢��ʱ�䣬����time���ֲ��伴��
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
		srec.no_pdf = false;

		return true;
	}

	// ��ȡ��ǰ��Ⱦ��ɢ�䷽���pdf��Ҳ�������յ�����Ŀ��
	// ���� referrence/Lambertian Scatter PDF.jpg
	double scatter_pdf(const ray& ray_in, const hit_record& rec, const ray& scatter) const override
	{
		double cos_theta = dot(rec.normal, unit_vector(scatter.direction()));
		return cos_theta < 0 ? 0 : cos_theta / pi;
	}

private:
	shared_ptr<texture> tex;
};

class metal : public material
{
public:
	metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const override
	{
		vec3 out_dir = reflect(ray_in.direction(), rec.normal);
		// fuzz:����������ٽ���һ�η��������ʹ������������ĥɰЧ��
		// �ο���referrence/fuzzy reflection.png
		// ע�⣬������Ҫ����fuzz��ֵ��ȷ��Ч���ĳ̶ȣ���������������(dir������fuzz������)Ҫ�ǹ�һ���ģ�fuzz��ֵ��������
		out_dir = unit_vector(out_dir) + fuzz * random_unit_vector();

		// ��Ϊ�����˶��ĺ��ʱ��Զ���ڹ��ߴ�����΢��ʱ�䣬����time���ֲ��伴��
		srec.attenuation = albedo;
		srec.pdf_ptr = nullptr;
		srec.no_pdf = true;
		srec.no_pdf_ray = ray(rec.p, out_dir, ray_in.time());

		return true;
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material
{
public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const override
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

		srec.attenuation = color(1.0, 1.0, 1.0); // ������������ȫ������
		srec.pdf_ptr = nullptr;
		srec.no_pdf = true;
		// ��Ϊ�����˶��ĺ��ʱ��Զ���ڹ��ߴ�����΢��ʱ�䣬����time���ֲ��伴��
		srec.no_pdf_ray = ray(rec.p, out_dir, ray_in.time());

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

class diffuse_light : public material
{
public:
	diffuse_light(const color& light_color) : tex(make_shared<solid_color>(light_color)) {}
	diffuse_light(shared_ptr<texture> tex) : tex(tex) {}

	color emit(const hit_record& rec, double u, double v, const point3& p) const override
	{
		if (!rec.front_face)
			return color(0, 0, 0);
		return tex->value(u, v, p);
	}

private:
	shared_ptr<texture> tex;
};

// ����ͬ�Բ��ϣ��ڸ���������������ͬ��
// �˴��ɼ򵥵���Ϊɢ�䷽���ڸ�����ļ�������ȵ�
class isotropic : public material 
{
public:
	isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
	isotropic(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const override 
	{
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<sphere_pdf>();
		srec.no_pdf = false;
		return true;
	}

	double scatter_pdf(const ray& ray_in, const hit_record& rec, const ray& scatter) const override
	{
		return 1 / (4 * pi);
	}

private:
	shared_ptr<texture> tex;
};