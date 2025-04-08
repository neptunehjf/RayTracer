#include "vec3.h"
#include "color.h"
#include "ray.h"

color ray_color(const ray& r);

int main() {

    // 画像のアスペクト比16:9を固定
    double aspect_radio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) (image_width / aspect_radio); // 近似計算のため0になる可能性あり
    image_height = (image_height < 1) ? 1 : image_height;  // 最小1を保証

    // Camera
    vec3 cam_pos = { 0.0, 0.0, 0.0 };
    double focal_length = 1.0; // 焦点距離（cameraとviewport間の距離）
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double(image_width) / image_height);  // 近似値のためアスペクト比を直接使用不可

    // カメラ座標系におけるviewport UV
    vec3 u = vec3(viewport_w, 0.0, 0.0);
    vec3 v = vec3(0.0, -viewport_h, 0.0); // viewportのv方向はカメラ座標系のy軸と逆方向

    // ピクセル単位のUVサイズに換算
    double pixel_x = viewport_w / image_width;
    double pixel_y = -viewport_h / image_height;

    // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
    // ビューポート原点の位置を計算（左上隅に配置、焦点距離分離れたXY平面上、カメラ位置はビューポート中心と一致）
    point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;

    // 转为像素的中心坐标
    // ピクセル中心座標への調整
    point3 pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 256x256の画像を生成（R値は水平方向に増加、G値は垂直方向に増加、B値は常に0）
    for (int j = 0; j < image_height; j++) // G
    {
        // 進捗状況を表示
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            // 求出每一个camera到像素中心的射线
            // ピクセル中心へのrayを計算
            point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
            vec3 ray_dir = pixel - cam_pos;
            ray r(cam_pos, ray_dir);

            // rayによる色計算
            color pixel_color = ray_color(r);
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n";
}

// rayの色計算関数
color ray_color(const ray& r)
{
    // y成分による線形補間
    vec3 r_uint = unit_vector(r.direction());
    // [-1.0, 1.0] ==> [0.0, 1.0]
    double a = 0.5 * (r_uint.y() + 1.0);

    // 青と白のグラデーション
    color c = (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);

    return c;
}