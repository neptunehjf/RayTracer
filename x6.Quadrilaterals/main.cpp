#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"

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

    cam.render(scene);
}

int main()
{
    time_t start_time, end_time;

    // 开始计时
    time(&start_time);

    switch (5)
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
    default:
        break;
    }

    // 结束计时
    time(&end_time);

    clog << "elapsed time: " << difftime(end_time, start_time) << " seconds\n";
    clog << "total bounce: " << total_bounce << endl;
    cin.get();

    return 0;
}