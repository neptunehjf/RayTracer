#pragma once

#include "common.h"
#include "hittable_list.h"

class camera
{
public:
	double aspect_radio = 1.0;
	int image_width = 100;

    // 光追的渲染是和camera绑定的，因此把render放到camera类里
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // 显示进度
            clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

            for (int i = 0; i < image_width; i++)
            {
                // 求出每一个camera到像素中心的射线
                point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
                vec3 ray_dir = pixel - cam_pos;
                ray r(cam_pos, ray_dir);

                // 根据ray算出color
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
        image_height = (int)(image_width / aspect_radio); // 因为是近似计算，有可能是0
        image_height = (image_height < 1) ? 1 : image_height;  // 保证不小于1
        cam_pos = point3(0.0, 0.0, 0.0);

        double focal_length = 1.0; // 焦距，camera和viewport之间的距离，目前定为1.0
        double viewport_h = 2.0;
        double viewport_w = viewport_h * (double(image_width) / image_height); // 因为image_height是近似计算的，不能直接用aspect_radio

        // viewport UV 在camera坐标系的向量
        vec3 u = vec3(viewport_w, 0.0, 0.0);
        vec3 v = vec3(0.0, -viewport_h, 0.0); // 因为viewport的所在的v方向和camera坐标系y方向相反

        // 换算成每个pixel对应的uv大小
        pixel_x = viewport_w / image_width;
        pixel_y = -viewport_h / image_height;

        // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
        point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;
        // 转为像素的中心坐标
        pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);
	}

    // 计算光线的颜色
    color ray_color(const ray& r, const hittable_list& scene)
    {
        // 返回光线与物体交点的法线
        hit_record rec;
        interval ray_t(0.0, inf);
        if (scene.hit(r, ray_t, rec))
        {
            // 法线在几何阶段(hit)已经计算好了
            // [-1.0, 1.0] ==> [0.0, 1.0]
            return 0.5 * (rec.normal + vec3(1.0, 1.0, 1.0));
        }

        // 以下为背景色
        // 根据r的y分量进行lerp
        vec3 r_uint = unit_vector(r.direction());
        // [-1.0, 1.0] ==> [0.0, 1.0]
        double a = 0.5 * (r_uint.y() + 1.0);

        // 返回蓝色与白色之间的lerp
        return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};