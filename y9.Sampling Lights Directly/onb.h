#pragma once

#include "common.h"

class onb
{
public:
	onb(const vec3& n)
	{
		axis[2] = unit_vector(n);
		vec3 temp;
		// 生成一个临时的不与n平行的向量z
		// n(w軸)と非平行な一時ベクトルを生成
		if (fabs(axis[2].x()) > 0.9)
			temp = vec3(0, 1, 0);
		else
			temp = vec3(1, 0, 0);
		// cross算出的x y轴的分量不一定能对应上，不过没影响，因为随机方向只和z轴夹角相关
		// 根据临时向量与n算出与n垂直的向量v
		//
		// クロス積で算出されるx/y軸成分は正確に対応しない可能性があるが、
        // ランダム方向はz軸(w軸)との角度のみに依存するため影響なし
        // 一時ベクトルとnからnに直交するベクトルvを算出
		axis[1] = unit_vector(cross(axis[2], temp));
		// u = w x v
		axis[0] = cross(axis[2], axis[1]);
	}

	const vec3& u() const { return axis[0]; }
	const vec3& v() const { return axis[1]; }
	const vec3& w() const { return axis[2]; }

    // 座標空間変換
    // 注意: ここで正規化処理しない
	vec3 transform(const vec3& v) const 
	{
		return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
	}

private:
	vec3 axis[3];
};