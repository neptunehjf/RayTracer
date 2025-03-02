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
    int sample_num = 10; //每个像素的采样数
    int bounce_limit = 10;
    int sqrt_sample_num = 10;

    double vfov = 90.0;                         // 竖直方向的FOV
    point3 look_from = point3(0.0, 0.0, 0.0);   // cam视线起点
    point3 look_at   = point3(0.0, 0.0, -1.0);  // cam视线终点
    point3 vup       = vec3(0.0, 1.0, 0.0);     // cam整体的上方向

    // referrence/defocus model_1.png
    // 透镜(散焦disk)越大，在里焦距越远的地方成像越模糊
    double defocus_angle = 0.0;    // 角度，和焦距一起决定了透镜的大小
    double focus_dist = 10.0;      // 完美成像的焦距

    color  background; // 场景背景色

    // 光追的渲染是和camera绑定的，因此把render放到camera类里
	void render(const hittable_list& scene)
	{
		initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            // 显示进度
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
                            // 求出每一个camera到像素中心附近的随机采样到的射线
                            ray r = get_ray(i, j, i_jitter, j_jitter);

                            // 根据ray算出color，累加起来，最后除以采样数以达到平滑的效果
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
    vec3 defocus_disk_u; // 透镜u向量
    vec3 defocus_disk_v; // 透镜v向量

	void initialize()
	{
        image_height = (int)(image_width / aspect_radio); // 因为是近似计算，有可能是0
        image_height = (image_height < 1) ? 1 : image_height;  // 保证不小于1
      
        cam_pos = look_from;

        sqrt_sample_num = (int)sqrt(sample_num);

        double theta = degree_to_radian(vfov); // tan是按弧度值实现的，必须提供弧度制而不是角度值
        double viewport_h = 2 * focus_dist * tan(theta / 2); // 参考referrence/camera1.png
        double viewport_w = viewport_h * (double(image_width) / image_height); // 因为image_height是近似计算的，不能直接用aspect_radio

        // 确定camera归一化空间坐标
        // 参考referrence/camera2.png
        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w)); // 因为v始终与vup共面，所以cross(vup, w)和cross(v, w)同向
        v = cross(w, u);

        // viewport UV 在camera坐标系的向量
        vec3 viewport_u = viewport_w * u;
        vec3 viewport_v = -viewport_h * v; // 因为viewport的所在的v方向和camera坐标系v方向相反

        // 换算成每个pixel对应的uv大小
        pixel_u = viewport_u / image_width;
        pixel_v = viewport_v / image_height;

        // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
        point3 viewport_zero = cam_pos - (w * focus_dist) - (viewport_u / 2) - (viewport_v / 2);
        // 转为像素的中心坐标
        pixel_zero = viewport_zero + 0.5 * (pixel_u + pixel_v);

        // 计算透镜的uv向量
        // referrence/defocus model_2.jpg
        double defocus_radius = focus_dist * tan(degree_to_radian(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
	}

    // 获取在以i,j处的像素的中心为中心，xy上的方形区域(-0.5 pixel ,0.5 pixel)内随机采样的光线ray
    // 显然这里随机采样的范围是在同一个像素内，这里可以看出光追和光栅化采样的不同
    // 如果是光栅化，因为光栅化采样的最小单位就是像素，在同一个像素内采样没有任何作用
    // 光追的情形，viewport的一个像素内的位置改变，也会导致光线ray方向改变，所以采样是有效的
    // 总之，光栅化的采样最小单位是像素，光追的采样最小单位是光线ray，像素是有限的而光线ray可以是无限的，高下立判
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

    // 生成一个在xy方向上的随机偏移量，在-0.5,0.5之间 Z方向无偏移量
    // 可用于在方形区域随机采样
    vec3 offset_square(int i_jitter, int j_jitter) const
    {
        // 通过分级采样(抖动)，使采样更均匀，加速收敛
        // 参照referrence/Monte Carlo Estimating Pi Jittering.png
        // [0,1) => [-0.5, 0.5)
        double x = (i_jitter + random_double()) / sqrt_sample_num - 0.5;
        double y = (j_jitter + random_double()) / sqrt_sample_num - 0.5;

        return vec3(x, y, 0.0);
    }

    // 计算光线的颜色
    color ray_color(const ray& r, const hittable_list& scene, int bounce_count)
    {
        // 给弹射加一个条件限制，防止无限弹射下去
        if (bounce_count >= bounce_limit)
            return vec3(0.0, 0.0, 0.0);

        // 返回光线与物体交点的法线
        hit_record rec;
        // 因为浮点数精度的原因，交点可能不会正好在物体表面，交点有可能在物体外部也有可能在内部
        // 如果交点在物体内部的话，以tmin为0进行计算就不对了，要把tmin修正为稍大的值
        // 以上修正解决了shadow acne的问题
        interval ray_t(0.001, inf);
        if (scene.hit(r, ray_t, rec))
        {
            ray ray_out;
            color attenuation;
            double sample_pdf;

            // 以下分支，光源材质和弹射材质是互斥的，因此逻辑简单清晰。
            // 以后有可能会同时支持两种特性的材质，暂时先放着吧
            // 
            // 可以看到光追优于光栅化的地方。光追的光源衰减，是通过弹射自然算出的
            // 光栅化无法自然做到，只能额外用公式来模拟光源衰减
            // 
            // 如果当前交点有弹射，说明是非光源材质，非光源材质只需返回弹射颜色
            if (rec.mat->scatter(r, rec, attenuation, ray_out, sample_pdf))
            {
                total_bounce++;
                double scatter_pdf = rec.mat->scatter_pdf(r, rec, ray_out);
                // sample_pdf和scatter_pdf完全一样，则实际没进行重要性采样，只是形式变成了重要性采样
                // 把sample_pdf设成scatter_pdf，debug时可以排除采样的干扰，判断收敛目标(散射函数)本身是否有问题
                sample_pdf = scatter_pdf;
                // 参照 referrence/Importance Sampling.png
                return attenuation * ray_color(ray_out, scene, ++bounce_count) * 
                       scatter_pdf / sample_pdf;
            }
            // 如果当前交点在光源材质，则只需返回光源颜色
            else
                return rec.mat->emit(rec.u, rec.v, rec.p);
        }

        // 什么都没击中，返回背景色
        return background;
        // 根据r的y分量进行lerp
        //vec3 r_uint = unit_vector(r.direction());
        //// [-1.0, 1.0] ==> [0.0, 1.0]
        //double a = 0.5 * (r_uint.y() + 1.0);

        //// 返回蓝色与白色之间的lerp
        //return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        
    }

    point3 defocus_disk_sample() const 
    {
        // Returns a random point in the camera defocus disk.
        vec3 p = random_on_unit_disk();
        return cam_pos + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

