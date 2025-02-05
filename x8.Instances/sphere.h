#pragma once

#include "common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
    // 静态球
	sphere(const point3& center0, double radius, shared_ptr<material> mat) : 
        center(center0, vec3(0.0, 0.0, 0.0)), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        bbox = aabb(center0 - r, center0 + r);
    }

    // 动态球，在一段时间的开始与结束，有2个不同的位置
    // 如果t == 0，球心在center1；如果时间t == 1，球心在center2。以此呈线性变化
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) : 
        center(center1, center2 - center1), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        aabb bbox1(center.at(0.0) - r, center.at(0.0) + r);
        aabb bbox2(center.at(1.0) - r, center.at(1.0) + r);
        bbox = aabb(bbox1, bbox2);
    }

	// 显示加override关键字的好处是，一是可读性好，二是编译器会当作重写函数来检查
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
        point3 current_center = center.at(r.time());
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = current_center - o; // 根据ray的时刻算出球心位置

        // 转化为二项式求解未知数t的问题，推导过程参考referrence/sphere_hit.jpg
        // 简化过程参考referrence/quadratic_simplify.jpg
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
            if (ray_t.surrounds(t_min))
                t = t_min;
            else if (ray_t.surrounds(t_max))
                t = t_max;
            else
                return false;

            // 传出交点的记录
            rec.p = r.at(t);
            // 1.注意这里并没用unit_vector方法计算outward_normal，这样可以减少sqrt运算，提升效率
            // 2.不需要normalize的地方就尽量不要normalize
            vec3 outward_normal = (rec.p - current_center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.t = t;
            rec.mat = mat;

            // 前提是以原点为球心，半径为1的球体，因此入参应该是outward_normal而不是rec.p
            // outward_normal是相对于球心的一个单位向量，因此也可以看作是以原点为球心的半径为1的位置，符合uv的计算前提
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }
            
        return false;
	}

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
	ray center;  // 球心，以及可能的运动方向。方向的向量不能normalize，因为要用不同的向量长度来表现不同物体的不同速度
    double radius;  // 半径
    shared_ptr<material> mat; //材质
    aabb bbox; // aabb包围盒

    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // 前提是以原点为球心，半径为1的球体
        // 笛卡尔坐标 =》球面坐标 参照 referrence/cartesian_to_spherical.jpg
        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;

        // 球面坐标 =》UV坐标
        u = phi / (2 * pi);
        v = theta / pi;
    }
};