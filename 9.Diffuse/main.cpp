#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

int main() 
{
    // Camera
    camera cam;
    cam.aspect_radio = 16.0 / 9.0;
    cam.image_width = 400;

    // Scene
    hittable_list scene;

    // 注意这里point3(0.0, 0.0, -1.0)是右值
    // 所以sphere构造函数的入参要么是const point3&，要么是point&&，否则编译报错
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // Render
    cam.render(scene);
}