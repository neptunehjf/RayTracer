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
    //
    // 1. ここで使用されているpoint3(0.0, 0.0, -1.0)は右辺値（rvalue）です
    // 2. したがってsphereコンストラクタの引数は、const point3& または point3&& で宣言する必要があります
    //    （これら以外の形式では右辺値のバインドが不可能となり、コンパイルエラーが発生します）
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // Render
    cam.render(scene);
}