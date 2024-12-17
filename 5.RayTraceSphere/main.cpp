#include "vec3.h"
#include "color.h"
#include "ray.h"

color ray_color(const ray& r);

int main() {

    // Image �̶���߱�Ϊ16 : 9 
    double aspect_radio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) (image_width / aspect_radio); // ��Ϊ�ǽ��Ƽ��㣬�п�����0
    image_height = (image_height < 1) ? 1 : image_height;  // ��֤��С��1

    // Camera
    vec3 cam_pos = { 0.0, 0.0, 0.0 };
    double focal_length = 1.0; // ���࣬camera��viewport֮��ľ��룬Ŀǰ��Ϊ1.0
    double viewport_h = 2.0;
    double viewport_w = viewport_h * (double(image_width) / image_height); // ��Ϊimage_height�ǽ��Ƽ���ģ�����ֱ����aspect_radio

    // viewport UV ��camera����ϵ������
    vec3 u = vec3(viewport_w, 0.0, 0.0);
    vec3 v = vec3(0.0, -viewport_h, 0.0); // ��Ϊviewport�����ڵ�v�����camera����ϵy�����෴

    // �����ÿ��pixel��Ӧ��uv��С
    double pixel_x = viewport_w / image_width;
    double pixel_y = -viewport_h / image_height;

    // ����viewportԭ����camera����ϵ��λ�ã�viewportԭ�������Ͻǣ����������focal_length����xyƽ�棬���λ�ú�viewport�����غ�
    point3 viewport_zero = cam_pos - vec3(0.0, 0.0, focal_length) - u / 2 - v / 2;
    // תΪ���ص���������
    point3 pixel_zero = viewport_zero + vec3(0.5 * pixel_x, 0.5 * pixel_y, 0.0);

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // ���256x256��ͼƬ��R��ˮƽ�����ϵ�����G�ڴ�ֱ�����ϵ�����B����Ϊ0.0
    for (int j = 0; j < image_height; j++) // G
    {
        // ��ʾ����
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            // ���ÿһ��camera���������ĵ�����
            point3 pixel = pixel_zero + vec3(i * pixel_x, j * pixel_y, 0.0);
            vec3 ray_dir = pixel - cam_pos;
            ray r(cam_pos, ray_dir);

            // ����ray���color
            color pixel_color = ray_color(r);
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n";
}

// ������ߵ���ɫ
color ray_color(const ray& r)
{
    // ����r��y��������lerp
    vec3 r_uint = unit_vector(r.direction());
    // [-1.0, 1.0] ==> [0.0, 1.0]
    double a = 0.5 * (r_uint.y() + 1.0);

    // ��ɫ���ɫ֮��lerp
    color c = (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);

    return c;
}