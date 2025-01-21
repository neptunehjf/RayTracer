#include "common.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"

int main() 
{
    time_t start_time, end_time;

    // ��ʼ��ʱ
    time(&start_time);

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

    // Scene
    hittable_list scene;

    auto checker = make_shared<checker_texture>(1.0, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<diffuse>(checker);
    scene.add(make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000.0, ground_material));

    for (int a = -11; a < 11; a++) 
    {
        for (int b = -11; b < 11; b++) 
        {
            double choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(0.0, 0.2, 0.0)).length() > 1.0 && // ȥ�����������ڽӽ���С��
                (center - point3(-4.0, 0.2, 0.0)).length() > 1.0 &&
                (center - point3(4.0, 0.2, 0.0)).length() > 1.0)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    color albedo = color::random() * color::random();
                    sphere_material = make_shared<diffuse>(albedo);

                    // center2��λ�÷�Ӧ��������˶��ٶ�
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

    // Render
    cam.render(scene);

    // ������ʱ
    time(&end_time);

    clog << "elapsed time: " << difftime(end_time, start_time) << " seconds\n";
    clog << "total bounce: " << total_bounce << endl;
    cin.get();

    return 0;
}