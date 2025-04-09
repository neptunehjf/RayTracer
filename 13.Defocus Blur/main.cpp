#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main() 
{
    // Camera
    camera cam;
    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.sample_num = 100;
    cam.bounce_limit = 50;

    cam.vfov = 20.0;
    cam.look_from = point3(-2.0, 2.0, 1.0);
    cam.look_at = point3(0.0, 0.0, -1.0);
    cam.vup = vec3(0.0, 1.0, 0.0);

    cam.defocus_angle = 10.0;
    cam.focus_dist = 3.4;

    // Scene
    hittable_list scene;

    auto material_ground = make_shared<diffuse>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<diffuse>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // 注意这里point3(XXXXX)是右值
    // 所以sphere构造函数的入参要么是const point3&，要么是point&&，否则编译报错
    //
    // 1. ここで使用されているpoint3(XXXXX)は右辺値（rvalue）です
    // 2. したがってsphereコンストラクタの引数は、const point3& または point3&& で宣言する必要があります
    //    （これら以外の形式では右辺値のバインドが不可能となり、コンパイルエラーが発生します）

    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    scene.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    scene.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    scene.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Render
    cam.render(scene);
}