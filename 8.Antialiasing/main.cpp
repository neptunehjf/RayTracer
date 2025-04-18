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

    // 廣吭宸戦point3(0.0, 0.0, -1.0)頁嘔峙
    // 侭參sphere更夛痕方議秘歌勣担頁const point3&��勣担頁point&&��倦夸園咎烏危
    //
    // 1. ここで聞喘されているpoint3(0.0, 0.0, -1.0)は嘔�x�｣�rvalue��です
    // 2. したがってsphereコンストラクタの哈方は、const point3& または point3&& で傚冱する駅勣があります
    //    �┐海譴蚋塒發緑諒修任腕吁x�､離丱ぅ鵐匹�音辛嬬となり、コンパイルエラ�`が�k伏します��
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // Render
    cam.render(scene);
}