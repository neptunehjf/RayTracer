#pragma once

#include "hittable_list.h"
#include <algorithm>
#include "dbg.h"

// Bounding Volume Hierarchies 包围盒层级结构
// 二叉树结构，每个包围盒看作一个节点，查询函数是hit，所以也要继承hittable
// 查询效率是O(log n)，远远优于线性查询的O(n)
// 具体参考referrence/bvh.jpg
//
// バウンディングボリューム階層構造（BVH）
// 二分木構造で各ノードがAABBを保持、hit関数で効率的な交差判定を実現
// hittableクラスを継承し、O(log n)の問合せ効率を実現（線形検索O(n)より大幅改善）
// 参照: referrence/bvh.jpg
class bvh_node : public hittable
{
public:
	// 直接传入拷贝是为了方便修改hittable_list的值又不影响原来的值
	// 
	// hittable_list値渡し（元データへの影響防止）
	bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

	bvh_node(vector<shared_ptr<hittable>>& objects, size_t start, size_t end)
	{
		bbox = aabb::empty;

		// 递归前算出总包围盒，再求出最长轴，用最长轴来划分物体。
		// 对于width 800 sample 100 的场景，用最长轴划分法，耗时42秒，用随机轴划分法耗时45秒。而且可以看出最长轴划分法的unhit和hit次数都小于随机轴划分法，可见划分更合理。
		/* 再帰前処理：
		   1. 全オブジェクトの統合AABB計算
		   2. 最長軸判定（パフォーマンス比較：
			  最長軸分割法 42秒 vs ランダム軸分割法 45秒
			  unhit/hit回数も最長軸法が優位） */
		for (size_t i = start; i < end; i++)
		{
			bbox = aabb(bbox, objects[i]->bounding_box());
		}

		// 因为最长轴上的物体分布更密集，更适合作为划分轴。比随机选取轴要更有效率
		//
		// 最長軸選択（密度の高い分布軸で分割効率向上）
		size_t axis = bbox.get_longest_axis();

		auto comparator = (axis == 0) ? box_compare_x :
					      (axis == 1) ? box_compare_y : box_compare_z;

		size_t objects_num = end - start;

		// 如果只有一个，也复制一个多余的，防止空节点
		if (objects_num == 1)
			left = right = objects[start];
		else if(objects_num == 2)
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			sort(objects.begin() + start, objects.begin() + end, comparator);
			size_t mid = start + objects_num / 2;
			left = make_shared<bvh_node>(objects, start, mid);
			right = make_shared<bvh_node>(objects, mid, end);
		}

		// 因为要实现最长轴划分优化算法，总包围盒移到递归前计算
		// 
		// 統合AABB計算を再帰前に移動
		// bbox = aabb(left->bounding_box(), right->bounding_box());

	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// 如果未击中本节点，直接返回false
		//
		// バウンディングボックスとの交差判定
		if (!bbox.hit(r, ray_t))
		{
			unhit_count++;
			return false;
		}
		hit_count++;
			
		// 如果击中了本节点，继续遍历左右子节点
		// ノードにヒットした場合、子ノードを再帰的に探索
		bool left_hit = left->hit(r, ray_t, rec);
		// 如果已经击中了左节点，则已经查询到大致范围在左节点，因此更新ray的t_max为rec.t，减少不必要的查询范围
		/* 左子ノードのヒット結果に基づく探索範囲最適化：
		   左でヒットした場合、右子ノードの探索範囲をrec.tまで縮小
		   これにより不要な交差判定を削減*/
		interval iv = interval(ray_t.min, left_hit ? rec.t : ray_t.max);
		bool right_hit = right->hit(r, iv, rec);

		return left_hit || right_hit;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	aabb bbox;
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;

	static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis)
	{
		interval a_interval_by_axis = a->bounding_box().get_interval_by_axis(axis);
		interval b_interval_by_axis = b->bounding_box().get_interval_by_axis(axis);
		return a_interval_by_axis.min < b_interval_by_axis.min;
	}

	static bool box_compare_x(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 0);
	}

	static bool box_compare_y(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 1);
	}

	static bool box_compare_z(const shared_ptr<hittable> a, const shared_ptr<hittable> b)
	{
		return box_compare(a, b, 2);
	}
};