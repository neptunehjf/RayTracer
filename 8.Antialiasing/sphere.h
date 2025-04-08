#pragma once

#include "common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(const point3& center, double radius) : center(center), radius(fmax(0.0, radius)) {}

	// 显示加override关键字的好处是，一是可读性好，二是编译器会当作重写函数来检查
    // override指定の利点：
    // 1. 可読性向上（明示的なオーバーライド）
    // 2. コンパイラによるオーバーライドチェック
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
        point3 o = r.origin();
        vec3 d = r.direction();
        vec3 oc = center - o;

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
            vec3 outward_normal = (rec.p - center) / radius; 
            rec.set_face_normal(r, outward_normal);
            rec.t = t;
            return true;
        }
            
        return false;
	}

private:
	point3 center;  // 球体中心座標
    double radius;  // 球体半径
};