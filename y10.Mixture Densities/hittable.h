#pragma once

#include "common.h"
#include "aabb.h"

// 因为material.h已经包含了hittable.h，为了防止互相包含，这里用只声明的方式
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
	bool front_face = true; 

	// 根据ray和外部法线的方向判断当前光线是从内部还是外部来的，以及求实际的normal
	void set_face_normal(const ray& r, vec3 outward_normal)
	{
		// 要确保传入的outward_normal是单位向量

		if (dot(r.direction(), outward_normal) > 0)
		{
			// 如果ray方向和外部法线的方向一致，ray来自物体内部
			front_face = false;
			normal = -outward_normal;
		}
		else
		{
			// 如果ray方向和外部法线的方向相反，ray来自物体外部
			front_face = true;
			normal = outward_normal;
		}
	}
};

// 抽象类，具体放到子类实现
class hittable
{
public:
	virtual ~hittable() = default;

	// 纯虚函数，具体放到子类实现
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

	// 返回物体的包围盒
	virtual aabb bounding_box() const = 0;

	// 返回以origin为起点到当前物体的随机向量
	virtual vec3 random(const point3& origin) const
	{
		return vec3(1, 0, 0);
	}

	// 返回random生成的随机向量的pdf值
	virtual double pdf_value(const point3& origin, const vec3& direction) const 
	{
		return 0.0;
	}


};

// 平移类，用于物体的平移
// 1 对实际物体的的hit判断封装了一层，用光线(camera)反向位移的方式来对位移后的物体做hit判断
// 用光线(camera)位移的方式，可以避免对复杂形状的实际物体进行位移操作
// 2 但是对于实际物体的包围盒，因为包围盒形状简单，直接对包围盒做位移操作，这样不需要在每次hit都位移，只需初始化物体的时候位移。效率更高
// 综上，对于实际物体，不位移实际物体，用光反向位移进行hit判断；对于包围盒，位移包围盒，用本来的光线进行hit判断
class translate : public hittable
{
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset)
	{
		// 构造新包围盒
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 物体移动了offset ,相当于camera(光线)移动-offset
		ray offset_r(r.origin() - offset, r.direction(), r.time());

		// 用移动后的光线与未移动的物体进行hit判断
		if (object->hit(offset_r, ray_t, rec))
		{
			// 如果hit了，实际上是在rec.p + offset处hit的
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
class rotate_y : public hittable
{
public:
	rotate_y(shared_ptr<hittable> object, double angle) : object(object)
	{
		double radian = degree_to_radian(angle);
		sin_theta = sin(radian);
		cos_theta = cos(radian);
		bbox = object->bounding_box();

		// 默认包围盒是empty
		point3 min(inf, inf, inf);
		point3 max(-inf, -inf, -inf);

		// 对于包围盒的8个顶点，每个分别旋转theta，然后每次旋转都会更新min max的值，最后再用min max构建一个新的包围盒
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					// 选择顶点
					double x = (1 - i) * bbox.x.min + i * bbox.x.max;
					double y = (1 - j) * bbox.y.min + j * bbox.y.max;
					double z = (1 - k) * bbox.z.min + k * bbox.z.max;

					// 旋转顶点
					double x_rotate = x * cos_theta + z * sin_theta;
					double z_rotate = x * (-sin_theta) + z * cos_theta;

					// 更新min,max的值
					vec3 temp(x_rotate, y, z_rotate);
					for (int i = 0; i < 3; i++)
					{
						min[i] = fmin(min[i], temp[i]);
						max[i] = fmin(max[i], temp[i]);
					}
				}

		// 构造新包围盒
		bbox = aabb(min, max);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 物体旋转了theta ,相当于camera(光线)旋转-theta
		// 结合旋转公式(参照referrence/rotation formula.jpg) 可得如下结果
		auto origin = point3(r.origin().x() * cos_theta - r.origin().z() * sin_theta, 
			                 r.origin().y(), 
			                 r.origin().x() * sin_theta + r.origin().z() * cos_theta);

		auto direction = vec3(r.direction().x() * cos_theta - r.direction().z() * sin_theta,
					          r.direction().y(),
			                  r.direction().x() * sin_theta + r.direction().z() * cos_theta);
		
		ray rotate_r(origin, direction, r.time());

		// 用旋转后的光线与未旋转的物体进行hit判断
		if (object->hit(rotate_r, ray_t, rec))
		{
			// 如果hit了，实际上是在rec.p 旋转了+theta处hit的
			rec.p = point3(rec.p.x() * cos_theta + rec.p.z() * sin_theta,
			               rec.p.y(),
						   rec.p.x() * (-sin_theta) + rec.p.z() * cos_theta);

			// 旋转的话，法线也应该还原成+theta的
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