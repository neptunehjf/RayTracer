#include <iostream>

using namespace std;

int main() {

    int image_width = 256;
    int image_height = 256;

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 输出256x256的图片，R在水平方向上递增，G在垂直方向上递增，B保持为0.0
    for (int j = 0; j < image_height; j++) // G
    {
        // 显示进度
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            double r = (double)i / (double)(image_width - 1);
            double g = (double)j / (double)(image_height - 1);
            double b = 0.0;

            // 0.0~1.0 转成 0~255
            int ir = (int)(r * 255.999);
            int ig = (int)(g * 255.999);
            int ib = (int)(b * 255.999);

            // 配置重定向到output.ppm，直接用cout输出即可
            cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    clog << "\rDone.                 \n";
}