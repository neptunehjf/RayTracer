#include "vec3.h"
#include "color.h"
#include "ray.h"

color ray_color(const ray& r);
bool is_sphere_hit(const point3& center, double radius, const ray& r);

int main() {

    // Image 固定宽高比为16 : 9 
    double aspect_radio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) (image_width / aspect_radio); // 因为是近似计算，有可能是0
    image_height = (image_height < 1) ? 1 : image_height;  // 保证不小于1

    // Camera
    vec3 cam_pos = { 0.0, 0.0, 0.0 };
    double focal_length = 1.0; // 焦距，camera和viewport之间的距离，目前定为1.0
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double(image_width) / image_height); // 因为image_height是近似计算的，不能直接用aspect_radio

    // viewport UV 在camera坐标系的向量
    vec3 u = vec3(viewport_w, 0.0, 0.0);
    vec3 v = vec3(0.0, -viewport_h, 0.0); // 因为viewport的所在的v方向和camera坐标系y方向相反

    // 换算成每个pixel对应的uv大小
    double pixel_x = viewport_w / image_width;
    double pixel_y = -viewport_h / image_height;

    // 计算viewport原点在camera坐标系的位置，viewport原点在左上角，离相机距离focal_length，在xy平面，相机位置和viewport中心重合
    point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;
    // 转为像素的中心坐标
    point3 pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 输出256x256的图片，R在水平方向上递增，G在垂直方向上递增，B保持为0.0
    for (int j = 0; j < image_height; j++) // G
    {
        // 显示进度
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            // 求出每一个camera到像素中心的射线
            point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
            vec3 ray_dir = pixel - cam_pos;
            ray r(cam_pos, ray_dir);

            // 根据ray算出color
            color pixel_color = ray_color(r);
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n";
}

// 计算光线的颜色
color ray_color(const ray& r)
{
    // 如果光线与球体相交，则光线为红色
    if (is_sphere_hit(point3(0.0, 0.0, -1.0), 0.5, r))
        return color(1.0, 0.0, 0.0);

    // 以下为背景色
    // 根据r的y分量进行lerp
    vec3 r_uint = unit_vector(r.direction());
    // [-1.0, 1.0] ==> [0.0, 1.0]
    double a = 0.5 * (r_uint.y() + 1.0);

    // 返回蓝色与白色之间的lerp
    return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);;
}

// 判断ray是否和球相交
bool is_sphere_hit(const point3& center, double radius, const ray& r)
{
    point3 o = r.origin();
    vec3 d = r.direction();
    vec3 oc = center - o;

    // 转化为二项式求解未知数t的问题，推导过程参考derivation/sphere_hit.jpg
    double a = dot(d, d);
    double b = -2 * dot(d, oc);
    double c = dot(oc, oc) - radius * radius;

    double discriminant = b * b - 4 * a * c;

    // 如果判别式等于0，则ray和sphere只有一个交点
    // 如果判别式大于0，则ray和sphere有两个交点
    // 如果判别式小于0，则ray和sphere有没有交点
    if (discriminant >= 0)
        return true;

    return false;
}