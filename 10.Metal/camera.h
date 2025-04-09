#pragma once

#include "common.h"
#include "hittable_list.h"
#include "material.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;
    int sample_num = 10;
    int bounce_limit = 10;

    // 光追的渲染是和camera绑定的，因此把render放到camera类里
    // レイトレーシングのレンダリングはカメラと密接に紐付いているため、Cameraクラス内にレンダリングメソッドを実装する
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // 進捗状況を表示
            clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

            for (int i = 0; i < image_width; i++)
            {
                color pixel_color;

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
        clog << "\rDone.                 \n";
	}

private:
    int image_height;
    point3 pixel_zero;
    double pixel_x;
    double pixel_y;
    point3 cam_pos;

	void initialize()
	{
        image_height = (int)(image_width / aspect_radio); // 近似計算のため0になる可能性あり
        image_height = (image_height < 1) ? 1 : image_height;  // 最小1を保証
        cam_pos = point3(0.0, 0.0, 0.0);

        double focal_length = 1.0; // 焦点距離（cameraとviewport間の距離）
        double viewport_h = 2.0;
        double viewport_w = viewport_h * (double(image_width) / image_height); // 近似値のためアスペクト比を直接使用不可

       // カメラ座標系におけるviewport UV
        vec3 u = vec3(viewport_w, 0.0, 0.0);
        vec3 v = vec3(0.0, -viewport_h, 0.0); // 因为viewport的所在的v方向和camera坐标系y方向相反

       // ピクセル単位のUVサイズに換算
        pixel_x = viewport_w / image_width;
        pixel_y = -viewport_h / image_height;

       // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
       // ビューポート原点の位置を計算（左上隅に配置、焦点距離分離れたXY平面上、カメラ位置はビューポート中心と一致）
        point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;
		
       // 转为像素的中心坐标
       // ピクセル中心座標への調整
        pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);
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
        point3 pixel = pixel_zero + vec3((i + offset.x()) * pixel_x, (j + offset.y()) * pixel_y, 0.0);
        vec3 ray_dir = pixel - cam_pos;
        return ray(cam_pos, ray_dir);
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

            if (rec.mat->scatter(r, rec, attenuation, ray_out))
                return attenuation * ray_color(ray_out, scene, ++bounce_count);

            return color(0.0, 0.0, 0.0);
        }

        // 背景色の処理
        // rayのy成分による線形補間
        vec3 r_uint = unit_vector(r.direction());
        // [-1.0, 1.0] ==> [0.0, 1.0]
        double a = 0.5 * (r_uint.y() + 1.0);

       // 青と白のグラデーション
        return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};