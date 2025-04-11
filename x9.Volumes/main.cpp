#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "volume.h"

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

int main()
{
    time_t start_time, end_time;

    // タイミング開始
    time(&start_time);

    switch (8)
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
    default:
        break;
    }

    // タイミング終了
    time(&end_time);

    clog << "elapsed time: " << difftime(end_time, start_time) << " seconds\n";
    clog << "total bounce: " << total_bounce << endl;
    cin.get();

    return 0;
}