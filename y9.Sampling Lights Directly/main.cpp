#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "volume.h"
#include <iomanip>

void scene_bouncing_spheres() 
{
    // Scene
    hittable_list scene;

    auto checker_tex = make_shared<checker_texture>(2.0, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<diffuse>(checker_tex);
    scene.add(make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000.0, ground_material));

    for (int a = -11; a < 11; a++) 
    {
        for (int b = -11; b < 11; b++) 
        {
            double choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            // 大きな球体に近接しすぎている小さな球体を除去
            if ((center - point3(0.0, 0.2, 0.0)).length() > 1.0 &&
                (center - point3(-4.0, 0.2, 0.0)).length() > 1.0 &&
                (center - point3(4.0, 0.2, 0.0)).length() > 1.0)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    color albedo = color::random() * color::random();
                    sphere_material = make_shared<diffuse>(albedo);

                    // center2の座標値が物体の運動速度を反映している
                    point3 center2 = center + vec3(0, random_double(0, 0.5), 0);
                    scene.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    color albedo = color::random(0.5, 1.0);
                    double fuzz = random_double(0.0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else 
                {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    scene.add(make_shared<sphere>(point3(0.0, 1.0, 0.0), 1.0, material1));

    auto material2 = make_shared<diffuse>(color(0.4, 0.2, 0.1));
    scene.add(make_shared<sphere>(point3(-4.0, 1.0, 0.0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<sphere>(point3(4.0, 1.0, 0.0), 1.0, material3));

    scene = hittable_list(make_shared<bvh_node>(scene));

    // Camera
    camera cam;
    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 20.0;
    cam.look_from = point3(13.0, 2.0, 3.0);
    cam.look_at = point3(0.0, 0.0, 0.0);
    cam.vup = vec3(0.0, 1.0, 0.0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.background = color(0.70, 0.80, 1.00);

    // Render
    cam.render(scene);
}

void scene_checkered_spheres() 
{
    // scene
    hittable_list scene;

    auto checker_tex = make_shared<checker_texture>(2.0, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    scene.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<diffuse>(checker_tex)));
    scene.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<diffuse>(checker_tex)));

    //オブジェクト数が極少の場合AABB bboxアルゴリズムを使って逆に効率が低下
    //scene = hittable_list(make_shared<bvh_node>(scene));

    // Camera
    camera cam;

    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 20;
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0.70, 0.80, 1.00);

    // render
    cam.render(scene);
}

void scene_earth()
{
    // Scene
    hittable_list scene;

    auto earth_tex = make_shared<image_texture>("earthmap.jpg");
    auto earth_material = make_shared<diffuse>(earth_tex);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_material);

    scene.add(globe);

    // Camera
    camera cam;

    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 20;
    cam.look_from = point3(0, 0, 12);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0.70, 0.80, 1.00);

    // Render
    cam.render(scene);
}

void scene_perlin_spheres() 
{
    // Scene
    hittable_list scene;

    auto perlin_tex = make_shared<noise_texture>(4);
    scene.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<diffuse>(perlin_tex)));
    scene.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<diffuse>(perlin_tex)));

    // Camera
    camera cam;

    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 20;
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0.70, 0.80, 1.00);

    // Render
    cam.render(scene);
}

void scene_quads() 
{
    // Scene
    hittable_list scene;

    // Materials
    auto left_red = make_shared<diffuse>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<diffuse>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<diffuse>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<diffuse>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<diffuse>(color(0.2, 0.8, 0.8));

    // Quads
    scene.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    scene.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    scene.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    scene.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    scene.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_radio = 1.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 80;
    cam.look_from = point3(0, 0, 9);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0.70, 0.80, 1.00);

    cam.render(scene);
}

void scene_simple_light() 
{
    hittable_list scene;

    auto pertext = make_shared<noise_texture>(4);
    scene.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<diffuse>(pertext)));
    scene.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<diffuse>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    scene.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));

    camera cam;

    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 20;
    cam.look_from = point3(26, 3, 6);
    cam.look_at = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(scene);
}

void scene_cornell_box() 
{
    hittable_list scene;

    auto red = make_shared<diffuse>(color(.65, .05, .05));
    auto white = make_shared<diffuse>(color(.73, .73, .73));
    auto green = make_shared<diffuse>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    scene.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    scene.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    scene.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    scene.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    scene.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    scene.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    scene.add(box1);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    scene.add(box2);

    camera cam;

    cam.aspect_radio = 1.0;
    cam.image_width = 600;
    cam.sample_num = 10;
    cam.bounce_limit = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(278, 278, -800);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    clog << "sample per pixel: " << cam.sample_num << endl;
    cam.render(scene);
}

