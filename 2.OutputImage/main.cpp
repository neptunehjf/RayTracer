#include <iostream>

using namespace std;

int main() {

    int image_width = 256;
    int image_height = 256;

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // 256x256の画像を生成（R値は水平方向に増加、G値は垂直方向に増加、B値は常に0）
    for (int j = 0; j < image_height; j++) // G
    {
        // 進捗状況を表示
        clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush;

        for (int i = 0; i < image_width; i++) // R
        {
            double r = (double)i / (double)(image_width - 1);
            double g = (double)j / (double)(image_height - 1);
            double b = 0.0;

            // 0.0〜1.0の値を0〜255の整数に変換
            int ir = (int)(r * 255.999);
            int ig = (int)(g * 255.999);
            int ib = (int)(b * 255.999);

            // ピクセルデータを出力（output.ppmにリダイレクト）
            cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    clog << "\rDone.                 \n";
}