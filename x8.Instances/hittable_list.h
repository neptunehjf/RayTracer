﻿#pragma once

#include "common.h"
#include "hittable.h"


// hittable_list是一系列hittable物体的集合，但如果把这个集合看作一个整体的话，也应该继承hittable物体的性质
// 物体被抽象成hittable了，没有形状，因此hittable_list不关心也没办法 判定hit
// hittable_list可以看成对于多个物体hit判定的一个框架，具体hit判定交给各几何形状子类
//
// hittable_listは複数のhittableを管理する集合体
// 集合全体としてヒット判定機能を継承するが、具体的な形状判定は個々のオブジェクトに委譲
class hittable_list : public hittable
{
public:
	// hittable的shared_ptr指针的集合
	// 使用shared_ptr可提升内存安全性
	// 
	// hittableのスマートポインタ配列
	// shared_ptr採用によるメモリ管理の安全性向上
	vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void add(shared_ptr<hittable> object) 
	{ 
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	void clear() { objects.clear(); }

	// override hittable
	bool hit(const ray& r, interval& ray_t, hit_record& rec) const override
	{
		// hit_record temp_rec;
		bool is_hit = false;

		// 对于一组hittable对象，只需找到最近的交点
		// 因此用上次循环中找到的最近的t值 
		// 1 通过取最近值的方式，不去染被遮挡的部分
		// 2 动态修改判定范围的最大值,效率比较高
		//
		// 一連のhittableオブジェクトに対して、最も近い交点のみを見つける
		// 前回のループで見つかった最も近いt値を利用して処理する
		// 1. 最近値を採用することで遮蔽領域の計算を省略（オクルージョン除去）
		// 2. 判定範囲の最大値を動的に更新する効率的なアルゴリズム
		for (const auto& object : objects)
		{
			if (object->hit(r, ray_t, rec))
			{
				ray_t.max = rec.t;
				is_hit = true;
			}		
		}

		return is_hit;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	aabb bbox; // デフォルト構築時は空のaabbを生成（初期化不要）
};