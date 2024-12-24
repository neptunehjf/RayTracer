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

    // ע������point3(0.0, 0.0, -1.0)����ֵ
    // ����sphere���캯�������Ҫô��const point3&��Ҫô��point&&��������뱨��
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // Render
    cam.render(scene);
}