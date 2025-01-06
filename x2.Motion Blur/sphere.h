#pragma once

#include "common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
    // ��̬��
	sphere(const point3& center, double radius, shared_ptr<material> mat) : 
        center(center, vec3(0.0, 0.0, 0.0)), radius(fmax(0.0, radius)), mat(mat) {}

    // ��̬����һ��ʱ��Ŀ�ʼ���������2����ͬ��λ��
    // ���t == 0��������center1�����ʱ��t == 1��������center2���Դ˳����Ա仯
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) : 
        center(center1, center2 - center1), radius(fmax(0.0, radius)), mat(mat) {}

	// ��ʾ��override�ؼ��ֵĺô��ǣ�һ�ǿɶ��Ժã����Ǳ������ᵱ����д���������
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
        point3 current_center = center.at(r.get_time());
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = current_center - o; // ����ray��ʱ���������λ��

        // ת��Ϊ����ʽ���δ֪��t�����⣬�Ƶ����̲ο�referrence/sphere_hit.jpg
        // �򻯹��̲ο�referrence/quadratic_simplify.jpg
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
            if (ray_t.surrounds(t_min))
                t = t_min;
            else if (ray_t.surrounds(t_max))
                t = t_max;
            else
                return false;

            // ��������ļ�¼
            rec.p = r.at(t);
            // 1.ע�����ﲢû��unit_vector��������outward_normal���������Լ���sqrt���㣬����Ч��
            // 2.����Ҫnormalize�ĵط��;�����Ҫnormalize
            vec3 outward_normal = (rec.p - current_center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.t = t;
            rec.mat = mat;
            return true;
        }
            
        return false;
	}

private:
	ray center;  // ���ģ��Լ����ܵ��˶����򡣷������������normalize����ΪҪ�ò�ͬ���������������ֲ�ͬ����Ĳ�ͬ�ٶ�
    double radius;  // �뾶
    shared_ptr<material> mat; //����
};