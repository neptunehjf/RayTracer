
# Software Raytracer

‌**概要**‌  
グラフィックスAPIを一切使わず、コンピュータグラフィックス理論と数学だけで実装したレイトレーサー。

GPUではなくCPU上で動作するソフトウェアレイトレーサー(Software Raytracer)。

‌**技術スタック**‌

-   ‌**プログラミング言語**‌: C++
-   ‌**理論基盤**‌: グラフィックス学、数学、物理、信号画像処理
-   ‌**ツール**‌: Viusal Studio2022、Photoshopなど

‌**プロジェクト構成**‌

-   ‌**referrence** - 参照資料（自分で数学式の導出プロセスも含む）
-   ‌**Dependency** - 依存ファイル
-   ‌**2.OutputImage** - PPM形式の画像出力
-   ‌**3.Vec3Class** - vec3とcolorのユーティリティクラス  
-   ‌**4.SendRay** - カメラからビューポートへの光線発射  
-   ‌**5.RayTraceSphere** - 光線と球体の交差判定
-   ‌**6.NormalAndMultiObject** - 法線計算と複数物体対応
-   ‌**7.CameraClass** - カメラ制御クラス
-   ‌**8.Antialiasing** - アンチエイリアシング
-   ‌**9.Diffuse** - Lambertianモデルによる拡散マテリアル
-   ‌**10.Metal** - 鏡面反射する金属材質 
-   ‌**11.Dielectrics** - スネルの法則によるガラス様材質
-   ‌**12.Positionable Camera** - カメラ位置調整機能
-   ‌**13.Defocus Blur** - 被写界深度効果 
-   ‌**14.Final Render** - 複数の拡散球/金属球/ガラス球を配置したシーン
-   ‌**x2.Motion Blur** - モーションブラー効果
-   ‌**x3.BVH** - AABB Bounding Boxによるレイトレ効率化
-   ‌**x4.Texture Mapping** - UVマッピングと手続き型テクスチャ生成
-   ‌**x5.Perlin Noise** -Perlinノイズによる自然テクスチャ生成 
-   ‌**x6.Quadrilaterals** - 四角形オブジェクト構築  
-   ‌**x7.Lights** - 光源システム導入 
-   ‌**x8.Instances** - オブジェクト変形機能とCornell Boxシーン
-   ‌**x9.Volumes** - ボリュームライティング効果 
-   ‌**x10.Final Scene** - 以上全機能統合テストシーン  
-   ‌**y2.Simple Monte Carlo** - Monte Carloサンプリングによる円周率の計算と、通常サンプリングと層化ジッターサンプリングの収束速度への影響比較
-   ‌**y3.One Dimensional Monte Carlo Integration** - 1次元関数のMonte Carlo積分計算と、異なるPDFサンプリング関数が収束速度に及ぼす影響の比較
-   ‌**y4.Monte Carlo Integration on the Sphere of Directions** - 球面方向上の積分計算
-   ‌**y6.Playing with Importance Sampling** - 重要度サンプリング準備のための陽関数化PDF
-   ‌**y7.Generating Random Directions** -  ランダム方向を生成し、各サンプリングpdfがcos³θ積分への収束性検証
-   ‌**y8.Orthonormal Bases** - 正規直交基底を用いて、z軸を法線として生成されたランダム方向を各法線方向に適用します
-   ‌**y9.Sampling Lights Directly** - 重点サンプリングを用いて散乱光線を光源方向に誘導（光源バイアスPDF）することでノイズを大幅に低減しますが、局所的な黒潰れを生じさせる問題があります
-   ‌**y10.Mixture Densities** - Lambertian分布PDFと光源バイアスPDFの重み付け合成により、光源バイアスPDF単独使用時の局所的な黒潰れ問題を改善
-   ‌**y12.Cleaning Up PDF Management** -  最終版 PDF管理モジュールの最適化を実装

※ コード中のコメントは一応日本語化していますが、自分用のメモとして中国語のコメントも多く残っています。悪しからずご了承ください。


‌**実行方法**‌  
-   画像が上書きされるのを防ぐため、実行前にプロジェクトディレクトリ内の既存画像を削除する必要があります
-   Visual Studio 2022で該当プロジェクトの`.sln`ファイルを開き、コンパイル後、VS 2022のデバッガーで実行

‌**成果の例**‌  
-   解像度：1200 x 675
-   ピクセル毎の‌サンプル数：500
-   ‌AABB Bounding Box最適化：有効
-   ‌重点サンプリング：無効
-   ‌レンダリング時間：秒
<img src="https://github.com/user-attachments/assets/db9a82b9-dc5d-40e6-b583-717d53215a77" width="800px" />
<br>
<br>
<br>

-   解像度：800 x 800
-   ピクセル毎の‌サンプル数：10000
-   ‌AABB Bounding Box最適化：有効
-   ‌重点サンプリング：無効
-   ‌レンダリング時間：24497秒
<img src="https://github.com/user-attachments/assets/0e843bb5-3f90-4a4b-a6c4-8baa2bb51936" width="800px" />
<br>
<br>
<br>

-   解像度：600 x 600
-   ピクセル毎の‌サンプル数：1000
-   ‌AABB Bounding Box最適化：無効
-   ‌重点サンプリング：有効
-   ‌レンダリング時間：371秒
<img src="https://github.com/user-attachments/assets/c29ab6b7-3d50-4be9-baf0-97b02d241f6d" width="800px" />
<br>
<br>
<br>

‌**連絡先**‌  
koalahjf@gmail.com
