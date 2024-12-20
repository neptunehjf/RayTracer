#pragma once

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(point3& center, double radius) : center(center), radius(fmax(0.0, radius)) {}

	// ��ʾ��override�ؼ��ֵĺô��ǣ�һ�ǿɶ��Ժã����Ǳ������ᵱ����д���������
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = center - o;

        // ת��Ϊ����ʽ���δ֪��t�����⣬�Ƶ����̲ο�derivation/sphere_hit.jpg
        // �򻯹��̲ο�derivation/quadratic_simplify.jpg
        double a = d.length_squared();
        double h = dot(d, oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;

        // ����б�ʽ����0����ray��sphereֻ��һ������
        // ����б�ʽ����0����ray��sphere����������
        // ����б�ʽС��0����ray��sphere��û�н���
        double t, t_min, t_max;
        if (discriminant >= 0)
        {
            t_min = (h - sqrt(discriminant)) / a;
            t_max = (h + sqrt(discriminant)) / a;

            // ����ray_tmin~ray_tmax��Χ�ڵ�����Ľ���
            if ((t_min > ray_tmin && t_min < ray_tmax))
                t = t_min;
            else if ((t_min > ray_tmin && t_min < ray_tmax))
                t = t_max;
            else
                return false;

            // ��������ļ�¼
            rec.p = r.at(t);
            rec.normal = (rec.p - center) / radius;
            rec.t = t;
            return true;
        }
            
        return false;
	}

private:
	point3 center;  // ����
    double radius;  // �뾶
};