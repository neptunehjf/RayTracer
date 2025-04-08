#pragma once

#include "common.h"

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t;

	// 判断ray是从物体外部还是内部打过来的
	// 当前情况下，在几何阶段做判断结果并存储可能是一种比较高效的选择
	//
	// rayが物体の外部/内部どちらから衝突したかを示すフラグ
	// 幾何計算段階で判定結果を保持することは効率的な設計選択
	bool front_face = true; 

	// 根据ray和外部法线的方向判断当前光线是从内部还是外部来的，以及求实际的normal
	// ray方向と外向き法線から衝突面の向きを判定し法線方向を設定
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// 要确保传入的outward_normal是单位向量
		// 外向き法線は単位ベクトルである必要あり

		if (dot(r.direction(), outward_normal) > 0)
		{
			// 如果ray方向和外部法线的方向一致，ray来自物体内部
			// レイ方向と外向き法線が同方向 → 物体内部からの衝突
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// 如果ray方向和外部法线的方向相反，ray来自物体外部
			// レイ方向と外向き法線が逆方向 → 物体外部からの衝突
			front_face = true;
			normal = outward_normal;
		}
	}
};

// 抽象类，具体放到子类实现
// 抽象基底クラス（実装はサブクラスで行う）
class hittable
{
public:
	virtual ~hittable() = default;

	// 纯虚函数，具体放到子类实现
	// 純粋仮想関数（サブクラスで実装必須）
	virtual bool hit(const ray& r, interval& ray_t, hit_record& rec) const = 0;
};