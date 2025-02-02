#pragma once

#include "common.h"
#include "hittable.h"

// �ı���
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
	// Q,u,v������һ���ı���
	point3 Q;  // �ı�����ʼ��
	vec3 u, v; // �ı��ε�������
	shared_ptr<material> mat; //����
	aabb bbox; // aabb��Χ��
};