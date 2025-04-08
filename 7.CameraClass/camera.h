#pragma once

#include "common.h"
#include "hittable_list.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;

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
                // 求出每一个camera到像素中心的射线
				// ピクセル中心へのrayを計算
                point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
                vec3 ray_dir = pixel - cam_pos;
                ray r(cam_pos, ray_dir);

                // rayによる色計算
                color pixel_color = ray_color(r, scene);
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

    // rayの色計算関数
    color ray_color(const ray& r, const hittable_list& scene)
    {
        // 光線と物体の交点における法線を返す
        hit_record rec;
        interval ray_t(0.0, inf);
        if (scene.hit(r, ray_t, rec))
        {
            // 法線は幾何処理段階(hit)で既に計算済み
            // [-1.0, 1.0] ==> [0.0, 1.0]
            return 0.5 * (rec.normal + vec3(1.0, 1.0, 1.0));
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