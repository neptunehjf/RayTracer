#pragma once

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "dbg.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;
    int sample_num = 10; //ÿ�����صĲ�����
    int bounce_limit = 10;
    int sqrt_sample_num = 10;

    double vfov = 90.0;                         // ��ֱ�����FOV
    point3 look_from = point3(0.0, 0.0, 0.0);   // cam�������
    point3 look_at   = point3(0.0, 0.0, -1.0);  // cam�����յ�
    point3 vup       = vec3(0.0, 1.0, 0.0);     // cam������Ϸ���

    // referrence/defocus model_1.png
    // ͸��(ɢ��disk)Խ�����ｹ��ԽԶ�ĵط�����Խģ��
    double defocus_angle = 0.0;    // �Ƕȣ��ͽ���һ�������͸���Ĵ�С
    double focus_dist = 10.0;      // ��������Ľ���

    color  background; // ��������ɫ

    // ��׷����Ⱦ�Ǻ�camera�󶨵ģ���˰�render�ŵ�camera����
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // ��ʾ����
            clog << "\rScanlines remaining: " << (image_height - j) << ' ' 
                 << " BBox Unhit Count: " << unhit_count << " BBox Hit Count: " 
                 << hit_count << flush;

            for (int i = 0; i < image_width; i++)
            {
                //if (i + 1 == 220 && j + 1 == 500) // for debug
                {
                    color pixel_color = vec3(0.0, 0.0, 0.0);

                    for (int i_jitter = 0; i_jitter < sqrt_sample_num; i_jitter++)
                        for (int j_jitter = 0; j_jitter < sqrt_sample_num; j_jitter++)
                        {
                            // ���ÿһ��camera���������ĸ��������������������
                            ray r = get_ray(i, j, i_jitter, j_jitter);

                            // ����ray���color���ۼ������������Բ������Դﵽƽ����Ч��
                            pixel_color += ray_color(r, scene, 0);
                        }

                    pixel_color /= sample_num;
                    write_color(cout, pixel_color);
                }

            }
        }
        clog << "\rDone.                 \n";
	}

