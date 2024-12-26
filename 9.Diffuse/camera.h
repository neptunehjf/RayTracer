#pragma once

#include "common.h"
#include "hittable_list.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;
    int sample_nums = 10;

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
                color pixel_color;

                for (int n = 0; n < sample_nums; n++)
                {
                    // ���ÿһ��camera���������ĸ��������������������
                    ray r = get_ray(i, j);

                    // ����ray���color���ۼ������������Բ������Դﵽƽ����Ч��
                    pixel_color += ray_color(r, scene);
                }

                pixel_color /= sample_nums;
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

    // ��ȡ����i,j�������ص�����Ϊ���ģ�xy�ϵķ�������(-0.5 pixel ,0.5 pixel)����������Ĺ���ray
    // ��Ȼ������������ķ�Χ����ͬһ�������ڣ�������Կ�����׷�͹�դ�������Ĳ�ͬ
    // ����ǹ�դ������Ϊ��դ����������С��λ�������أ���ͬһ�������ڲ���û���κ�����
    // ��׷�����Σ�viewport��һ�������ڵ�λ�øı䣬Ҳ�ᵼ�¹���ray����ı䣬���Բ�������Ч��
    // ��֮����դ���Ĳ�����С��λ�����أ���׷�Ĳ�����С��λ�ǹ���ray�����������޵Ķ�����ray���������޵ģ���������
    ray get_ray(int i, int j) const
    {
        vec3 offset = offset_square();
        point3 pixel = pixel_zero + vec3((i + offset.x()) * pixel_x, (j + offset.y()) * pixel_y, 0.0);
        vec3 ray_dir = pixel - cam_pos;
        return ray(cam_pos, ray_dir);
    }

    // ����һ����xy�����ϵ����ƫ��������-0.5,0.5֮�� Z������ƫ����
    // �������ڷ��������������
    vec3 offset_square() const
    {
        return vec3(-0.5 + random_double(), -0.5 + random_double(), 0.0);
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