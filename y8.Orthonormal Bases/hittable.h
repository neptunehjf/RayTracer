#pragma once

#include "common.h"
#include "aabb.h"

// 因为material.h已经包含了hittable.h，为了防止互相包含，这里用只声明的方式
// 
// material.hは既にhittable.hをインクルードしているため、相互インクルードを防ぐために前方宣言を使用
class material;

class hit_record
{
public:
	point3 p;
	vec3 normal;
	double t = 0.0;
	shared_ptr<material> mat;
	double u;
	double v;

	// 判断ray是从物体外部还是内部打过来的
	// 当前情况下，在几何阶段做判断结果并存储可能是一种比较高效的选择
	//
	// rayが物体の外部/内部どちらから衝突したかを示すフラグ
	// 幾何計算段階で判定結果を保持することは効率的な設計選択
	bool front_face = true; 

	// 根据ray和外部法线的方向判断当前光线是从内部还是外部来的，以及求实际的normal
	// ray方向と外向き法線から衝突面の向きを判定し法線方向を設定
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// 要确保传入的outward_normal是单位向量
		// 外向き法線は単位ベクトルである必要あり

		if (dot(r.direction(), outward_normal) > 0)
		{
			// 如果ray方向和外部法线的方向一致，ray来自物体内部
			// レイ方向と外向き法線が同方向 → 物体内部からの衝突
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// 如果ray方向和外部法线的方向相反，ray来自物体外部
			// レイ方向と外向き法線が逆方向 → 物体外部からの衝突
			front_face = true;
			normal = outward_normal;
		}
	}
};

// 抽象类，具体放到子类实现
// 抽象基底クラス（実装はサブクラスで行う）
class hittable
{
public:
	virtual ~hittable() = default;

	// 纯虚函数，具体放到子类实现
	// 純粋仮想関数（サブクラスで実装必須）
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	// オブジェクトのバウンディングボックスを返す
	virtual aabb bounding_box() const = 0;
};

// 平移类，用于物体的平移
// 1 对实际物体的的hit判断封装了一层，用光线(camera)反向位移的方式来对位移后的物体做hit判断
// 用光线(camera)位移的方式，可以避免对复杂形状的实际物体进行位移操作
// 2 但是对于实际物体的包围盒，因为包围盒形状简单，直接对包围盒做位移操作，这样不需要在每次hit都位移，只需初始化物体的时候位移。效率更高
// 综上，对于实际物体，不位移实际物体，用光反向位移进行hit判断；对于包围盒，位移包围盒，用本来的光线进行hit判断
//
// 平行移動クラス。物体の平行移動を扱う
// 1 実際の物体に対するヒット判定を1層カプセル化し、カメラ（光線）を逆方向に移動させることで 移動後の物体に対するヒット判定を実現する
// 光線を移動させる方法により、複雑な形状の実際の物体を移動させる操作を回避できる
// 2 ただしバウンディングボックスについては、形状が単純なため直接ボックスを移動させる これにより毎回のヒット判定で移動処理を行う必要がなく、物体初期化時に1回移動するだけでよいため効率的
// 要約:
// - 実際の物体: 物体を移動させず、光線を逆移動させてヒット判定
// - バウンディングボックス: ボックスを直接移動させ、元の光線でヒット判定
class translate : public hittable
{
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset)
	{
		// 新しいバウンディングボックスを構築
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 物体がoffset分移動した場合、カメラ（光線）を-offset分移動させるのと等価
		ray offset_r(r.origin() - offset, r.direction(), r.time());

		// 移動後の光線と元の物体でヒット判定
		if (object->hit(offset_r, ray_t, rec))
		{
			// 如果hit了，实际上是在rec.p + offset处hit的
			// 
			// ヒット位置は実際にはoffset分ずれた位置
			rec.p += offset;
			return true;
		}
		else
			return false;

	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};

// 旋转类，用于物体沿y轴旋转
// 实现思路和上面的平移类是一致的
//
// Y軸回転クラス。物体のY軸周り回転を扱う
// 実装方針は前述の平行移動クラスと同様
class rotate_y : public hittable
{
public:
	rotate_y(shared_ptr<hittable> object, double angle) : object(object)
	{
		double radian = degree_to_radian(angle);
		sin_theta = sin(radian);
		cos_theta = cos(radian);
		bbox = object->bounding_box();

		// デフォルトのバウンディングボックスは空
		point3 min(inf, inf, inf);
		point3 max(-inf, -inf, -inf);

		// 对于包围盒的8个顶点，每个分别旋转theta，然后每次旋转都会更新min max的值，最后再用min max构建一个新的包围盒
		//
		// バウンディングボックスの8頂点に対し、各頂点をtheta分回転させた後 min/max値を更新し、新しいバウンディングボックスを構築
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					// 頂点の選択
					double x = (1 - i) * bbox.x.min + i * bbox.x.max;
					double y = (1 - j) * bbox.y.min + j * bbox.y.max;
					double z = (1 - k) * bbox.z.min + k * bbox.z.max;

					// 頂点の回転計算
					double x_rotate = x * cos_theta + z * sin_theta;
					double z_rotate = x * (-sin_theta) + z * cos_theta;

					// min,max値の更新
					vec3 temp(x_rotate, y, z_rotate);
					for (int i = 0; i < 3; i++)
					{
						min[i] = fmin(min[i], temp[i]);
						max[i] = fmin(max[i], temp[i]);
					}
				}

		// 新しいバウンディングボックスを構築
		bbox = aabb(min, max);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 物体がtheta分回転した場合、カメラ（光線）を-theta分回転させるのと等価
	    // 回転公式参照（参照 referrence/rotation formula.jpg）
		auto origin = point3(r.origin().x() * cos_theta - r.origin().z() * sin_theta, 
			                 r.origin().y(), 
			                 r.origin().x() * sin_theta + r.origin().z() * cos_theta);

		auto direction = vec3(r.direction().x() * cos_theta - r.direction().z() * sin_theta,
					          r.direction().y(),
			                  r.direction().x() * sin_theta + r.direction().z() * cos_theta);
		
		ray rotate_r(origin, direction, r.time());

		// 回転後の光線と元の物体でヒット判定
		if (object->hit(rotate_r, ray_t, rec))
		{
			// 実際のヒット位置は+theta分回転した位置
			rec.p = point3(rec.p.x() * cos_theta + rec.p.z() * sin_theta,
			               rec.p.y(),
						   rec.p.x() * (-sin_theta) + rec.p.z() * cos_theta);

			// 法線も+theta分回転を適用する必要がある
			rec.normal = point3(rec.normal.x() * cos_theta + rec.normal.z() * sin_theta,
				                rec.normal.y(),
				                rec.normal.x() * (-sin_theta) + rec.normal.z() * cos_theta);

			return true;
		}
		else
			return false;

	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	shared_ptr<hittable> object;
	double sin_theta;
	double cos_theta;
	aabb bbox;
};