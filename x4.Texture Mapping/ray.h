#pragma once

#include "common.h"

class ray 
{
public:
	ray() {}
	ray(const point3& origin, const vec3 direction) : o(origin), d(direction), tm(0.0) {}
	ray(const point3& origin, const vec3 direction, double time) : o(origin), d(direction), tm(time) {}

	//因为return的是引用，所以返回值也要加const才能保证不被修改
    // 原点の参照をconstで取得（参照返却のため変更防止）
	const point3& origin() const { return o; }

	const vec3& direction() const { return d; }

	// 获得ray在时间t的位置
	// 時刻tにおけるrayの位置座標を計算
	point3 at(double t) const
	{
		return o + t * d;
	}

	// 获取当前时刻
	double get_time() const
	{
		return tm;
	}

private:
	point3 o; // 始点

	// d的大小相当于速度(宏观上与物体相对位置有关，并不是微观的光线的速度，光线的速度可认为是无穷大)
	//
	// dの大きさは速度に相当（巨視的には物体との相対位置に関連し、微視的な光線の速度ではない。光線速度は無限大と見做せる）
	vec3 d;   

	// camera发出射线的时刻，用于模拟不同时刻运动物体的位置变化
	// 只是为了表现物体的运动，是宏观世界的时间，要远远大于微观光线传播的时间，无论光线反弹多少次，都可以认为是固定的
	// 
	// カメラが発射する光線の時刻。運動物体の位置変化を時間的にシミュレートするため
	// 物体の運動表現専用の巨視的世界の時間。微視的光線伝播時間より十分大きいため、
	// 光線が何度反射しても時刻は固定されていると見做せる
	double tm = 0.0; 
};