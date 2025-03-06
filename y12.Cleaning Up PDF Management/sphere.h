#pragma once

#include "common.h"
#include "hittable.h"
#include "onb.h"

class sphere : public hittable
{
public:
    // ��̬��
	sphere(const point3& center0, double radius, shared_ptr<material> mat) : 
        center(center0, vec3(0.0, 0.0, 0.0)), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        bbox = aabb(center0 - r, center0 + r);
    }

    // ��̬����һ��ʱ��Ŀ�ʼ���������2����ͬ��λ��
    // ���t == 0��������center1�����ʱ��t == 1��������center2���Դ˳����Ա仯
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) : 
        center(center1, center2 - center1), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        aabb bbox1(center.at(0.0) - r, center.at(0.0) + r);
        aabb bbox2(center.at(1.0) - r, center.at(1.0) + r);
        bbox = aabb(bbox1, bbox2);
    }

	// ��ʾ��override�ؼ��ֵĺô��ǣ�һ�ǿɶ��Ժã����Ǳ������ᵱ����д���������
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
        point3 current_center = center.at(r.time());
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

            // ǰ������ԭ��Ϊ���ģ��뾶Ϊ1�����壬������Ӧ����outward_normal������rec.p
            // outward_normal����������ĵ�һ����λ���������Ҳ���Կ�������ԭ��Ϊ���ĵİ뾶Ϊ1��λ�ã�����uv�ļ���ǰ��
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }
            
        return false;
	}

    aabb bounding_box() const override
    {
        return bbox;
    }

    // ����referrence/samples_a_sphere.jpg �Ģ�
    double pdf_value(const point3& origin, const vec3& direction) const override 
    {
        // Ŀǰֻ֧�־�̬��

        hit_record rec;
        if (!this->hit(ray(origin, direction), interval(0.001, inf), rec))
            return 0;

        double dist_squared = (center.at(0) - origin).length_squared();
        double cos_theta_max = std::sqrt(1 - radius * radius / dist_squared);
        double solid_angle = 2 * pi * (1 - cos_theta_max);

        return  1 / solid_angle;
    }

    vec3 random(const point3& origin) const override 
    {
        vec3 direction = center.at(0) - origin;
        double distance_squared = direction.length_squared();
        onb uvw(direction);
        // ���߿ռ�ת����ռ�
        return uvw.transform(random_to_sphere(radius, distance_squared));
    }

private:
	ray center;  // ���ģ��Լ����ܵ��˶����򡣷������������normalize����ΪҪ�ò�ͬ���������������ֲ�ͬ����Ĳ�ͬ�ٶ�
    double radius;  // �뾶
    shared_ptr<material> mat; //����
    aabb bbox; // aabb��Χ��

    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // ǰ������ԭ��Ϊ���ģ��뾶Ϊ1������
        // �ѿ������� =���������� ���� referrence/cartesian_to_spherical.jpg
        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;

        // �������� =��UV����
        u = phi / (2 * pi);
        v = theta / pi;
    }

    // ����referrence/samples_a_sphere.jpg �Ģ�
    static vec3 random_to_sphere(double radius, double distance_squared) 
    {
        double r1 = random_double();
        double r2 = random_double();
        double z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

        double phi = 2 * pi * r1;
        double x = cos(phi) * sqrt(1 - z * z);
        double y = sin(phi) * sqrt(1 - z * z);

        return vec3(x, y, z);
    }
};