void scene_cornell_smoke() {
    hittable_list scene;

    auto red = make_shared<diffuse>(color(.65, .05, .05));
    auto white = make_shared<diffuse>(color(.73, .73, .73));
    auto green = make_shared<diffuse>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    scene.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    scene.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    scene.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    scene.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    scene.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    scene.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    scene.add(make_shared<volume>(box1, 0.01, color(0, 0, 0)));
    scene.add(make_shared<volume>(box2, 0.01, color(1, 1, 1)));

    camera cam;

    cam.aspect_radio = 1.0;
    cam.image_width = 600;
    cam.sample_num = 200;
    cam.bounce_limit = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(278, 278, -800);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(scene);
}

void scene_final(int image_width, int samples_num, int bounce_limit) 
{
    hittable_list boxes1;
    auto ground = make_shared<diffuse>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) 
    {
        for (int j = 0; j < boxes_per_side; j++) 
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list scene;

    scene.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    scene.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<diffuse>(color(0.7, 0.3, 0.1));
    scene.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    scene.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    scene.add(make_shared<sphere>(point3(0, 150, 145), 50, 
                                  make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    scene.add(boundary);
    scene.add(make_shared<volume>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    scene.add(make_shared<volume>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<diffuse>(make_shared<image_texture>("earthmap.jpg"));
    scene.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    scene.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<diffuse>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<diffuse>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) 
    {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    scene.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2), 15),vec3(-100, 270, 395)));

    camera cam;

    cam.aspect_radio = 1.0;
    cam.image_width = image_width;
    cam.sample_num = samples_num;
    cam.bounce_limit = bounce_limit;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(478, 278, -600);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(scene);
}

// 正方形と内接円による円周率πの推定
// 参照referrence/Monte Carlo Estimating Pi.jpg
void test_monte_carlo_pi()
{
    // 円内に落ちたサンプル数
    size_t n = 0;
    size_t n_jitter = 0;
    // 抖动采样数的平方根
    // ジッターサンプル数の平方根
    size_t sqrt_N_jitter = 1000;

    for (int i = 0; i < sqrt_N_jitter; i++)
        for(int j = 0; j < sqrt_N_jitter; j++)
        {
            double x = random_double(-1, 1);
            double y = random_double(-1, 1);
            if (x * x + y * y < 1)
                n++;

            // 通过分级采样(抖动)，使采样更均匀，加速收敛
            // 
            // 階層化サンプリング（ジッター）によるサンプル分布の均一化と収束加速処理‌
            // 
            // 参照referrence/Monte Carlo Estimating Pi Jittering.png
            // [0,1) => [-1, 1)
            x = 2 * ((i + random_double()) / sqrt_N_jitter) - 1;
            y = 2 * ((j + random_double()) / sqrt_N_jitter) - 1;
            if (x * x + y * y < 1)
                n_jitter++;
        }

    clog << fixed << setprecision(12);
    clog << "Estimated Pi: " << 4.0 * n / (sqrt_N_jitter * sqrt_N_jitter) << endl;
    clog << "Estimated Pi with Jittering: " << 4.0 * n_jitter / (sqrt_N_jitter * sqrt_N_jitter) << endl;
}

// 正方形と内接円によるπ推定の収束性検証
// 
// 参照referrence/Monte Carlo Estimating Pi.jpg
void test_monte_carlo_pi_converage()
{
    clog << fixed << setprecision(12);

    // 円内サンプルカウンタ
    size_t n = 0;
    // 総試行回数
    size_t cnt = 0;

    while (true)
    {
        cnt++;

        double x = random_double(-1, 1);
        double y = random_double(-1, 1);

        if (x * x + y * y < 1)
            n++;

        // 100000回毎に結果を出力
        if (cnt % 100000 == 0)
            clog << "Estimated Pi: " << 4.0 * n / cnt << endl;      
    }
}

// x²の[0,2]区間における積分をモンテカルロ法で計算
void test_monte_carlo_integrate_x2()
{
    // 総サンプリング数
    size_t N = 1000000;
    // サンプリング区間 [0,2]
    int a = 0, b = 2;

    double sum = 0;

    for (int i = 0; i < N; i++)
    {
        double x = random_double(a, b);
        sum += (x * x);
    }

    clog << fixed << setprecision(12);
    // integrate = interval * average，采样次数N越多，average越接近expect
    // 積分値 = 区間幅 × 平均値（サンプリング数↑⇒平均値は期待値に収束）
    clog << "Integrate of x2: " << (b - a) * (sum / N) << endl;
}

