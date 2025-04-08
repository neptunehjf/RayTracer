#pragma once

#include <iostream>
#include "vec3.h"

// 给vec3起一个别名，主要是为了把几何与颜色区分开
// vec3にカラータイプの別名を定義（幾何計算用と色表現用の型を区別するため）
using color = vec3;

// 写一个颜色值到out
// カラー値を出力ストリームに書き込む
void write_color(ostream& out, const color& c)
{
	double r = c.x();
	double g = c.y();
	double b = c.z();

	// [0.0, 1.0] ==> [0, 255]
	int ir = int(255.999 * r);
	int ig = int(255.999 * g);
	int ib = int(255.999 * b);

	// 出力フォーマット（output.ppmへのリダイレクト）
	out << ir << ' ' << ig << ' ' << ib << '\n';
}