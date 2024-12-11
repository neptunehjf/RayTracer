#include <iostream>

using namespace std;

int main() {

    int image_width = 256;
    int image_height = 256;

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // ���256x256��ͼƬ��R��ˮƽ�����ϵ�����G�ڴ�ֱ�����ϵ�����B����Ϊ0.0
    for (int j = 0; j < image_height; j++) // G
    {
        // ��ʾ����
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            double r = (double)i / (double)(image_width - 1);
            double g = (double)j / (double)(image_height - 1);
            double b = 0.0;

            // 0.0~1.0 ת�� 0~255
            int ir = (int)(r * 255.999);
            int ig = (int)(g * 255.999);
            int ib = (int)(b * 255.999);

            // �����ض���output.ppm��ֱ����cout�������
            cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    clog << "\rDone.                 \n";
}