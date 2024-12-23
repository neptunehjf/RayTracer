#include "common.h"
#include "hittable_list.h"
#include "sphere.h"

color ray_color(const ray& r, const hittable_list& scene);
double hit_sphere(const point3& center, double radius, const ray& r);

int main() {

    // Image �̶���߱�Ϊ16 : 9 
    double aspect_radio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) (image_width / aspect_radio); // ��Ϊ�ǽ��Ƽ��㣬�п�����0
    image_height = (image_height < 1) ? 1 : image_height;  // ��֤��С��1

    // Scene
    hittable_list scene;

    // ע������point3(0.0, 0.0, -1.0)����ֵ
    // ����sphere���캯�������Ҫô��const point3&��Ҫô��point&&��������뱨��
    scene.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    scene.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

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
            color pixel_color = ray_color(r, scene);
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n";
}

// ������ߵ���ɫ
color ray_color(const ray& r, const hittable_list& scene)
{
    // ��
    point3 center(0.0, 0.0, -1.0);
    double radius = 0.5;

    // ���ع��������彻��ķ���
    hit_record rec;
    if (scene.hit(r, 0.0, inf, rec))
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
    return (1 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);;
}

// ���ray��sphere����Ľ����Ӧ��tֵ
double hit_sphere(const point3& center, double radius, const ray& r)
{
    point3 o = r.origin();
    vec3 d = r.direction();
    vec3 oc = center - o;

    // ת��Ϊ����ʽ���δ֪��t�����⣬�Ƶ����̲ο�derivation/sphere_hit.jpg
    double a = d.length_squared();
    double h = dot(d, oc);
    double c = oc.length_squared() - radius * radius;

    double discriminant = h * h - a * c;

    // ����б�ʽ����0����ray��sphereֻ��һ������
    // ����б�ʽ����0����ray��sphere����������
    // ����б�ʽС��0����ray��sphere��û�н���
    if (discriminant >= 0)
        return (h - sqrt(discriminant)) / a; //��t

    // ���޽��㣬���ظ�ֵ
    return -1.0;
}