#pragma once

#include "common.h"
#include "rtw_stb_image.h"

class texture
{
public:
	virtual ~texture() = default;

	// u,v UV映射的方式，提前准备的2D材质图片
	// p   空间(spatial)材质的方式，每次在交点根据位置算出对应的材质颜色值
	virtual color get_value(double u, double v, const point3& p) const = 0;
};

// 纯色
class solid_color : public texture
{
public:
	solid_color(const color& albedo) : albedo(albedo) {}

	solid_color(double r, double g, double b) : solid_color(color(r, g, b)) {}

	color get_value(double u, double v, const point3& p) const override
	{
		// 纯色是固定值
		return albedo;
	}

private:
	color albedo;
};

// 程式化生成 棋盘格子材质 
class checker_texture : public texture
{
public:
	checker_texture(double density, shared_ptr<texture> even, shared_ptr<texture> odd) :
		density(density), even(even), odd(odd) {}

	checker_texture(double density, const color& c1, const color& c2) :
		checker_texture(density, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

	// 程式化生成奇偶交错的纯色
	color get_value(double u, double v, const point3& p) const override
	{
		// 注意floor返回值和参数一致，所以要强制转换
		auto x_integer = (int)floor(density * p.x());
		auto y_integer = (int)floor(density * p.y());
		auto z_integer = (int)floor(density * p.z());

		bool is_even = ((x_integer + y_integer + z_integer) % 2 == 0);

		return is_even ? even->get_value(u, v, p) : odd->get_value(u, v, p);
	}

private:
	// 格子密度。density越大，格子越小越密集；
	// 格子大小scale可能更符合直觉，但是因为要计算scale的倒数，效率较低
	double density;

	// 奇偶交错的两种花纹
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

// 基于UV的材质贴图
class image_texture : public texture 
{
public:
	image_texture(const char* filename) : image(filename) {}

	color get_value(double u, double v, const point3& p) const override 
	{
		// 如果没读取到贴图，就返回青色
		if (image.height() <= 0) return color(0.0, 1.0, 1.0);

		// u坐标范围应该在[0.0,1.0]
		u = interval(0.0, 1.0).clamp(u);
		// v坐标范围应该在[1,0]，因为实际的图片坐标是和右手系坐标在y方向相反的
		v = 1.0 - interval(0.0, 1.0).clamp(v);

		double i = int(u * image.width());
		double j = int(v * image.height());
		const unsigned char* pixel = image.pixel_data(i, j);

		double color_scale = 1.0 / 255.0; // 整数转浮点
		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	rtw_image image;
};