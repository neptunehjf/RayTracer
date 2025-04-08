#include "common.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable_list& scene);
double hit_sphere(const point3& center, double radius, const ray& r);

int main() {

    // 画像のアスペクト比16:9を固定
    double aspect_radio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) (image_width / aspect_radio); // 近似計算のため0になる可能性あり
    image_height = (image_height < 1) ? 1 : image_height;  // 最小1を保証

    // Scene
    hittable_list scene;

    // 注意这里point3(0.0, 0.0, -1.0)是右值
    // 所以sphere构造函数的入参要么是const point3&，要么是point&&，否则编译报错
    //
    // 1. ここで使用されているpoint3(0.0, 0.0, -1.0)は右辺値（rvalue）です
    // 2. したがってsphereコンストラクタの引数は、const point3& または point3&& で宣言する必要があります
    //    （これら以外の形式では右辺値のバインドが不可能となり、コンパイルエラーが発生します）
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

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
            color pixel_color = ray_color(r, scene);
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n";
}

// rayの色計算関数
color ray_color(const ray& r, const hittable_list& scene)
{
    // 球
    point3 center(0.0, 0.0, -1.0);
    double radius = 0.5;

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
    return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);;
}

// 光線と球体の最も近い交点に対応するt値を算出
double hit_sphere(const point3& center, double radius, const ray& r)
{
    point3 o = r.origin();
    vec3 d = r.direction();
    vec3 oc = center - o;

    // 转化为二项式求解未知数t的问题，推导过程参考referrence/sphere_hit.jpg
    // 二次方程式の解としてtを求めることになる（導出過程はreferrence/sphere_hit.jpg参照）
    double a = d.length_squared();
    double h = dot(d, oc);
    double c = oc.length_squared() - radius * radius;

    double discriminant = h * h - a * c;

    /* 判別式の条件:
       = 0 : レイと球が1点で接する
       > 0 : 2点で交差する
       < 0 : 交差点なし */
    if (discriminant >= 0)
        return (h - sqrt(discriminant)) / a; //　tを求める

    // 交点が存在しない場合は負の値を返す
    return -1.0;
}