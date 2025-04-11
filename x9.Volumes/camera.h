#pragma once

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "dbg.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;
    int sample_num = 10;
    int bounce_limit = 10;

    double vfov = 90.0;                         // 垂直方向の視野角（Field of View）
    point3 look_from = point3(0.0, 0.0, 0.0);   // カメラ視点の起点（位置座標）
    point3 look_at   = point3(0.0, 0.0, -1.0);  // カメラ視点の終点（注視点座標）
    point3 vup       = vec3(0.0, 1.0, 0.0);     // カメラの上方ベクトル（空間上のY軸方向）

    // 参照 referrence/defocus model_1.png
    // 透镜(散焦disk)越大，在里焦距越远的地方成像越模糊
    // 
    // レンズ（デフォーカスディスク）のサイズが大きいほど、焦点距離から離れた領域の像がぼやける
    //
    // 角度，和焦距一起决定了透镜的大小 
    // 散光角度（焦点距離と連動してレンズサイズを決定）
    double defocus_angle = 0.0;   
    // 完美成像的焦距 
    // 最適焦点距離（被写体が完全に鮮明に映る距離）‌
    double focus_dist = 10.0;     

    color  background; // 背景色

    // 光追的渲染是和camera绑定的，因此把render放到camera类里
    // レイトレーシングのレンダリングはカメラと密接に紐付いているため、Cameraクラス内にレンダリングメソッドを実装する
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // 進捗状況を表示
            clog << "\rScanlines remaining: " << (image_height - j) << ' ' 
                 << " Unhit Count: " << unhit_count << " Hit Count: " 
                 << hit_count << flush;

            for (int i = 0; i < image_width; i++)
            {
                //if (i + 1 == 115 && j + 1 == 168) // for debug
                {
                    color pixel_color = vec3(0.0, 0.0, 0.0);

                    for (int n = 0; n < sample_num; n++)
                    {
	                    // 求出每一个camera到像素中心附近的随机采样到的射线
	                    // カメラからピクセル中心周辺へのランダムサンプリング光線を生成
	                    ray r = get_ray(i, j);

	                    // 根据ray算出color，累加起来，最后除以采样数以达到平滑的效果
	                    // 光線追跡結果を累積（後でサンプル数で除算して平滑化）
	                    pixel_color += ray_color(r, scene, 0);

                    }

                    pixel_color /= sample_num;
                    write_color(cout, pixel_color);
                }

            }
        }
        clog << "\rDone.                 \n";
	}

