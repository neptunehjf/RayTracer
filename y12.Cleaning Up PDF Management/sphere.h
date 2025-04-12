#pragma once

#include "common.h"
#include "hittable.h"
#include "onb.h"

class sphere : public hittable
{
public:
    // 静的な球体
	sphere(const point3& center0, double radius, shared_ptr<material> mat) : 
        center(center0, vec3(0.0, 0.0, 0.0)), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        bbox = aabb(center0 - r, center0 + r);
    }

    // 動的な球体（時間区間の始点と終点で2位置を持つ）
    // 
    // t == 0 で中心がcenter1、t == 1 でcenter2に線形補間

    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) : 
        center(center1, center2 - center1), radius(fmax(0.0, radius)), mat(mat) 
    {
        auto r = vec3(radius, radius, radius);
        aabb bbox1(center.at(0.0) - r, center.at(0.0) + r);
        aabb bbox2(center.at(1.0) - r, center.at(1.0) + r);
        bbox = aabb(bbox1, bbox2);
    }

	// 显示加override关键字的好处是，一是可读性好，二是编译器会当作重写函数来检查
    // override指定の利点：
    // 1. 可読性向上（明示的なオーバーライド）
    // 2. コンパイラによるオーバーライドチェック
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
        point3 current_center = center.at(r.time());
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = current_center - o; // レイの時刻に基づいて算出した球心位置

        // 二次方程式の解として衝突点tを求める
        // 導出過程：referrence/sphere_hit.jpg
        // 式簡略化：referrence/quadratic_simplify.jpg
        double a = d.length_squared();
        double h = dot(d, oc);
        double c = oc.length_squared() - radius * radius;

        double discriminant = h * h - a * c;

        // 判別式による衝突判定
        // >0: 2交点, =0: 1交点, <0: 衝突なし
        double t, t_min, t_max;
        if (discriminant >= 0)
        {
            t_min = (h - sqrt(discriminant)) / a;
            t_max = (h + sqrt(discriminant)) / a;

            // レイの有効範囲内で最も近い交点を選択
            if (ray_t.surrounds(t_min))
                t = t_min;
            else if (ray_t.surrounds(t_max))
                t = t_max;
            else
                return false;

            // 衝突情報の記録
            rec.p = r.at(t);

            // 1.注意这里并没用unit_vector方法计算outward_normal，这样可以减少sqrt运算，提升效率
            // 2.不需要normalize的地方就尽量不要normalize
            //
            // 1. unit_vectorを使用せず外向法線を計算 → sqrt計算回数の削減により処理効率向上
            // 2. 正規化が不要な箇所では計算コストの高いnormalize処理を回避
            vec3 outward_normal = (rec.p - current_center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.t = t;
            rec.mat = mat;

            // 前提是以原点为球心，半径为1的球体，因此入参应该是outward_normal而不是rec.p
            // outward_normal是相对于球心的一个单位向量，因此也可以看作是以原点为球心的半径为1的位置，符合uv的计算前提
            //
            // 原点中心・半径1の球体が前提のため、rec.pではなく外向き法線(outward_normal)を引数に使用
            // outward_normalは球心に対する単位ベクトル = 原点中心半径1の球面上の点 → UV計算の前提条件を満たす
            get_sphere_uv(outward_normal, rec.u, rec.v);
            return true;
        }
            
        return false;
	}

    aabb bounding_box() const override
    {
        return bbox;
    }

    // 参照referrence/samples_a_sphere.jpg の②
    double pdf_value(const point3& origin, const vec3& direction) const override 
    {
        // 現在は静的な球体のみ対応

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
        // 接空間からワールド空間への変換
        return uvw.transform(random_to_sphere(radius, distance_squared));
    }

private:
	ray center;  // 球心，以及可能的运动方向。方向的向量不能normalize，因为要用不同的向量长度来表现不同物体的不同速度
                 // 球心および移動方向（方向ベクトルは正規化不可。異なる物体の速度差をベクトル長で表現するため）
    double radius;  // 球体半径
    shared_ptr<material> mat; //材質

    aabb bbox; // aabb bbox


    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // 原点を中心とする半径1の球体が前提
        // デカルト座標 → 球面座標変換 参照: referrence/cartesian_to_spherical.jpg
        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;

        // 球面座標 → UV座標変換
        u = phi / (2 * pi);
        v = theta / pi;
    }

    // 参照referrence/samples_a_sphere.jpg の①
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