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

		// 叉乘向量的长度等于面积
		area = n.length();
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

	// 参照referrence/PDF of a Light.png
	double pdf_value(const point3& origin, const vec3& direction) const override 
	{
		hit_record rec;
		if (!this->hit(ray(origin, direction), interval(0.001, inf), rec))
			return 0;

		double distance_squared = rec.t * rec.t * direction.length_squared();
		double cosine = fabs(dot(direction, rec.normal) / direction.length());

		return distance_squared / (cosine * area);
	}

	vec3 random(const point3& origin) const override 
	{
		// 光线射向物体表面随机的一个点
		point3 p = Q + (random_double() * u) + (random_double() * v);
		return p - origin;
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
	double area; //面积

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

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
	// 一个box可以看作6个quad，因此box是hittable_list
	auto sides = make_shared<hittable_list>();

	// 对角顶点min max
	auto min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
	auto max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

	// quad的u v向量
	auto dx = vec3(max.x() - min.x(), 0, 0);
	auto dy = vec3(0, max.y() - min.y(), 0);
	auto dz = vec3(0, 0, max.z() - min.z());

	// 6个quad组成一个box
	sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
	sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
	sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
	sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

	return sides;
}