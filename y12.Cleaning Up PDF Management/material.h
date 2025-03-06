#pragma once

#include "hittable.h"
#include "texture.h"
#include "pdf.h"

// 管理散射相关数据
class scatter_record
{
public:
	color attenuation; // 散射衰减，表示散射后留下的能量(颜色)
	shared_ptr<pdf> pdf_ptr; // pdf指针，管理了散射的随机方向和对应的pdf值
	bool no_pdf;    // 无pdf标志位，适用镜面反射，折射等散射方向固定的情况
	ray no_pdf_ray; // 镜面反射，折射等的固定散射方向
};


class material
{
public:
	virtual ~material() = default;

	//不同的材质的散射特性不同，因此这里只定义全虚函数，具体由各材质来实现
	// ray_in 入射光线
	// rec 交点信息，包含了具体的材质类型
	// attenuation 能量衰减(对于物体表面吸收的能量来说)，就是被反射能量比例
	// scatter 出射光线（散射光线）
	virtual bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const
	{
		return false;
	}

	// 光源材质，默认不发光(黑色)
	// 纯色光源一般用不到参数u,v,p，要实现一些特殊效果(比如带贴图的光源)的话可能会用到
	virtual color emit(const hit_record& rec, double u, double v, const point3& p) const
	{
		return color(0, 0, 0);
	}

	// 获取当前渲染的散射方向的pdf，也就是最终的收敛目标
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
		// 根据lambertian reflection模型算出的随机反射方向，和normal夹角越小，概率越大
		// 参考referrence/lambertian reflection.png
		//vec3 out_dir = rec.normal + random_unit_vector();

		// 先根据z轴生成随机向量，再把坐标系转成法线作为z轴的坐标空间(切线坐标)
		// 参考referrence/lambertian reflection.png
		//onb uvw(rec.normal);
		//vec3 out_dir = uvw.transform(random_cosine_direction());

		//防止dir是0向量的情况
		//if (out_dir.near_zero())
		//	out_dir = rec.normal;

		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
		srec.no_pdf = false;

		return true;
	}

	// 获取当前渲染的散射方向的pdf，也就是最终的收敛目标
	// 参照 referrence/Lambertian Scatter PDF.jpg
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
		// fuzz:金属反射后再进行一次方向随机，使金属看起来有磨砂效果
		// 参考：referrence/fuzzy reflection.png
		// 注意，这里需要根据fuzz的值来确定效果的程度，所以其他的因子(dir向量，fuzz的向量)要是归一化的，fuzz的值才有意义
		out_dir = unit_vector(out_dir) + fuzz * random_unit_vector();

		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
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

		srec.attenuation = color(1.0, 1.0, 1.0); // 不吸收能量，全部折射
		srec.pdf_ptr = nullptr;
		srec.no_pdf = true;
		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
		srec.no_pdf_ray = ray(rec.p, out_dir, ray_in.time());

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

// 各项同性材料，在各个方向上性质相同。
// 此处可简单地认为散射方向在个方向的几率是相等的
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