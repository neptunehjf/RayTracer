#pragma once

#include "common.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class texture
{
public:
	virtual ~texture() = default;

	// u,v UV映射的方式，提前准备的2D材质图片
	// p   空间(spatial)材质的方式，每次在交点根据位置算出对应的材质颜色值
	// 
	// u,v: UVマッピング用テクスチャ座標
	// p: 空間座標ベースのテクスチャリング用パラメータ
	virtual color value(double u, double v, const point3& p) const = 0;
};

// 単色
class solid_color : public texture
{
public:
	solid_color(const color& albedo) : albedo(albedo) {}

	solid_color(double r, double g, double b) : solid_color(color(r, g, b)) {}

	color value(double u, double v, const point3& p) const override
	{
		// 単色は固定値を使用
		return albedo;
	}

private:
	color albedo;
};

// 程式化生成 棋盘格子材质 
//
// プロシージャル生成チェッカーパターン
class checker_texture : public texture
{
public:
	checker_texture(double density, shared_ptr<texture> even, shared_ptr<texture> odd) :
		density(density), even(even), odd(odd) {}

	checker_texture(double density, const color& c1, const color& c2) :
		checker_texture(density, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

	// 程式化生成奇偶交错的纯色
	//
	// 3Dチェッカーパターンの生成ロジック
	color value(double u, double v, const point3& p) const override
	{
		// 注意floor返回值和参数一致，所以要强制转换
		//
		// floor関数の戻り値型は引数と一致させるため明示的にキャスト
		auto x_integer = (int)floor(density * p.x());
		auto y_integer = (int)floor(density * p.y());
		auto z_integer = (int)floor(density * p.z());

		bool is_even = ((x_integer + y_integer + z_integer) % 2 == 0);

		return is_even ? even->value(u, v, p) : odd->value(u, v, p);
	}

private:
	// 格子密度。density越大，格子越小越密集；
	// 格子大小scale可能更符合直觉，但是因为要计算scale的倒数，效率较低
	//
	// パターンの密度（密度が高いほど細かいパターン）
	// scaleパラメータより除算演算が不要なため効率的
	double density;

	// 奇偶交错的两种花纹
	// 
	// 交互に配置する2種類のテクスチャ
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

// 基于UV的材质贴图
// 
// 画像ベースUVテクスチャ
class image_texture : public texture 
{
public:
	image_texture(const char* filename) : image(filename) {}

	color value(double u, double v, const point3& p) const override 
	{
		// 如果没读取到贴图，就返回青色
		// 
		// 画像読み込み失敗時のフォールバック（シアン色）
		if (image.height() <= 0) return color(0.0, 1.0, 1.0);

		// u座標のクランプ処理 [0.0, 1.0]範囲に制限
		u = interval(0.0, 1.0).clamp(u);
		// v座標反転処理（画像座標系と右手座標系のY軸方向違いを補正）
		v = 1.0 - interval(0.0, 1.0).clamp(v);

		double i = int(u * image.width());
		double j = int(v * image.height());
		const unsigned char* pixel = image.pixel_data(i, j);

		double color_scale = 1.0 / 255.0; // 整数から浮動小数点への変換ス
		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	rtw_image image;
};

class noise_texture : public texture
{
public:
	noise_texture(double frenquency) : frenquency(frenquency) {}

	color value(double u, double v, const point3& p) const
	{
		// noise [-1, 1] => [0, 1]
		// return color(1.0, 1.0, 1.0) * 0.5 * (1.0 + noise.noise(frenquency * p));


		// 湍流 叠加7层
		// 
		// 乱流効果（7層の重畳）
		//return color(1.0, 1.0, 1.0) * noise.turbulence(p, 7);

		// 生成大理石材质
		// 初始值是0.5，用sin波形生成图案，z轴坐标作为自变量，再加上湍流的偏移
		// 
		// 大理石模様生成ロジック
		// ベースカラー0.5（グレイ）に正弦波パターンを適用
		// Z座標を位相変化の軸として使用＋乱流による変調
		return color(0.5, 0.5, 0.5) * (1.0 + sin(frenquency * p.z() + 10 * noise.turbulence(p, 7)));
	}

private:
	perlin noise;
	double frenquency;
};