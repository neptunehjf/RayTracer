#pragma once

#include "common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(const point3& center, double radius) : center(center), radius(fmax(0.0, radius)) {}

	// ��ʾ��override�ؼ��ֵĺô��ǣ�һ�ǿɶ��Ժã����Ǳ������ᵱ����д���������
    // overrideָ�������㣺
    // 1. ���i�����ϣ���ʾ�Ĥʥ��`�Щ`�饤�ɣ�
    // 2. ����ѥ���ˤ�륪�`�Щ`�饤�ɥ����å�
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = center - o;

        // ���η���ʽ�ν�Ȥ����nͻ��t������
        // �����^�̣�referrence/sphere_hit.jpg
        // ʽ���Ի���referrence/quadratic_simplify.jpg
        double a = d.length_squared();
        double h = dot(d, oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;

        // �Єeʽ�ˤ���nͻ�ж�
        // >0: 2����, =0: 1����, <0: �nͻ�ʤ�
        double t, t_min, t_max;
        if (discriminant >= 0)
        {
            t_min = (h - sqrt(discriminant)) / a;
            t_max = (h + sqrt(discriminant)) / a;

            // �쥤���Є������ڤ�������������x�k
            if (ray_t.surrounds(t_min))
                t = t_min;
            else if (ray_t.surrounds(t_max))
                t = t_max;
            else
                return false;

            // �nͻ����ӛ�h
            rec.p = r.at(t);

            // 1.ע�����ﲢû��unit_vector��������outward_normal���������Լ���sqrt���㣬����Ч��
            // 2.����Ҫnormalize�ĵط��;�����Ҫnormalize
            //
            // 1. unit_vector��ʹ�ä������򷨾���Ӌ�� �� sqrtӋ����������p�ˤ��I�턿������
            // 2. ��Ҏ������Ҫ�ʹw���Ǥ�Ӌ�㥳���Ȥθߤ�normalize�I���ر�
            vec3 outward_normal = (rec.p - center) / radius; 
            rec.set_face_normal(r, outward_normal);
            rec.t = t;
            return true;
        }
            
        return false;
	}

private:
	point3 center;  // ������������
    double radius;  // ����뾶
};