private:
    int image_height = 10;
    point3 pixel_zero;
    vec3 pixel_u;
    vec3 pixel_v;
    point3 cam_pos;
    vec3 u, v, w;
    vec3 defocus_disk_u; // ͸��u����
    vec3 defocus_disk_v; // ͸��v����

	void initialize()
	{
        image_height = (int)(image_width / aspect_radio); // ��Ϊ�ǽ��Ƽ��㣬�п�����0
        image_height = (image_height < 1) ? 1 : image_height;  // ��֤��С��1
      
        cam_pos = look_from;

        sqrt_sample_num = (int)sqrt(sample_num);

        double theta = degree_to_radian(vfov); // tan�ǰ�����ֵʵ�ֵģ������ṩ�����ƶ����ǽǶ�ֵ
        double viewport_h = 2 * focus_dist * tan(theta / 2); // �ο�referrence/camera1.png
        double viewport_w = viewport_h * (double(image_width) / image_height); // ��Ϊimage_height�ǽ��Ƽ���ģ�����ֱ����aspect_radio

        // ȷ��camera��һ���ռ�����
        // �ο�referrence/camera2.png
        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w)); // ��Ϊvʼ����vup���棬����cross(vup, w)��cross(v, w)ͬ��
        v = cross(w, u);

        // viewport UV ��camera����ϵ������
        vec3 viewport_u = viewport_w * u;
        vec3 viewport_v = -viewport_h * v; // ��Ϊviewport�����ڵ�v�����camera����ϵv�����෴

        // �����ÿ��pixel��Ӧ��uv��С
        pixel_u = viewport_u / image_width;
        pixel_v = viewport_v / image_height;

        // ����viewportԭ����camera����ϵ��λ�ã�viewportԭ�������Ͻǣ����������focal_length����xyƽ�棬���λ�ú�viewport�����غ�
        point3 viewport_zero = cam_pos - (w * focus_dist) - (viewport_u / 2) - (viewport_v / 2);
        // תΪ���ص���������
        pixel_zero = viewport_zero + 0.5 * (pixel_u + pixel_v);

        // ����͸����uv����
        // referrence/defocus model_2.jpg
        double defocus_radius = focus_dist * tan(degree_to_radian(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
	}

    // ��ȡ����i,j�������ص�����Ϊ���ģ�xy�ϵķ�������(-0.5 pixel ,0.5 pixel)����������Ĺ���ray
    // ��Ȼ������������ķ�Χ����ͬһ�������ڣ�������Կ�����׷�͹�դ�������Ĳ�ͬ
    // ����ǹ�դ������Ϊ��դ����������С��λ�������أ���ͬһ�������ڲ���û���κ�����
    // ��׷�����Σ�viewport��һ�������ڵ�λ�øı䣬Ҳ�ᵼ�¹���ray����ı䣬���Բ�������Ч��
    // ��֮����դ���Ĳ�����С��λ�����أ���׷�Ĳ�����С��λ�ǹ���ray�����������޵Ķ�����ray���������޵ģ���������
    ray get_ray(int i, int j, int i_jitter, int j_jitter) const
    {
        vec3 offset = offset_square(i_jitter, j_jitter);
        point3 pixel = pixel_zero + ((i + offset.x()) * pixel_u) 
                                  + ((j + offset.y()) * pixel_v);
        point3 ray_origin = (defocus_angle <= 0) ? cam_pos : defocus_disk_sample();
        vec3 ray_dir = pixel - ray_origin;
        double ray_time = random_double();

        return ray(ray_origin, ray_dir, ray_time);
    }

    // ����һ����xy�����ϵ����ƫ��������-0.5,0.5֮�� Z������ƫ����
    // �������ڷ��������������
    vec3 offset_square(int i_jitter, int j_jitter) const
    {
        // ͨ���ּ�����(����)��ʹ���������ȣ���������
        // ����referrence/Monte Carlo Estimating Pi Jittering.png
        // [0,1) => [-0.5, 0.5)
        double x = (i_jitter + random_double()) / sqrt_sample_num - 0.5;
        double y = (j_jitter + random_double()) / sqrt_sample_num - 0.5;

        return vec3(x, y, 0.0);
    }

    // ������ߵ���ɫ
    color ray_color(const ray& r, const hittable_list& scene, int bounce_count)
    {
        // �������һ���������ƣ���ֹ���޵�����ȥ
        if (bounce_count >= bounce_limit)
            return vec3(0.0, 0.0, 0.0);

        // ���ع��������彻��ķ���
        hit_record rec;
        // ��Ϊ���������ȵ�ԭ�򣬽�����ܲ���������������棬�����п����������ⲿҲ�п������ڲ�
        // ��������������ڲ��Ļ�����tminΪ0���м���Ͳ����ˣ�Ҫ��tmin����Ϊ�Դ��ֵ
        // �������������shadow acne������
        interval ray_t(0.001, inf);
        if (scene.hit(r, ray_t, rec))
        {
            ray ray_out;
            color attenuation;
            double sample_pdf;

            // ���·�֧����Դ���ʺ͵�������ǻ���ģ�����߼���������
            // �Ժ��п��ܻ�ͬʱ֧���������ԵĲ��ʣ���ʱ�ȷ��Ű�
            // 
            // ���Կ�����׷���ڹ�դ���ĵط�����׷�Ĺ�Դ˥������ͨ��������Ȼ�����
            // ��դ���޷���Ȼ������ֻ�ܶ����ù�ʽ��ģ���Դ˥��
            // 
            // �����ǰ�����е��䣬˵���Ƿǹ�Դ���ʣ��ǹ�Դ����ֻ�践�ص�����ɫ
            if (rec.mat->scatter(r, rec, attenuation, ray_out, sample_pdf))
            {
                total_bounce++;
                double scatter_pdf = rec.mat->scatter_pdf(r, rec, ray_out);
                // sample_pdf��scatter_pdf��ȫһ������ʵ��û������Ҫ�Բ�����ֻ����ʽ�������Ҫ�Բ���
                // ��sample_pdf���scatter_pdf��debugʱ�����ų������ĸ��ţ��ж�����Ŀ��(ɢ�亯��)�����Ƿ�������
                sample_pdf = scatter_pdf;
                // ���� referrence/Importance Sampling.png
                return attenuation * ray_color(ray_out, scene, ++bounce_count) * 
                       scatter_pdf / sample_pdf;
            }
            // �����ǰ�����ڹ�Դ���ʣ���ֻ�践�ع�Դ��ɫ
            else
                return rec.mat->emit(rec.u, rec.v, rec.p);
        }

        // ʲô��û���У����ر���ɫ
        return background;
        // ����r��y��������lerp
        //vec3 r_uint = unit_vector(r.direction());
        //// [-1.0, 1.0] ==> [0.0, 1.0]
        //double a = 0.5 * (r_uint.y() + 1.0);

        //// ������ɫ���ɫ֮���lerp
        //return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        
    }

    point3 defocus_disk_sample() const 
    {
        // Returns a random point in the camera defocus disk.
        vec3 p = random_on_unit_disk();
        return cam_pos + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

