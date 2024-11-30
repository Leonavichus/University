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

/**
 * Функция для применения бочкообразной дисторсии к изображению.
 *
 * @param img - Исходное изображение.
 */
void applyBarrelDistortion(Mat &img) {
    // Создаем сетку координат для изображения
    Mat xi, yi; // Матрицы координат X и Y
    vector<float> t_x, t_y;

    // Генерируем координаты столбцов и строк
    for (int i = 0; i < img.cols; i++) {
        t_x.push_back(float(i));
    }
    for (int i = 0; i < img.rows; i++) {
        t_y.push_back(float(i));
    }

    // Повторяем массивы для создания матриц координат
    repeat(Mat(t_x).reshape(1, 1), img.rows, 1, xi);  // Горизонтальная сетка (xi)
    repeat(Mat(t_y).reshape(1, 1).t(), 1, img.cols, yi);  // Вертикальная сетка (yi)

    // Смещение и нормализация координат
    double xmid = xi.cols / 2.0;  // Центр изображения по X
    double ymid = xi.rows / 2.0;  // Центр изображения по Y
    xi -= xmid; // Смещаем центр сетки к (0, 0)
    xi /= xmid; // Нормализуем координаты по ширине
    yi -= ymid; // Смещаем центр сетки к (0, 0)
    yi /= ymid; // Нормализуем координаты по высоте

    // Преобразуем в полярные координаты (радиус и угол)
    Mat r, theta;
    cartToPolar(xi, yi, r, theta);

    // Параметры для бочкообразной дисторсии
    double F3 = 0.1, F5 = 0.12; // Коэффициенты 3-го и 5-го порядка
    Mat r3, r5;

    pow(r, 3, r3); // Вычисляем r^3
    pow(r, 5, r5); // Вычисляем r^5

    // Применяем бочкообразное искажение: r' = r + F3 * r^3 + F5 * r^5
    r += r3 * F3;
    r += r5 * F5;

    // Преобразуем обратно в декартовы координаты
    Mat u, v;
    polarToCart(r, theta, u, v);

    // Восстанавливаем координаты и возвращаем их в исходный диапазон
    u *= xmid;
    u += xmid;
    v *= ymid;
    v += ymid;

    // Применяем ремаппинг с учетом новых координат
    Mat I_barrel;
    remap(img, I_barrel, u, v, INTER_LINEAR);

    // Отображаем результат
    namedWindow("Бочкообразная дисторсия", WINDOW_NORMAL);
    imshow("Бочкообразная дисторсия", I_barrel);
}

/**
 * Функция для применения подушкообразной дисторсии к изображению.
 *
 * @param img - Исходное изображение.
 */
void applyPillowDistortion(Mat &img) {
    // Создаем сетку координат для изображения
    Mat xi, yi; // Матрицы координат X и Y
    vector<float> t_x, t_y;

    // Генерируем координаты столбцов и строк
    for (int i = 0; i < img.cols; i++) {
        t_x.push_back(float(i));
    }
    for (int i = 0; i < img.rows; i++) {
        t_y.push_back(float(i));
    }

    // Повторяем массивы для создания матриц координат
    repeat(Mat(t_x).reshape(1, 1), img.rows, 1, xi);  // Горизонтальная сетка (xi)
    repeat(Mat(t_y).reshape(1, 1).t(), 1, img.cols, yi);  // Вертикальная сетка (yi)

    // Смещение и нормализация координат
    double xmid = xi.cols / 2.0;  // Центр изображения по X
    double ymid = xi.rows / 2.0;  // Центр изображения по Y
    xi -= xmid; // Смещаем центр сетки к (0, 0)
    xi /= xmid; // Нормализуем координаты по ширине
    yi -= ymid; // Смещаем центр сетки к (0, 0)
    yi /= ymid; // Нормализуем координаты по высоте

    // Преобразуем в полярные координаты (радиус и угол)
    Mat r, theta;
    cartToPolar(xi, yi, r, theta);

    // Параметр для подушкообразной дисторсии
    double F3 = -0.003; // Коэффициент 3-го порядка
    r += F3 * r.mul(r); // Применяем подушкообразное искажение: r' = r + F3 * r^2

    // Преобразуем обратно в декартовы координаты
    Mat u, v;
    polarToCart(r, theta, u, v);

    // Восстанавливаем координаты и возвращаем их в исходный диапазон
    u *= xmid;
    u += xmid;
    v *= ymid;
    v += ymid;

    // Применяем ремаппинг с учетом новых координат
    Mat I_distorted;
    remap(img, I_distorted, u, v, INTER_LINEAR);

    // Отображаем результат
    namedWindow("Подушкообразная дисторсия", WINDOW_NORMAL);
    imshow("Подушкообразная дисторсия", I_distorted);
}

/**
 * Основная функция программы.
 */
int main() {
    // Загрузка изображения
    Mat img = imread("background.png");
    if (img.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    // Отображаем исходное изображение
    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    imshow("Оригинальное изображение", img);

    // Применяем бочкообразную дисторсию
    applyBarrelDistortion(img);

    // Применяем подушкообразную дисторсию
    applyPillowDistortion(img);

    // Ожидание нажатия клавиши для завершения программы
    cout << "Нажмите 'Q' для выхода." << endl;
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }
    return 0;
}
