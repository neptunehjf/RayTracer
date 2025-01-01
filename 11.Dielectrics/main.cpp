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
    cam.bounce_limit = 50;

    // Scene
    hittable_list scene;

    auto material_ground = make_shared<diffuse>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<diffuse>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectrics>(1.0 / 1.33);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // 注意这里point3(xxxxx)是右值
    // 所以sphere构造函数的入参要么是const point3&，要么是point&&，否则编译报错
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    scene.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    scene.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // Render
    cam.render(scene);
}