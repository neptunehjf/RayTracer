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
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		hit_record rec1, rec2;

		// 参照referrence/volume_scatter1.jpg
		// 先求出光线与volume边界是否有2个交点
		if (!boundary->hit(r, interval::universe, rec1))
			return false;

		interval intv = interval(rec1.t + 0.0001, inf);

		if (!boundary->hit(r, intv, rec2))
			return false;

		// 以上是用universe求出的交点，实际还要考虑光线有效区间
		// 光线有效区间范围在volume边界内部的情况，要把交点更新为区间内的值
		if (rec1.t < ray_t.min)
			rec1.t = ray_t.min;
		if (rec2.t > ray_t.max)
			rec2.t = ray_t.max;

		// 以上计算是根据 rec1.t < rec2.t 作为前提的，所以不满足则返回false
		if (rec1.t >= rec2.t)
			return false;

		// 第一个交点在光线反方向的情况，把光线原点视为第一个交点
		if (rec1.t < 0)
			rec1.t = 0;

		// 光线速度的标量
		double ray_speed = r.direction().length();
		// 不发生scatter的情况下，光线在volume内部的传播距离
		double distance = ray_speed * (rec2.t - rec1.t);
		// 光线发生scatter的情况下，光线volume内部的传播距离
		// 参照referrence/volume_scatter2.png
		double hit_distance = neg_inv_density * log(random_double());

		if (hit_distance < distance)
			return false;

		rec.t = rec1.t + hit_distance / ray_speed;
		rec.p = r.at(rec.t);
		rec.front_face = true;       // 不关心，任意值都可以
		rec.normal = vec3(1, 0, 0);  // 不关心，任意值都可以
		rec.mat = mat;
	}

	aabb bounding_box() const override
	{
		return boundary->bounding_box();
	}

private:
	shared_ptr<hittable> boundary; // 指定一个hittable物体作为volume的边界
	double neg_inv_density;        // volume密度的倒数的负数，具体参照referrence/volume_scatter2.png
	shared_ptr<material> mat;      // 体积的材质
};