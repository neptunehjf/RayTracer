#pragma once

#include "common.h"
#include "hittable.h"

// 四边形
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
	}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
		// to be implemented
        return false;
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
};