/*
1. Простейшие геометрические преобразования. Выбрать произвольное изображение.
Выполнить над ним линейные и нелинейные преобразования (конформные, аффинные и проективные отображения)
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
    // Загрузка изображения
    string imagePath = "background.png";
    Mat img = imread(imagePath);
    if (img.empty()) {
        cerr << "Ошибка: Не удалось загрузить изображение по пути: " << imagePath << endl;
        return -1;
    }

    // 1. Конформные преобразования
    // -------------------------------------

    // 1.1 Сдвиг
    Mat T_shift = (Mat_<double>(2, 3) <<
        1, 0, 50,
        0, 1, 30);
    Mat img_shift;
    warpAffine(img, img_shift, T_shift, img.size());

    // 1.2 Однородное масштабирование
    Mat T_scale = (Mat_<double>(2, 3) <<
        1.5, 0, 0,
        0, 1.5, 0);
    Mat img_scale;
    warpAffine(img, img_scale, T_scale, Size(int(img.cols * 1.5), int(img.rows * 1.5)));

    // 1.3 Поворот
    double angle = 45.0;
    Point2f center(img.cols / 2.0, img.rows / 2.0);
    Mat T_rotate = getRotationMatrix2D(center, angle, 1.0);
    Mat img_rotate;
    warpAffine(img, img_rotate, T_rotate, img.size());

    // 1.4 Отражение
    Mat T_reflect = (Mat_<double>(2, 3) <<
        1, 0, 0,
        0, -1, img.rows);
    Mat img_reflect;
    warpAffine(img, img_reflect, T_reflect, img.size());

    // 2. Аффинные преобразования
    // -------------------------------------

    // 2.1 Скос
    Mat T_shear = (Mat_<double>(2, 3) <<
        1, 0.5, 0,
        0.2, 1, 0);
    Mat img_shear;
    warpAffine(img, img_shear, T_shear, Size(img.cols * 2, img.rows * 2));

    // 2.2 Неоднородное масштабирование
    Mat T_nonscale = (Mat_<double>(2, 3) <<
        2.0, 0, 0,
        0, 0.5, 0);
    Mat img_nonscale;
    warpAffine(img, img_nonscale, T_nonscale, Size(img.cols * 2, img.rows / 2));

    // 3. Нелинейные преобразования
    // -------------------------------------

    // 3.1 Проективное преобразование
    Point2f srcPoints[4] = {
        Point2f(0, 0),
        Point2f(img.cols - 1, 0),
        Point2f(img.cols - 1, img.rows - 1),
        Point2f(0, img.rows - 1)
    };
    Point2f dstPoints[4] = {
        Point2f(30, 30),
        Point2f(img.cols - 30, 50),
        Point2f(img.cols - 50, img.rows - 30),
        Point2f(50, img.rows - 50)
    };
    Mat T_projective = getPerspectiveTransform(srcPoints, dstPoints);
    Mat img_projective;
    warpPerspective(img, img_projective, T_projective, img.size());

    // 3.2 Кусочно-линейное отображение
    Mat img_piecewise = img.clone();
    Mat rightHalf = img_piecewise(Rect(img.cols / 2, 0, img.cols / 2, img.rows));
    Mat T_piecewise = (Mat_<double>(2, 3) <<
        2.0, 0, 0,
        0, 1.0, 0);
    warpAffine(rightHalf, rightHalf, T_piecewise, Size(rightHalf.cols * 2, rightHalf.rows));

    // 3.3 Полиномиальное отображение
    Mat img_polynomial;
    const double T[2][6] = {
        {0, 1, 0, 0.00001, 0.002, 0.002},
        {0, 0, 1, 0, 0, 0}
    };

    if (img.depth() == CV_8U) {
        img.convertTo(img_polynomial, CV_32F, 1.0 / 255);
    } else {
        img_polynomial = img.clone();
    }

    vector<Mat> channels;
    split(img_polynomial, channels);

    for (int k = 0; k < channels.size(); k++) {
        Mat transformedChannel = Mat::zeros(channels[k].size(), channels[k].type());
        for (int x = 0; x < channels[k].cols; x++) {
            for (int y = 0; y < channels[k].rows; y++) {
                int newX = int(round(T[0][0] + x * T[0][1] + y * T[0][2] +
                                     x * x * T[0][3] + x * y * T[0][4] + y * y * T[0][5]));
                int newY = int(round(T[1][0] + x * T[1][1] + y * T[1][2] +
                                     x * x * T[1][3] + x * y * T[1][4] + y * y * T[1][5]));
                if (newX >= 0 && newX < channels[k].cols && newY >= 0 && newY < channels[k].rows) {
                    transformedChannel.at<float>(newY, newX) = channels[k].at<float>(y, x);
                }
            }
        }
        channels[k] = transformedChannel;
    }

    merge(channels, img_polynomial);
    img_polynomial.convertTo(img_polynomial, CV_8U, 255);

    // 3.4 Синусоидальное искажение
    Mat u = Mat::zeros(img.rows, img.cols, CV_32F);
    Mat v = Mat::zeros(img.rows, img.cols, CV_32F);
    for (int x = 0; x < img.cols; x++) {
        for (int y = 0; y < img.rows; y++) {
            u.at<float>(y, x) = float(x + 20 * sin(2 * M_PI * y / 90));
            v.at<float>(y, x) = float(y);
        }
    }
    Mat img_sinusoid;
    remap(img, img_sinusoid, u, v, INTER_LINEAR);

    // Отображение всех результатов
    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    imshow("Оригинальное изображение", img);

    namedWindow("Сдвиг", WINDOW_NORMAL);
    imshow("Сдвиг", img_shift);

    namedWindow("Однородное масштабирование", WINDOW_NORMAL);
    imshow("Однородное масштабирование", img_scale);

    namedWindow("Поворот", WINDOW_NORMAL);
    imshow("Поворот", img_rotate);

    namedWindow("Отражение (Ox)", WINDOW_NORMAL);
    imshow("Отражение (Ox)", img_reflect);

    namedWindow("Скос", WINDOW_NORMAL);
    imshow("Скос", img_shear);

    namedWindow("Неоднородное масштабирование", WINDOW_NORMAL);
    imshow("Неоднородное масштабирование", img_nonscale);

    namedWindow("Проективное преобразование", WINDOW_NORMAL);
    imshow("Проективное преобразование", img_projective);

    namedWindow("Кусочно-линейное отображение", WINDOW_NORMAL);
    imshow("Кусочно-линейное отображение", img_piecewise);

    namedWindow("Полиномиальное отображение", WINDOW_NORMAL);
    imshow("Полиномиальное отображение", img_polynomial);

    namedWindow("Синусоидальное искажение", WINDOW_NORMAL);
    imshow("Синусоидальное искажение", img_sinusoid);

    // Ожидание нажатия клавиши
    cout << "Нажмите 'Q' для выхода." << endl;
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