// 用monte carlo求复杂pdf函数的halfway point（指刚好把概率积分对半分的采样点）
// 复杂pdf函数参照referrence/complex_pdf.png
// 如果递归下去，可以通过概率积分得到每一个采样点
// 说白了就是建立pdf的积分与采样点的联系
//
// 複雑な確率密度関数(pdf)のhalfway point（確率積分を二分するサンプリング点）をモンテカルロ法で算出
// 参照 referrence/complex_pdf.png
// 再帰的に適用することで各サンプル点の確率積分値を取得可能
// 要約：pdfの積分曲線とサンプリング点の対応関係構築
void test_monte_carlo_halfway_point()
{
    // 総サンプリング数
    const size_t N = 10000;
    // サンプル格納配列
    sample spl[N] = { 0 };
    // サンプリング区間 [0, 2π]
    double a = 0, b = 2 * pi;
    // 全サンプル点のpdf総和
    double sum = 0;
    // 計算効率化のため2πの逆数を事前計算
    double inv_2pi = 1 / (2 * pi);

    for (int i = 0; i < N; i++)
    {
        double x = random_double(a, b); // サンプリング点
        double sin_x = sin(x);
        double p_x = exp(-x * inv_2pi) * sin_x * sin_x; //pdf(x)
        sum += p_x;

        // 未ソート状態で配列に格納
        spl[i].x = x;
        spl[i].p_x = p_x;
    }

    // x値の昇順でサンプル配列をソート
    sort(begin(spl), end(spl), compare_by_x);

    // halfway point计算
    double half_sum = sum / 2;
    double halfway_point;
    double accum = 0;

    for (int i = 0; i < N; i++)
    {
        accum += spl[i].p_x;
        if (accum >= half_sum)
        {
            halfway_point = spl[i].x;
            break;
        }
    }

    clog << fixed << setprecision(12);
    clog << "Average: " << sum / N << endl;
    clog << "Integrate: " << (b - a) * sum / N << endl;
    clog << "Halfway point: " << halfway_point << endl;
}

// 重点サンプリング法によるx²の[0,2]区間積分計算
// 参照referrence/Importance Sampling.jpg 的
void test_importance_sampling_x2()
{
    cout << fixed << setprecision(12);
    int N = 1000000;
    double sum = 0;

    cout << "Sample nums = " << N << endl;
    // pdf为常数函数(等于没有重要性采样，但是可以基于此扩展成线性函数，二次函数等)
    // 一様分布pdf適用例（重点サンプリング未使用だが線形/二次関数へ拡張可能）
    for (int i = 0; i < N; i++) 
    {
        double x = icd_uniform(random_double());
        sum += x * x / pdf_uniform(x); // pdf除算＝区間幅乗算
    }
    
    cout << "Integrate of x2 with uniform pdf = " << (sum / N) << endl;

    // 線形分布pdf適用
    sum = 0;

    for (int i = 0; i < N; i++)
    {
        double d = random_double();
        if (d < 1e-8) // NaN防止処理
            continue;
        double x = icd_linear(d);
        sum += x * x / pdf_linear(x); // pdf除算＝区間幅乗算
    }

    cout << "Integrate of x2 with linear pdf = " << (sum / N) << endl;

    // 二次関数分布pdf適用
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        double d = random_double();
        if (d < 1e-8) // NaN防止処理
            continue;
        double x = icd_quadratic(d);
        sum += x * x / pdf_quadratic(x); // pdf除算＝区間幅乗算
    }
    cout << "Integrate of x2 with quadratic pdf = " << (sum / N) << endl;
}

