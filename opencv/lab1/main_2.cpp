/*
2. Проекции. Выбрать произвольное изображение, содержащее монотонные области и выделяющиеся объекты.
Произвести построение проекций изображения на вертикальную и горизонтальную оси.
Определить границы областей объектов.
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat I = imread("ava2.jpg", IMREAD_GRAYSCALE);

    // Проверка успешности загрузки изображения
    if (I.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    // Вертикальная проекция (на Oy)
    Mat proj_oy = Mat::zeros(I.rows, 1, CV_32F);  // Создаем матрицу для вертикальной проекции
    for (int i = 0; i < I.rows; i++) {
        float sum = 0;
        for (int j = 0; j < I.cols; j++) {
            sum += I.at<uchar>(i, j);  // Суммируем интенсивности всех пикселей в строке i
        }
        proj_oy.at<float>(i) = sum;  // Сохраняем сумму интенсивностей в матрице вертикальной проекции
    }

    // Горизонтальная проекция (на Ox)
    Mat proj_ox = Mat::zeros(1, I.cols, CV_32F);  // Создаем матрицу для горизонтальной проекции
    for (int j = 0; j < I.cols; j++) {
        float sum = 0;
        for (int i = 0; i < I.rows; i++) {
            sum += I.at<uchar>(i, j);  // Суммируем интенсивности всех пикселей в столбце j
        }
        proj_ox.at<float>(0, j) = sum;  // Сохраняем сумму интенсивностей в матрице горизонтальной проекции
    }

    // Нормализация проекций для отображения
    double proj_oy_max, proj_ox_max;
    minMaxLoc(proj_oy, nullptr, &proj_oy_max);  // Находим максимальное значение в вертикальной проекции
    minMaxLoc(proj_ox, nullptr, &proj_ox_max);  // Находим максимальное значение в горизонтальной проекции

    // Создаем изображения для отображения проекций
    Mat proj_oy_img = Mat::ones(256, I.cols, CV_8UC3) * 255;  // Белое изображение для вертикальной проекции
    Mat proj_ox_img = Mat::ones(I.rows, 256, CV_8UC3) * 255;  // Белое изображение для горизонтальной проекции

    // Нормализуем значения проекций для графического отображения
    for (int i = 0; i < proj_oy.rows; i++) {
        int normalized_value = cvRound(proj_oy.at<float>(i) * proj_oy_img.cols / proj_oy_max);  // Нормализуем значение к ширине изображения
        line(proj_oy_img, Point(0, i), Point(normalized_value, i), Scalar(0, 0, 0), 1);  // Рисуем линию, отображающую вертикальную проекцию
    }

    for (int j = 0; j < proj_ox.cols; j++) {
        int normalized_value = cvRound(proj_ox.at<float>(0, j) * proj_ox_img.rows / proj_ox_max);  // Нормализуем значение к высоте изображения
        line(proj_ox_img, Point(j, 0), Point(j, normalized_value), Scalar(0, 0, 0), 1);  // Рисуем линию, отображающую горизонтальную проекцию
    }

    // Отображение всех изображений
    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    resizeWindow("Оригинальное изображение", 800, 600);

    namedWindow("Вертикальная проекция", WINDOW_NORMAL);
    resizeWindow("Вертикальная проекция", 800, 600);

    namedWindow("Горизонтальная проекция", WINDOW_NORMAL);
    resizeWindow("Горизонтальная проекция", 800, 600);

    imshow("Оригинальное изображение", I);           // Показ оригинального изображения
    imshow("Вертикальная проекция", proj_oy_img);    // Показ вертикальной проекции
    imshow("Горизонтальная проекция", proj_ox_img);  // Показ горизонтальной проекции

    // Ожидание нажатия клавиши для закрытия окон
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
