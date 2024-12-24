#pragma once

#include "common.h"
#include "hittable_list.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;

    // ��׷����Ⱦ�Ǻ�camera�󶨵ģ���˰�render�ŵ�camera����
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // ��ʾ����
            clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

            for (int i = 0; i < image_width; i++)
            {
                // ���ÿһ��camera���������ĵ�����
                point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
                vec3 ray_dir = pixel - cam_pos;
                ray r(cam_pos, ray_dir);

                // ����ray���color
                color pixel_color = ray_color(r, scene);
                write_color(cout, pixel_color);
            }
        }
        clog << "\rDone.                 \n";
	}

private:
    int image_height;
    point3 pixel_zero;
    double pixel_x;
    double pixel_y;
    point3 cam_pos;

	void initialize()
	{
        image_height = (int)(image_width / aspect_radio); // ��Ϊ�ǽ��Ƽ��㣬�п�����0
        image_height = (image_height < 1) ? 1 : image_height;  // ��֤��С��1
        cam_pos = point3(0.0, 0.0, 0.0);

        double focal_length = 1.0; // ���࣬camera��viewport֮��ľ��룬Ŀǰ��Ϊ1.0
        double viewport_h = 2.0;
        double viewport_w = viewport_h * (double(image_width) / image_height); // ��Ϊimage_height�ǽ��Ƽ���ģ�����ֱ����aspect_radio

        // viewport UV ��camera����ϵ������
        vec3 u = vec3(viewport_w, 0.0, 0.0);
        vec3 v = vec3(0.0, -viewport_h, 0.0); // ��Ϊviewport�����ڵ�v�����camera����ϵy�����෴

        // �����ÿ��pixel��Ӧ��uv��С
        pixel_x = viewport_w / image_width;
        pixel_y = -viewport_h / image_height;

        // ����viewportԭ����camera����ϵ��λ�ã�viewportԭ�������Ͻǣ����������focal_length����xyƽ�棬���λ�ú�viewport�����غ�
        point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;
        // תΪ���ص���������
        pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);
	}

    // ������ߵ���ɫ
    color ray_color(const ray& r, const hittable_list& scene)
    {
        // ���ع��������彻��ķ���
        hit_record rec;
        interval ray_t(0.0, inf);
        if (scene.hit(r, ray_t, rec))
        {
            // �����ڼ��ν׶�(hit)�Ѿ��������
            // [-1.0, 1.0] ==> [0.0, 1.0]
            return 0.5 * (rec.normal + vec3(1.0, 1.0, 1.0));
        }

        // ����Ϊ����ɫ
        // ����r��y��������lerp
        vec3 r_uint = unit_vector(r.direction());
        // [-1.0, 1.0] ==> [0.0, 1.0]
        double a = 0.5 * (r_uint.y() + 1.0);

        // ������ɫ���ɫ֮���lerp
        return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};