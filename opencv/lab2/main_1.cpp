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
    Mat img = imread("background.png"); // Укажите путь к изображению
    if (img.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    // 1. Конформные преобразования
    // 1.1 Сдвиг
    Mat T_shift = (Mat_<double>(2, 3) <<
        1, 0, 50,  // Сдвиг на 50 пикселей по оси x
        0, 1, 30); // Сдвиг на 30 пикселей по оси y
    Mat img_shift;
    warpAffine(img, img_shift, T_shift, img.size());

    // 1.2 Однородное масштабирование
    Mat T_scale = (Mat_<double>(2, 3) <<
        1.5, 0, 0,  // Увеличение по x в 1.5 раза
        0, 1.5, 0); // Увеличение по y в 1.5 раза
    Mat img_scale;
    warpAffine(img, img_scale, T_scale, Size(int(img.cols * 1.5), int(img.rows * 1.5)));

    // 1.3 Поворот
    double angle = 45.0; // Угол в градусах
    Point2f center(img.cols / 2.0, img.rows / 2.0); // Центр изображения
    Mat T_rotate = getRotationMatrix2D(center, angle, 1.0); // Поворот без изменения масштаба
    Mat img_rotate;
    warpAffine(img, img_rotate, T_rotate, img.size());

    // 1.4 Отражение (относительно оси Ox)
    Mat T_reflect = (Mat_<double>(2, 3) <<
        1, 0, 0,    // Не меняем по оси X
        0, -1, img.rows - 1); // Отражение по оси Y
    Mat img_reflect;
    warpAffine(img, img_reflect, T_reflect, img.size());

    // 2. Аффинные преобразования
    // 2.1 Скос
    Mat T_shear = (Mat_<double>(2, 3) <<
        1, 0.5, 0,  // Скос по оси x
        0.2, 1, 0); // Скос по оси y
    Mat img_shear;
    warpAffine(img, img_shear, T_shear, Size(img.cols * 2, img.rows * 2));

    // 2.2 Неоднородное масштабирование
    Mat T_nonscale = (Mat_<double>(2, 3) <<
        2.0, 0, 0,  // Увеличение по x в 2 раза
        0, 0.5, 0); // Уменьшение по y в 2 раза
    Mat img_nonscale;
    warpAffine(img, img_nonscale, T_nonscale, Size(img.cols * 2, img.rows / 2));

    // 3. Нелинейные преобразования
    // 3.1. Проективное преобразование
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

    // 3.2. Кусочно-линейное отображение (Piecewise Linear Transformation)
    double stretch = 2.0;
    Mat T_piecewise = (Mat_<double>(2, 3) <<
            stretch, 0, 0,    // Увеличение по оси x
            0, 1, 0);         // Без изменений по оси y
    Mat img_piecewise = img.clone();
    Mat img_right = img_piecewise(Rect(img.cols / 2, 0, img.cols / 2, img.rows)); // Правая половина изображения
    warpAffine(img_right, img_right, T_piecewise, Size(img.cols / 2, img.rows)); // Применяем растяжение

    // 3.3. Полиномиальное отображение (Polynomial Transformation)
    // Модификация координат точек с использованием полиномиальной функции
    const double T[2][6] = {
            {0, 1, 0, 0.00001, 0.002, 0.002},
            {0, 0, 1, 0, 0, 0}
        };
    Mat img_polynomial;
    if (img.depth() == CV_8U)
        img.convertTo(img_polynomial, CV_32F, 1.0 / 255);
    else
        img_polynomial = img;

    // Разделение на каналы
    vector<Mat> img_BGR;
    split(img_polynomial, img_BGR);

    // Применяем полиномиальное отображение
    for (int k = 0; k < img_BGR.size(); k++) {
        Mat img_channel = Mat::zeros(img_BGR[k].rows, img_BGR[k].cols, img_BGR[k].type());
        for (int x = 0; x < img_BGR[k].cols; x++) {
            for (int y = 0; y < img_BGR[k].rows; y++) {
                int xnew = int(round(T[0][0] + x * T[0][1] + y * T[0][2] +
                                        x * x * T[0][3] + x * y * T[0][4] + y * y * T[0][5]));
                int ynew = int(round(T[1][0] + x * T[1][1] + y * T[1][2] +
                                        x * x * T[1][3] + x * y * T[1][4] + y * y * T[1][5]));
                if (xnew >= 0 && xnew < img_BGR[k].cols && ynew >= 0 && ynew < img_BGR[k].rows)
                    img_channel.at<float>(ynew, xnew) = img_BGR[k].at<float>(y, x);
            }
        }
        img_BGR[k] = img_channel;
    }

    // Объединяем обратно каналы
    merge(img_BGR, img_polynomial);

    // Преобразуем обратно в uint8, если нужно
    if (img.depth() == CV_8U)
        img_polynomial.convertTo(img_polynomial, CV_8U, 255);

    // 3.4. Синусоидальное искажение
    Mat u = Mat::zeros(img.rows, img.cols, CV_32F);
    Mat v = Mat::zeros(img.rows, img.cols, CV_32F);
    for (int x = 0; x < img.cols; x++) {
        for (int y = 0; y < img.rows; y++) {
            u.at<float>(y, x) = float(x + 20 * sin(2 * M_PI * y / 90)); // Синусоидальное искажение по оси x
            v.at<float>(y, x) = float(y); // Без изменений по оси y
        }
    }

    Mat img_sinusoid;
    remap(img, img_sinusoid, u, v, INTER_LINEAR);

    // Отображение результатов
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
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
