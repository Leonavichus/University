/*
2. Коррекция дисторсии.
Выбрать произвольное изображение либо с подушкообразной, либо с бочкообразной дисторсией.
Выполнить коррекцию изображения.
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

void applyBarrelDistortion(Mat &img) {
    // Создаем сетку координат для изображения
    Mat xi, yi;
    vector<float> t_x, t_y;

    for (int i = 0; i < img.cols; i++) {
        t_x.push_back(float(i));
    }
    for (int i = 0; i < img.rows; i++) {
        t_y.push_back(float(i));
    }

    repeat(Mat(t_x).reshape(1, 1), img.rows, 1, xi);
    repeat(Mat(t_y).reshape(1, 1).t(), 1, img.cols, yi);

    // Смещение и нормализация сетки
    double xmid = xi.cols / 2.0;
    double ymid = xi.rows / 2.0;
    xi -= xmid;
    xi /= xmid;
    yi -= ymid;
    yi /= ymid;

    // Преобразуем в полярные координаты
    Mat r, theta;
    cartToPolar(xi, yi, r, theta);

    // Параметры бочкообразной дисторсии (5-й порядок)
    double F3 = 0.1, F5 = 0.12;
    Mat r3, r5;
    pow(r, 3, r3); // r3 = r^3
    pow(r, 5, r5); // r5 = r^5

    // Применяем дисторсию
    r += r3 * F3;
    r += r5 * F5;

    // Преобразуем обратно в декартовы координаты
    Mat u, v;
    polarToCart(r, theta, u, v);

    // Нормализуем и смещаем координаты
    u *= xmid;
    u += xmid;
    v *= ymid;
    v += ymid;

    // Выполняем ремаппинг изображения с искажением
    Mat I_barrel;
    remap(img, I_barrel, u, v, INTER_LINEAR);

    // Отображаем результат
    namedWindow("Бочкообразная дисторсия", WINDOW_NORMAL);
    imshow("Бочкообразная дисторсия", I_barrel);
}

void applyPillowDistortion(Mat &img) {
    // Создаем сетку координат для изображения
    Mat xi, yi;
    vector<float> t_x, t_y;

    for (int i = 0; i < img.cols; i++) {
        t_x.push_back(float(i));
    }
    for (int i = 0; i < img.rows; i++) {
        t_y.push_back(float(i));
    }

    repeat(Mat(t_x).reshape(1, 1), img.rows, 1, xi);
    repeat(Mat(t_y).reshape(1, 1).t(), 1, img.cols, yi);

    // Смещение и нормализация сетки
    double xmid = xi.cols / 2.0;
    double ymid = xi.rows / 2.0;
    xi -= xmid;
    xi /= xmid;
    yi -= ymid;
    yi /= ymid;

    // Преобразуем в полярные координаты
    Mat r, theta;
    cartToPolar(xi, yi, r, theta);

    // Параметры подушкообразной дисторсии (3-й порядок)
    double F3 = -0.003;
    r += F3 * r.mul(r); // r + F3 * r^2

    // Преобразуем обратно в декартовы координаты
    Mat u, v;
    polarToCart(r, theta, u, v);

    // Нормализуем и смещаем координаты
    u *= xmid;
    u += xmid;
    v *= ymid;
    v += ymid;

    // Выполняем ремаппинг изображения с искажением
    Mat I_distorted;
    remap(img, I_distorted, u, v, INTER_LINEAR);

    // Отображаем результат
    namedWindow("Подушкообразная дисторсия", WINDOW_NORMAL);
    imshow("Подушкообразная дисторсия", I_distorted);
}

int main() {
    // Загрузка изображения
    Mat img = imread("background.png"); // Укажите путь к изображению
    if (img.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    imshow("Оригинальное изображение", img);

    // Применяем бочкообразную дисторсию
    applyBarrelDistortion(img);

    // Применяем подушкообразную дисторсию
    applyPillowDistortion(img);

    // Ожидание нажатия клавиши
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }
    return 0;
}
