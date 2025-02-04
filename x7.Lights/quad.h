#pragma once

#include "common.h"
#include "hittable.h"

// �ı���
// ���� referrence/ray-quad-intersection.jpg
class quad : public hittable
{
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) :
		Q(Q), u(u), v(v), mat(mat) 
	{
		// �����ı��ε�bbox
		auto bbox1 = aabb(Q, Q + u + v);
		auto bbox2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox1, bbox2);

		// ���� referrence/ray-quad-intersection.jpg ��
		vec3 n = cross(u, v);
		normal = unit_vector(n);
		D = dot(normal, Q);
		// ���� referrence/ray-quad-intersection.jpg ��
		// ע��w�ķ���Ҫ��δ��һ���ģ���Ϊn = cross(u, v);
		w = n / dot(n, n);
	}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
		// ���� referrence/ray-quad-intersection.jpg ��

		// ��ĸ
		double denom = dot(normal, r.direction());

		// ��sphere��ͬ��quad����Ҫ����grazing case����quadû�к��
		// ���ֻҪray��labƽ�о�һ������false
		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		point3 intersection = r.at(t);
		// ����ֻ��˵�������ڸ�ƽ����
		// 
		// ���¼���ý����Ƿ��ڸ��ı����� P=Q+��u+��v
		// ���� referrence/ray-quad-intersection.jpg
		vec3 p = intersection - Q;
		double alpha = dot(w, cross(p, v));
		double beta = dot(w, cross(u, p));

		if (!is_inside(alpha, beta, rec))
			return false;

		// ��������ļ�¼
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
	// Q,u,v������һ���ı���
	point3 Q;  // �ı�����ʼ��
	vec3 u, v; // �ı��ε�������
	shared_ptr<material> mat; //����
	aabb bbox; // aabb��Χ��
	vec3 normal; // ����
	double D; // Dֵ
	vec3 w; //��ʱ���������ڼ򻯼���

	bool is_inside(double alpha, double beta, hit_record& rec) const
	{
		// ���alpha��beta����0��1�ķ�Χ�ڣ�˵��������

		interval unit_interval(0, 1);

		if (!unit_interval.contains(alpha) || !unit_interval.contains(beta))
			return false;

		// ��Χ����[0, 1��]���ÿ�������UV mapping
		rec.u = alpha;
		rec.v = beta;
		return true;
	}
};