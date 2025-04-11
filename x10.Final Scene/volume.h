#pragma once

#include "common.h"
#include "hittable.h"
#include "texture.h"
#include "interval.h"

class volume : public hittable
{
public:
	volume(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex) :
		   boundary(boundary), neg_inv_density(-1 / density), mat(make_shared<isotropic>(tex))
	{}

	volume(shared_ptr<hittable> boundary, double density, const color& albedo) :
		boundary(boundary), neg_inv_density(-1 / density), mat(make_shared<isotropic>(albedo))
	{}

	// 根据密度和概率算出在volume内部hit的位置
	// 
	// 密度と確率に基づき体積内部の衝突位置を算出
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		hit_record rec1, rec2;

		// 参照referrence/volume_scatter1.jpg
		// 
		// 体積境界面と光線が2点で交差するかを判定
		if (!boundary->hit(r, interval::universe, rec1))
			return false;

		interval intv = interval(rec1.t + 0.0001, inf);

		if (!boundary->hit(r, intv, rec2))
			return false;

		// 以上是用universe求出的交点，实际还要考虑光线有效区间
		// 光线有效区间范围在volume边界内部的情况，要把交点更新为区间内的值
		//
		// 有効光線区間内に収まるよう交点をクリッピング
		if (rec1.t < ray_t.min)
			rec1.t = ray_t.min;
		if (rec2.t > ray_t.max)
			rec2.t = ray_t.max;

		// 以上计算是根据 rec1.t < rec2.t 作为前提的，所以不满足则返回false
		//
		// 交差点の前後関係が不正な場合
		if (rec1.t >= rec2.t)
			return false;

		// 第一个交点在光线反方向的情况，把光线原点视为第一个交点
		// 逆方向交差の処理：原点を始点として再設定
		if (rec1.t < 0)
			rec1.t = 0;

		// 光線速度のスカラー量
		double ray_speed = r.direction().length();

		// 不发生scatter的情况下，光线在volume内部的传播距离
		// 散乱なしの場合の体積内伝播距離
		double distance = ray_speed * (rec2.t - rec1.t);

		// 光线发生scatter的情况下，光线volume内部的传播距离
		// 散乱発生時の伝播距離（reference/volume_scatter2.png参照）
		double hit_distance = neg_inv_density * log(random_double());

		if (hit_distance > distance)
			return false;

		rec.t = rec1.t + hit_distance / ray_speed;
		rec.p = r.at(rec.t);
		rec.front_face = true;       // 不要（任意の値で可）
		rec.normal = vec3(1, 0, 0);  // 不要（任意の値で可）
		rec.mat = mat;
	}

	aabb bounding_box() const override
	{
		return boundary->bounding_box();
	}

private:
	shared_ptr<hittable> boundary; // 体積境界を定義するhittableオブジェクト
	double neg_inv_density;        // volume密度の逆数の負数（reference/volume_scatter2.png参照）
	shared_ptr<material> mat;      // 体積材質

};