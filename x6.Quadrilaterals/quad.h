#pragma once

#include "common.h"
#include "hittable.h"

// 四边形
// 参照 referrence/ray-quad-intersection.jpg
class quad : public hittable
{
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) :
		Q(Q), u(u), v(v), mat(mat) 
	{
		// 计算四边形的bbox
		auto bbox1 = aabb(Q, Q + u + v);
		auto bbox2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox1, bbox2);

		// 参照 referrence/ray-quad-intersection.jpg ②
		vec3 n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);
		// 参照 referrence/ray-quad-intersection.jpg ③
		// 注意w的法线要用未归一化的，因为n = cross(u, v);
		w = n / dot(n, n);
	}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
		// 参照 referrence/ray-quad-intersection.jpg ①

		// 分母
		double denom = dot(normal, r.direction());

		// 和sphere不同，quad不需要考虑grazing case，且quad没有厚度
		// 因此只要ray与lab平行就一定返回false
		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		point3 intersection = r.at(t);
		// 以上只能说明交点在该平面内
		// 
		// 以下计算该交点是否在该四边形内 P=Q+αu+βv
		// 参照 referrence/ray-quad-intersection.jpg
		vec3 p = intersection - Q;
		double alpha = dot(w, cross(p, v));
		double beta = dot(w, cross(u, p));

		if (!is_inside(alpha, beta, rec))
			return false;

		// 传出交点的记录
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
	// Q,u,v决定了一个四边形
	point3 Q;  // 四边形起始点
	vec3 u, v; // 四边形的两条边
	shared_ptr<material> mat; //材质
	aabb bbox; // aabb包围盒
	vec3 normal; // 法线
	double D; // D值
	vec3 w; //临时向量，用于简化计算

	bool is_inside(double alpha, double beta, hit_record& rec) const
	{
		// 如果alpha，beta都在0到1的范围内，说明交点在

		interval unit_interval(0, 1);

		if (!unit_interval.contains(alpha) || !unit_interval.contains(beta))
			return false;

		// 范围都是[0, 1，]正好可以用于UV mapping
		rec.u = alpha;
		rec.v = beta;
		return true;
	}
};