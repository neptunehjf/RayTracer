#pragma once

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(point3& center, double radius) : center(center), radius(fmax(0.0, radius)) {}

	// 显示加override关键字的好处是，一是可读性好，二是编译器会当作重写函数来检查
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = center - o;

        // 转化为二项式求解未知数t的问题，推导过程参考derivation/sphere_hit.jpg
        // 简化过程参考derivation/quadratic_simplify.jpg
        double a = d.length_squared();
        double h = dot(d, oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;

        // 如果判别式等于0，则ray和sphere只有一个交点
        // 如果判别式大于0，则ray和sphere有两个交点
        // 如果判别式小于0，则ray和sphere有没有交点
        double t, t_min, t_max;
        if (discriminant >= 0)
        {
            t_min = (h - sqrt(discriminant)) / a;
            t_max = (h + sqrt(discriminant)) / a;

            // 求在ray_tmin~ray_tmax范围内的最近的交点
            if ((t_min > ray_tmin && t_min < ray_tmax))
                t = t_min;
            else if ((t_min > ray_tmin && t_min < ray_tmax))
                t = t_max;
            else
                return false;

            // 传出交点的记录
            rec.p = r.at(t);
            rec.normal = (rec.p - center) / radius;
            rec.t = t;
            return true;
        }
            
        return false;
	}

private:
	point3 center;  // 球心
    double radius;  // 半径
};