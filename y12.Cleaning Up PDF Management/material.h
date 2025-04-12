#pragma once

#include "hittable.h"
#include "texture.h"
#include "pdf.h"

// 散乱関連データ管理クラス
class scatter_record
{
public:
	color attenuation;        // 散乱減衰（散乱後の残存エネルギーを色で表現
	shared_ptr<pdf> pdf_ptr;  // PDFポインタ（散乱方向の確率分布とPDF値を管理）
	bool no_pdf;              // PDF不使用フラグ（鏡面反射・屈折など方向固定の場合に有効）
	ray no_pdf_ray;           // 鏡面反射/屈折用の固定方向散乱光線‌
};

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
	virtual bool scatter(const ray& ray_in, const hit_record& rec, scatter_record& srec) const
	{
		return false;
	}

	// 発光体材質のデフォルト設定：非発光（黒色）
	// 単色光源では通常パラメータu、v、pを使用しない
	// テクスチャマップ光源など特殊効果実装時にパラメータ活用可能
	virtual color emit(const hit_record& rec, double u, double v, const point3& p) const
	{
		return color(0, 0, 0);
	}

	// 获取当前渲染的散射方向的pdf，也就是最终的收敛目标
	//
	// 現在のレンダリングにおける散乱方向の確率密度関数(PDF)を取得。これが最終的な収束目標となる
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
		// 根据lambertian reflection模型算出的随机反射方向，和normal夹角越小，概率越
		// 
		// 参考referrence/lambertian reflection.png
		// 
		// Lambertian反射モデルに基づくランダム反射方向計算
		// 法線方向との角度が小さいほど確率が高くなる

		// 先根据z轴生成随机向量，再把坐标系转成法线作为z轴的坐标空间(切线坐标)
		// 最初にZ軸基準でランダムベクトルを生成し、法線を新Z軸とする接空間座標系に変換
		// 参考referrence/lambertian reflection.png
		//onb uvw(rec.normal);
		//vec3 out_dir = uvw.transform(random_cosine_direction());

		// ゼロベクトル回避
		//if (out_dir.near_zero())
		//	out_dir = rec.normal;

		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
		// 物体の運動における巨視的時間は光線伝播の微視的時間よりも遥かに大きいため、timeは維持されています
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
		srec.no_pdf = false;

		return true;
	}

	// 获取当前渲染的散射方向的pdf，也就是最终的收敛目标
	//
	// 現在のレンダリングにおける散乱方向の確率密度関数(PDF)を取得。これが最終的な収束目標となる
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
		// 参照：referrence/fuzzy reflection.png
		// fuzz:金属反射后再进行一次方向随机，使金属看起来有磨砂效果
		// 注意，这里需要根据fuzz的值来确定效果的程度，所以其他的因子(dir向量，fuzz的向量)要是归一化的，fuzz的值才有意义
		// 
		// fuzz: 金属反射後に方向ベクトルをランダム化して表面のつや消し効果を表現
		// 注意: 効果の強さをfuzz値で制御するため、元の反射方向(dir)と乱れベクトルは
		// 単位ベクトルに正規化されている必要があります（物理的に意味のある計算のため）
		out_dir = unit_vector(out_dir) + fuzz * random_unit_vector();

		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
		// 物体の運動における巨視的時間は光線伝播の微視的時間よりも遥かに大きいため、timeは維持されています
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
		// 屈折率の計算（媒質の内外判定）
		double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

		vec3 unit_v = unit_vector(ray_in.direction());
		double cos_theta = fmin(dot(-unit_v, rec.normal), 1.0);
		double sin_theta = sqrt(1 - cos_theta * cos_theta);

		vec3 out_dir;

		// Schlick's approximation，模拟Fresnel现象，这里暂时用简单的random_double()引入随机性
		// 
		// 全反射判定（スネルの法則）とシュリック近似による反射確率計算
		// 
		// 参照: referrence/total internal reflection.png
		if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double())
		{
			// 反射方向計算
			out_dir = reflect(unit_v, rec.normal);
		}
		else
		{
			// 屈折方向計算（入力ベクトルは正規化済み）
			out_dir = refract(unit_v, rec.normal, ri);
		}

		srec.attenuation = color(1.0, 1.0, 1.0); // エネルギー吸収なし（完全屈折）
		srec.pdf_ptr = nullptr;
		srec.no_pdf = true;
		// 因为物体运动的宏观时间远大于光线传播的微观时间，所以time保持不变即可
		// 物体の運動における巨視的時間は光線伝播の微視的時間よりも遥かに大きいため、timeは維持されています
		srec.no_pdf_ray = ray(rec.p, out_dir, ray_in.time());

		return true;
	}

private:
	// 被折射介质的内外的相对折射率
	// 注意空气在介质外的时候，分母可忽略，因为系数是1.0
	//
	// 相対屈折率（外部媒質に対する内部媒質の比）
	// 注: 外部が空気の場合は分母を1.0とみなせる
	double refraction_index;

	// 求反射率。根据Fresnel现象，grazing角度观察，反光越强
	//
	// フレネル反射率計算（シュリック近似）
	// グラージング角（浅い入射角）で反射率が上昇する現象を表現
	static double reflectance(double cos_theta, double ri) 
	{
		// Schlick's approximation 参照referrence/schlick approximation.png
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
//
// 等方性材料は全ての方向で特性が同一
// ここでは散乱方向が全方向に均一に分布すると簡略化して計算可能
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