private:
    int image_height = 10;
    point3 pixel_zero;
    vec3 pixel_u;
    vec3 pixel_v;
    point3 cam_pos;
    vec3 u, v, w;
    vec3 defocus_disk_u; // レンズ面の水平基底ベクトル
    vec3 defocus_disk_v; // レンズ面の垂直基底ベクトル

	void initialize()
	{
        image_height = (int)(image_width / aspect_radio); // 近似計算のため0になる可能性あり
        image_height = (image_height < 1) ? 1 : image_height;  // 最小1を保証
      
        cam_pos = look_from;

        double theta = degree_to_radian(vfov);  // tan関数はラジアン値を使用するため角度値ではなく弧度法で計算
        double viewport_h = 2 * focus_dist * tan(theta / 2); // 参照referrence/camera1.png
        double viewport_w = viewport_h * (double(image_width) / image_height); // image_heightは近似値のためアスペクト比を直接使用できません

        // カメラ正規化座標系の決定
        // 参考referrence/camera2.png
        w = unit_vector(look_from - look_at);
        // 因为v始终与vup共面，所以cross(vup, w)和cross(v, w)同向
        // vupとwの外積方向同じ（vupとvが同一平面にあるため）
        u = unit_vector(cross(vup, w)); 
        v = cross(w, u);

        // カメラ座標系におけるviewport UV
        vec3 viewport_u = viewport_w * u;
        vec3 viewport_v = -viewport_h * v; // ビューポートのv方向はカメラ座標系のY軸方向と逆向きになるため

        // ピクセル単位のUVサイズに換算
        pixel_u = viewport_u / image_width;
        pixel_v = viewport_v / image_height;

        // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
        // ビューポート原点の位置を計算（左上隅に配置、焦点距離分離れたXY平面上、カメラ位置はビューポート中心と一致）
        point3 viewport_zero = cam_pos - (w * focus_dist) - (viewport_u / 2) - (viewport_v / 2);
        // 转为像素的中心坐标
        // ピクセル中心座標への調整
        pixel_zero = viewport_zero + 0.5 * (pixel_u + pixel_v);

        // レンズ面のUV ベクトル を計算
        // referrence/defocus model_2.jpg
        double defocus_radius = focus_dist * tan(degree_to_radian(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
	}

    // 获取在以i,j处的像素的中心为中心，xy上的方形区域(-0.5 pixel ,0.5 pixel)内随机采样的光线ray
    // 显然这里随机采样的范围是在同一个像素内，这里可以看出光追和光栅化采样的不同
    // 如果是光栅化，因为光栅化采样的最小单位就是像素，在同一个像素内采样没有任何作用
    // 光追的情形，viewport的一个像素内的位置改变，也会导致光线ray方向改变，所以采样是有效的
    // 总之，光栅化的采样最小单位是像素，光追的采样最小单位是光线ray，像素是有限的而光线ray可以是无限的，高下立判
    //
    // i,j位置のピクセル中心を原点とするXY平面上の方形領域(-0.5ピクセル, 0.5ピクセル)内でランダムサンプリングされた光線を生成
    // 光線追跡とラスタライズのサンプリング差異:
    // ラスタライズはピクセルが最小単位（同一ピクセル内サンプリング無意味）
    // 光線追跡ではピクセル内位置変化が光線方向変化に直結→サンプリング有効
    // 結論: ラスタライズのサンプリング粒度はピクセル、光線追跡は光線単位（幾何学的無限分解能）
    ray get_ray(int i, int j) const
    {
        vec3 offset = offset_square();
        point3 pixel = pixel_zero + ((i + offset.x()) * pixel_u) 
                                  + ((j + offset.y()) * pixel_v);
        point3 ray_origin = (defocus_angle <= 0) ? cam_pos : defocus_disk_sample();
        vec3 ray_dir = pixel - ray_origin;
        double ray_time = random_double();

        return ray(ray_origin, ray_dir, ray_time);
    }

    // XY方向に[-0.5, 0.5]範囲のランダムオフセットを生成（Z方向は0）
    // 方形領域内のランダムサンプリングに使用
    vec3 offset_square() const
    {
        return vec3(-0.5 + random_double(), -0.5 + random_double(), 0.0);
    }

    // rayの色計算関数
    color ray_color(const ray& r, const hittable_list& scene, int bounce_count)
    {
        // 反射回数に制限を加え、無限反射を防ぐ
        if (bounce_count >= bounce_limit)
            return vec3(0.0, 0.0, 0.0);

        // 光線と物体の交点における法線を返す
        hit_record rec;
        // 因为浮点数精度的原因，交点可能不会正好在物体表面，交点有可能在物体外部也有可能在内部
        // 如果交点在物体内部的话，以tmin为0进行计算就不对了，要把tmin修正为稍大的值
        // 以上修正解决了shadow acne的问题
        // 
        // 浮動小数点精度の問題により、交点が物体表面からわずかにずれる可能性がある
        // 交点が物体内部にある場合、tminを0に設定すると計算誤差が生じるため
        // tminをわずかに大きな値に修正することでシャドウアクネ問題を解決
        interval ray_t(0.001, inf);
        if (scene.hit(r, ray_t, rec))
        {
            ray ray_out;
            color attenuation;

            // 以下分支，光源材质和弹射材质是互斥的，因此逻辑简单清晰。
            // 以后有可能会同时支持两种特性的材质，暂时先放着吧
            // 可以看到光追优于光栅化的地方。光追的光源衰减，是通过弹射自然算出的
            // 光栅化无法自然做到，只能额外用公式来模拟光源衰减
            // 如果当前交点有弹射，说明是非光源材质，非光源材质只需返回弹射颜色
			//
			// 分岐処理：発光体材質と反射材質は排他的なためロジックが簡潔
		    // （将来的に両特性を併用可能にする拡張予定あり、暫定的に現状維持）
		    // レイトレーシングの優位性：光の減衰が自然計算される
		    // ラスタライズでは数式による模擬計算が必要
		    // 散乱が発生する場合（非発光体材質）：反射色を再帰計算
            if (rec.mat->scatter(r, rec, attenuation, ray_out))
            {
                total_bounce++;
                return attenuation * ray_color(ray_out, scene, ++bounce_count);
            }
			// 発光体材質の場合：直接発光色を返す
            else
                return rec.mat->emit(rec.u, rec.v, rec.p);
        }

        // 衝突なし：背景色を返す
        return background;
        // 背景色の処理
        // rayのy成分による線形補間
        //vec3 r_uint = unit_vector(r.direction());
        //// [-1.0, 1.0] ==> [0.0, 1.0]
        //double a = 0.5 * (r_uint.y() + 1.0);

        //// 青と白のグラデーション
        //return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        
    }

    point3 defocus_disk_sample() const 
	{
        // カメラのデフォーカスディスク内でランダムな点を生成する‌
        vec3 p = random_on_unit_disk();
        return cam_pos + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

