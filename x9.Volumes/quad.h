#pragma once

#include "common.h"
#include "hittable.h"

// 四辺形
// 参照 referrence/ray-quad-intersection.jpg
class quad : public hittable
{
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) :
		Q(Q), u(u), v(v), mat(mat) 
	{
		// 四辺形のbboxを計算
		auto bbox1 = aabb(Q, Q + u + v);
		auto bbox2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox1, bbox2);

		// 参照 referrence/ray-quad-intersection.jpg ②
		vec3 n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);
		// 参照 referrence/ray-quad-intersection.jpg ③
		// 注意：wの法線は正規化前の値を使用（n = cross(u, v)のまま）
		w = n / dot(n, n);
	}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
		// 参照 referrence/ray-quad-intersection.jpg ①

		// 分母計算
		double denom = dot(normal, r.direction());

		// 和sphere不同，quad不需要考虑grazing case，且quad没有厚度
		// 因此只要ray与lab平行就一定返回false
		//
		// 球体との違い：四辺形は厚みがなく、掠入射ケースを考慮不要
		// レイが平面と平行なら即時falseを返す

		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		point3 intersection = r.at(t);
		// 以上只能说明交点在该平面内
		// 以下计算该交点是否在该四边形内 P=Q+αu+βv
		// 
		// // ここまでの計算は平面内の交点のみ保証
		// 以下は交点が四辺形内部にあるかとかの計算　P=Q+αu+βv
		// 
		// 参照 referrence/ray-quad-intersection.jpg
		vec3 p = intersection - Q;
		double alpha = dot(w, cross(p, v));
		double beta = dot(w, cross(u, p));

		if (!is_inside(alpha, beta, rec))
			return false;

		// 衝突情報の記録
		rec.p = intersection;
		rec.set_face_normal(r, normal);
		rec.t = t;
		rec.mat = mat;

        return true;
    }

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	// Q,u,vが四辺形を定義
	point3 Q; // 四辺形の起点
	vec3 u, v; // 四辺形の辺ベクトル
	shared_ptr<material> mat; // マテリアル
	aabb bbox; // AABBバウンディングボックス
	vec3 normal; // 法線ベクトル
	double D; // 平面方程式のD値
	vec3 w; // 計算用一時ベクトル

	bool is_inside(double alpha, double beta, hit_record& rec) const
	{
		// alphaとbetaが[0,1]範囲内なら交点が四辺形内部にある
		interval unit_interval(0, 1);

		if (!unit_interval.contains(alpha) || !unit_interval.contains(beta))
			return false;

		// [0,1]範囲をUVマッピングに直接利用
		rec.u = alpha;
		rec.v = beta;
		return true;
	}
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
	// ボックスは6つの面で構成されるためhittable_listを使用
	auto sides = make_shared<hittable_list>();

	// 対角頂点の最小値と最大値を計算
	auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
	auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

	// 各面の基底ベクトルを定義
	auto dx = vec3(max.x() - min.x(), 0, 0);
	auto dy = vec3(0, max.y() - min.y(), 0);
	auto dz = vec3(0, 0, max.z() - min.z());

	// 6つのでボックスを構築
	sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));   // 前面（Z最大）
	sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // 右面（X最大）
	sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // 背面（Z最小）
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // 左面（X最小）
	sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // 上面（Y最大）
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // 底面（Y最小）

	return sides;
}