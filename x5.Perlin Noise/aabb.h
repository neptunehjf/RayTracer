#pragma once

#include "common.h"

// Axis-Aligned Bounding Box
// 参照 referrence/aabb.jpg
class aabb
{
public:
	// aabb的3对slab，与对应的坐标轴对齐
	// 
	// 3軸に整列したスラブペア（AABBの境界面）
	interval x, y, z;

	// 默认构造 空aabb  因为interval默认是空(inf, -inf)
	//
	// デフォルトコンストラクタ：空のAABBを生成（intervalのデフォルトは[+∞,-∞]）
	aabb() {} 
	
	// 用interval直接构造aabb
	// 
	// interval指定によるAABB構築
	aabb(const interval& x, const interval& y, const interval& z) :
		x(x), y(y), z(z) {}

	// 用两个点间接构造aabb，入参只需提供数据，不需要排序
	//
	// 2点からAABBを構築（座標値の大小自動判定）
	aabb(const point3& a, const point3& b)
	{
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
	}

	// 用2个包围盒构造1个包围盒
	//
	// 2つのAABBを包含する新たなAABBを構築
	aabb(const aabb& bbox1, const aabb& bbox2)
	{
		x = interval(bbox1.x, bbox2.x);
		y = interval(bbox1.y, bbox2.y);
		z = interval(bbox1.z, bbox2.z);
	}

	const interval& get_interval_by_axis(int n) const
	{
		// x0 y1 z2
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	//  调用次数多(循环次数多 + 递归)，需要重点优化
	//  对于width 400 sample 100的渲染场景，实装后渲染时间由77秒优化到36秒，优化效果不是很明显，进一步研究发现，是因为在频繁调用的hit函数内部申请了两个变量来接收ray_t.min和ray_t.max的原因。用以下方式优化后的时间缩短为11秒。
	//	优化：
	//	1 hit内部不申请变量，而是把入参ray_t改为值传递，并直接修改ray_t的值，这是最重要的优化点
	//	2 提前计算好倒数，后续乘倒数来实现除法
	//	3 能加const修饰尽量加const
	//
	// 呼び出し回数が多い（多重ループ + 再帰）ため最適化が重要
	// width400 sample100のレンダリングシーンで、実装後のレンダリング時間が77秒→36秒に改善したが、更なる解析の結果、
	// hit関数内でray_t.min/maxの値受け取りにローカル変数を2つ使用していたことがボトルネックと判明
	// 下記最適化により11秒まで短縮
	// 最適化内容：
	// 1. hit関数内部で変数を宣言せず、引数ray_tを値渡しで直接変更（最も効果的な改善）
	// 2. 除算を乗算に置換するため逆数を事前計算
	// 3. const修飾を可能な限り追加

	bool hit(const ray& r, interval ray_t) const
	{
		const point3& o = r.origin();
		const vec3& d = r.direction();

		// 一个向量是否与aabb相交 分解为 一个向量在x,y,z坐标轴上的分量是否都分别与对应的slab相交
		//
        // ベクトルとAABBの交差判定を3軸に分解して処理
		for (int axis = 0; axis < 3; axis++)
		{
			const interval& ax = get_interval_by_axis(axis);

			// axis direction inverse，提前计算倒数以提高性能
			// 加const修饰符以提高性能
			//
			// 軸方向の逆数を事前計算（パフォーマンス向上のため）
			// const修飾子で最適化
			const double adinv = 1 / d[axis];

			// スラブとの交差に対応するt値計算
			double t0 = (ax.min - o[axis]) * adinv;
			double t1 = (ax.max - o[axis]) * adinv;

			// 光线沿坐标轴负方向传播时，t0和t1的大小是反的
			//
			// 座標軸負方向へのレイ伝播時にt値の大小関係が逆転
			if (t0 > t1)
				swap(t0, t1);

			// slab与ray的t求交集
			
			/* 最適化前（36秒）：
			   新規メモリ割当：t_min/t_max変数宣言
			   const double t_min = max(t0, ray_t.min);
			   const double t_max = min(t1, ray_t.max); */

			// 最適化後（11秒）：
			// 仮引数ray_tのメモリ領域を直接利用（値の外部伝播不可がトレードオフ）
			ray_t.min = max(t0, ray_t.min);
			ray_t.max = min(t1, ray_t.max);

			// 交差区間が存在しない場合即時リターン
			if (ray_t.min >= ray_t.max)
				return false;

		}
		return true;
	}

	int get_longest_axis() const
	{
		// x0 y1 z2
		if (x.size() > y.size() && x.size() > z.size()) return 0;
			
		if (y.size() > x.size() && y.size() > z.size()) return 1;

		if (z.size() > x.size() && z.size() > y.size()) return 2;
	}

	// 声明和类相同类型的静态变量是合法的，不会导致递归定义
	// クラス自身と同じ型の静的メンバ変数の宣言は有効であり、再帰的定義を引き起こさない
	static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);
