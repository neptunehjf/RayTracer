#pragma once

#include "common.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class texture
{
public:
	virtual ~texture() = default;

	// u,v UVӳ��ķ�ʽ����ǰ׼����2D����ͼƬ
	// p   �ռ�(spatial)���ʵķ�ʽ��ÿ���ڽ������λ�������Ӧ�Ĳ�����ɫֵ
	virtual color get_value(double u, double v, const point3& p) const = 0;
};

// ��ɫ
class solid_color : public texture
{
public:
	solid_color(const color& albedo) : albedo(albedo) {}

	solid_color(double r, double g, double b) : solid_color(color(r, g, b)) {}

	color get_value(double u, double v, const point3& p) const override
	{
		// ��ɫ�ǹ̶�ֵ
		return albedo;
	}

private:
	color albedo;
};

// ��ʽ������ ���̸��Ӳ��� 
class checker_texture : public texture
{
public:
	checker_texture(double density, shared_ptr<texture> even, shared_ptr<texture> odd) :
		density(density), even(even), odd(odd) {}

	checker_texture(double density, const color& c1, const color& c2) :
		checker_texture(density, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

	// ��ʽ��������ż����Ĵ�ɫ
	color get_value(double u, double v, const point3& p) const override
	{
		// ע��floor����ֵ�Ͳ���һ�£�����Ҫǿ��ת��
		auto x_integer = (int)floor(density * p.x());
		auto y_integer = (int)floor(density * p.y());
		auto z_integer = (int)floor(density * p.z());

		bool is_even = ((x_integer + y_integer + z_integer) % 2 == 0);

		return is_even ? even->get_value(u, v, p) : odd->get_value(u, v, p);
	}

private:
	// �����ܶȡ�densityԽ�󣬸���ԽСԽ�ܼ���
	// ���Ӵ�Сscale���ܸ�����ֱ����������ΪҪ����scale�ĵ�����Ч�ʽϵ�
	double density;

	// ��ż��������ֻ���
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

// ����UV�Ĳ�����ͼ
class image_texture : public texture 
{
public:
	image_texture(const char* filename) : image(filename) {}

	color get_value(double u, double v, const point3& p) const override 
	{
		// ���û��ȡ����ͼ���ͷ�����ɫ
		if (image.height() <= 0) return color(0.0, 1.0, 1.0);

		// u���귶ΧӦ����[0.0,1.0]
		u = interval(0.0, 1.0).clamp(u);
		// v���귶ΧӦ����[1,0]����Ϊʵ�ʵ�ͼƬ�����Ǻ�����ϵ������y�����෴��
		v = 1.0 - interval(0.0, 1.0).clamp(v);

		double i = int(u * image.width());
		double j = int(v * image.height());
		const unsigned char* pixel = image.pixel_data(i, j);

		double color_scale = 1.0 / 255.0; // ����ת����
		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	rtw_image image;
};

class noise_texture : public texture
{
public:
	// perlin��Ĭ�Ϲ����幹�죬����Ҫ����
	noise_texture(double frenquency) : frenquency(frenquency) {}

	color get_value(double u, double v, const point3& p) const
	{
		// noise [-1, 1] => [0, 1]
		// return color(1.0, 1.0, 1.0) * 0.5 * (1.0 + noise.noise(frenquency * p));


		// ���� ����7��
		//return color(1.0, 1.0, 1.0) * noise.turbulence(p, 7);

		// ���ɴ���ʯ����
		// ��ʼֵ��1.0��ɫ����sin��������ͼ����z��������Ϊ�Ա������ټ���������ƫ��
		return color(0.5, 0.5, 0.5) * (1.0 + sin(frenquency * p.z() + 10 * noise.turbulence(p, 7)));
	}

private:
	perlin noise;
	double frenquency;
};