// 用monte carlo求cos_theta的平方在各个方向上的积分 theta是随机方向向量与z轴的夹角
// 各个方向上的积分可以看作是在单位球体上的积分
// 单位球体，方向向量长度为1，所以cos_theta = 1 * cos_theta = d.z
// 单位球体的情况下，可以把3D向量的采样转成对2D平面的采样。2D采样要比3D采样快得多
// 均匀采样的话 pdf = 1 / 球体面积 = 1 / 4pi
//
// モンテカルロ法による単位球面上のcosθ^2積分計算
// θ: ランダム方向ベクトルとz軸のなす角
// 1. 方向積分の球面表現:
//    全方向の積分は単位球面(半径1)上の積分と等価
// 2. 幾何学的簡略化:
//    単位球面上では方向ベクトル長=1 ⇒ cosθ = d.z()成分で直接計算可能
// 3. 効率的サンプリング戦略:
//    3D球面サンプリングを2Dパラメータ空間(θ,φ)へ射影
//    → 計算量削減のため極座標系を利用
// 4. 一様サンプリング時の確率密度:
//    pdf = 1 / 球面表面積 = 1/(4π) （球面表面積公式 S=4πr²）
void test_cos2_on_unit_sphere()
{
    // 総サンプル数
    size_t N = 1000000;
    // サンプルリンクベクトル
    vec3 d;

    double sum = 0;

    for (int i = 0; i < N; i++)
    {
        d = random_unit_vector();
        sum += (d.z() * d.z() / pdf_sphere_uniform(d));
    }

    clog << fixed << setprecision(12);
    clog << "Integrate of cos_theta square: " << sum / N << endl;
}

// 単位球面上で任意方向に一様分布するサンプルを生成するテスト
// 参照referrence/uniform_sample_sphere.jpg
void test_uniform_sample_on_unit_sphere()
{
    for (int i = 0; i < 200; i++) 
    {
        double r1 = random_double();
        double r2 = random_double();
        double x = cos(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
        double y = sin(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
        double z = 1 - 2 * r2;
        clog << x << " " << y << " " << z << endl;
    }
}

// 半球サンプリングによるcosθの3乗積分の検証
// 参照referrence/cos_cube_hemi_sample.jpg
// 理論値pi/2に収束することでサンプリングの不偏性を確認
void test_cos_cube_hemi_sample()
{
    int N = 1000000;

    double sum = 0.0;
    for (int i = 0; i < N; i++) 
    {
        double r2 = random_double();
        sum += f_cos_cube(1 - r2) / pdf_hemi();
    }

    clog << fixed << setprecision(12);
    clog << "PI/2 = " << pi / 2.0 << endl;
    clog << "Estimate = " << sum / N << endl;
}


// cos/pi分布に基づくサンプリングでcosθの3乗積分を検証
// 参照referrence/cos_cube_cos_sample.png 
// 参照referrence/Lambertian Scatter PDF.jpg
// 理論値pi/2に収束することでLambertian散乱のPDFの正当性を確認
void test_cos_cube_cos_sample()
{
    int N = 1000000;

    double sum = 0.0;
    for (int i = 0; i < N; i++) 
    {
        //double r1 = random_double();
        double r2 = random_double();

        //double phi = 2 * pi * r1;
        //double x = cos(phi) * sqrt(r2);
        //double y = sin(phi) * sqrt(r2);
        double z = sqrt(1 - r2); // 単位ベクトルのz成分 = cosθ

        sum += f_cos_cube(z) / pdf_cos(z);
    }

    clog << fixed << setprecision(12);
    clog << "PI/2 = " << pi / 2.0 << endl;
    clog << "Estimate = " << sum / N << endl;
}

int main()
{
    time_t start_time, end_time;

    // タイミング開始
    time(&start_time);

    switch (7)
    {
    case 1:
        scene_bouncing_spheres();
        break;
    case 2:
        scene_checkered_spheres();
        break;
    case 3:
        scene_earth();
        break;
    case 4:
        scene_perlin_spheres();
        break;
    case 5:
        scene_quads();
        break;
    case 6:
        scene_simple_light();
        break;
    case 7:
        scene_cornell_box();
        break;
    case 8:
        scene_cornell_smoke();
        break;
    case 9:
        scene_final(800, 10000, 40);
        break;
    case 10:
        test_monte_carlo_pi();
        break;
    case 11:
        test_monte_carlo_pi_converage();
        break;
    case 12:
        test_monte_carlo_integrate_x2();
        break;
    case 13:
        test_monte_carlo_halfway_point();
        break;
    case 14:
        test_importance_sampling_x2();
        break;
    case 15:
        test_cos2_on_unit_sphere();
        break;
    case 16:
        test_uniform_sample_on_unit_sphere();
        break;
    case 17:
        test_cos_cube_hemi_sample();
        break;
    case 18:
        test_cos_cube_cos_sample();
        break;
    default:
        scene_final(400, 250, 4);
        break;
    }

    // タイミング終了
    time(&end_time);

    clog << "elapsed time: " << difftime(end_time, start_time) << " seconds\n";
    clog << "total bounce: " << total_bounce << endl;
    cin.get();

    return 0;
}