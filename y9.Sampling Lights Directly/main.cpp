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

            if ((center - point3(0.0, 0.2, 0.0)).length() > 1.0 && // 去除了离大球过于接近的小球
                (center - point3(-4.0, 0.2, 0.0)).length() > 1.0 &&
                (center - point3(4.0, 0.2, 0.0)).length() > 1.0)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    color albedo = color::random() * color::random();
                    sphere_material = make_shared<diffuse>(albedo);

                    // center2的位置反应了物体的运动速度
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

    // 对于物体数极少的情况，用aabb包围盒优化算法反而效率更低
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

// 通过正方形和其内切圆估算Pi值
// 参照referrence/Monte Carlo Estimating Pi.jpg
void test_monte_carlo_pi()
{
    // 落在圆内的采样数
    size_t n = 0;
    size_t n_jitter = 0;
    // 抖动采样数的平方根
    size_t sqrt_N_jitter = 1000;

    for (int i = 0; i < sqrt_N_jitter; i++)
        for(int j = 0; j < sqrt_N_jitter; j++)
        {
            double x = random_double(-1, 1);
            double y = random_double(-1, 1);
            if (x * x + y * y < 1)
                n++;

            // 通过分级采样(抖动)，使采样更均匀，加速收敛
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

// 通过正方形和其内切圆估算Pi值
// 参照referrence/Monte Carlo Estimating Pi.jpg
void test_monte_carlo_pi_converage()
{
    clog << fixed << setprecision(12);

    // 落在圆内的采样数
    size_t n = 0;
    // 运行计数
    size_t cnt = 0;

    while (true)
    {
        cnt++;

        double x = random_double(-1, 1);
        double y = random_double(-1, 1);

        if (x * x + y * y < 1)
            n++;

        // 每隔100000次采样打印一次结果
        if (cnt % 100000 == 0)
            clog << "Estimated Pi: " << 4.0 * n / cnt << endl;      
    }
}

// 用monte carlo求x2在[0,2]上的积分
void test_monte_carlo_integrate_x2()
{
    // 总采样数
    size_t N = 1000000;
    // 采样范围interval[0,2]
    int a = 0, b = 2;

    double sum = 0;

    for (int i = 0; i < N; i++)
    {
        double x = random_double(a, b);
        sum += (x * x);
    }

    clog << fixed << setprecision(12);
    // integrate = interval * average，采样次数N越多，average越接近expect
    clog << "Integrate of x2: " << (b - a) * (sum / N) << endl;
}

// 用monte carlo求复杂pdf函数的halfway point（指刚好把概率积分对半分的采样点）
// 复杂pdf函数参照referrence/complex_pdf.png
// 如果递归下去，可以通过概率积分得到每一个采样点
// 说白了就是建立pdf的积分与采样点的联系
void test_monte_carlo_halfway_point()
{
    // 总采样数
    const size_t N = 10000;
    // 采样数组
    sample spl[N] = { 0 };
    // 采样范围interval[0,2 * pi]
    double a = 0, b = 2 * pi;
    // 所有采样点的pdf总和
    double sum = 0;
    // 2pi的倒数，提前计算以提高效率
    double inv_2pi = 1 / (2 * pi);

    for (int i = 0; i < N; i++)
    {
        double x = random_double(a, b); //采样点
        double sin_x = sin(x);
        double p_x = exp(-x * inv_2pi) * sin_x * sin_x; //pdf(x)
        sum += p_x;

        // 采样结果存入数组(未排序)
        spl[i].x = x;
        spl[i].p_x = p_x;
    }

    // 按x从小到大顺序排序采样数组
    sort(begin(spl), end(spl), compare_by_x);

    // 计算halfway point
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

// Monte Carlo之重要性采样 求x2在[0,2]上的积分
// 参照referrence/Inverse Method.jpg
// 参照referrence/Importance Sampling.png
void test_importance_sampling_x2()
{
    cout << fixed << setprecision(12);
    int N = 1000000;
    double sum = 0;

    cout << "Sample nums = " << N << endl;
    // pdf为常数函数(等于没有重要性采样，但是可以基于此扩展成线性函数，二次函数等)
    for (int i = 0; i < N; i++) 
    {
        double x = icd_uniform(random_double());
        sum += x * x / pdf_uniform(x); // 除以pdf相当于乘以interval
    }
    
    cout << "Integrate of x2 with uniform pdf = " << (sum / N) << endl;

    // pdf为线性函数
    sum = 0;

    for (int i = 0; i < N; i++)
    {
        double d = random_double();
        if (d < 1e-8) // 防止NaN的情况
            continue;
        double x = icd_linear(d);
        sum += x * x / pdf_linear(x); // 除以pdf相当于乘以interval
    }

    cout << "Integrate of x2 with linear pdf = " << (sum / N) << endl;

    // pdf为二次函数
    sum = 0;
    for (int i = 0; i < N; i++)
    {
        double d = random_double();
        if (d < 1e-8) // 防止NaN的情况
            continue;
        double x = icd_quadratic(d);
        sum += x * x / pdf_quadratic(x); // 除以pdf相当于乘以interval
    }
    cout << "Integrate of x2 with quadratic pdf = " << (sum / N) << endl;
}

// 用monte carlo求cos_theta的平方在各个方向上的积分 theta是随机方向向量与z轴的夹角
// 各个方向上的积分可以看作是在单位球体上的积分
// 单位球体，方向向量长度为1，所以cos_theta = 1 * cos_theta = d.z
// 单位球体的情况下，可以把3D向量的采样转成对2D平面的采样。2D采样要比3D采样快得多
// 均匀采样的话 pdf = 1 / 球体面积 = 1 / 4pi
void test_cos2_on_unit_sphere()
{
    // 总采样数
    size_t N = 1000000;
    // 采样向量
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

// 打印出在任意方向上均匀分布的样本
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

// 在半球上采样求cos的3次方的积分
// 参照referrence/cos_cube_hemi_sample.jpg
// 手算结果是pi/2，采样结果收敛于pi/2，验证了采样pdf是无偏的
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


// 在cos/pi上采样求cos的3次方的积分
// 参照referrence/cos_cube_cos_sample.png 
// 参照referrence/Lambertian Scatter PDF.jpg
// 手算结果是pi/2，采样结果收敛于pi/2，验证了采样pdf是无偏的
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
        double z = sqrt(1 - r2); // 因为是单位向量 z等于cos theta

        sum += f_cos_cube(z) / pdf_cos(z);
    }

    clog << fixed << setprecision(12);
    clog << "PI/2 = " << pi / 2.0 << endl;
    clog << "Estimate = " << sum / N << endl;
}

int main()
{
    time_t start_time, end_time;

    // 开始计时
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

    // 结束计时
    time(&end_time);

    clog << "elapsed time: " << difftime(end_time, start_time) << " seconds\n";
    clog << "total bounce: " << total_bounce << endl;
    cin.get();

    return 0;
}