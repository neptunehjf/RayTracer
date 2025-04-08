#pragma once

#include "interval.h"

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

	// 这里最大值0.999是为了避免结果256导致的溢出(单字节颜色)
	// 最大値0.999は256値によるオーバーフロー防止のため（1バイトカラー）
	const interval range(0.000, 0.999); 
	// [0.0, 1.0] ==> [0, 255]
	int ir = int(256 * range.clamp(r));
	int ig = int(256 * range.clamp(g));
	int ib = int(256 * range.clamp(b));

	// 出力フォーマット（output.ppmへのリダイレクト）
	out << ir << ' ' << ig << ' ' << ib << '\n';
}