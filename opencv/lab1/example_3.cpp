#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Функция для рисования графика
template <typename T>
void DrawGraph(Mat &img, const Mat &data, const Scalar &color, double maxVal) {
    for (int i = 0; i < data.rows; i++) {
        int height = cvRound(data.at<T>(i) * img.rows / maxVal);
        line(img, Point(i, img.rows - 1), Point(i, img.rows - height), color, 1, 8, 0);
    }
}

int main() {
    // Загрузка изображения
    Mat I = imread("ava2.jpg", IMREAD_COLOR);  // Используем IMREAD_COLOR для цветного изображения
    if (I.empty()) {
        cerr << "Ошибка: не удалось загрузить изображение!" << endl;
        return -1;
    }

    // Создание массива для проекции
    Mat Proj(I.rows, 1, CV_32F);
    bool bw = (I.channels() == 1);  // Проверка на монохромное изображение

    // Подсчет суммы интенсивности по строкам
    for (int i = 0; i < I.rows; i++) {
        double sum = 0;
        for (int j = 0; j < I.cols; j++) {
            if (bw)
                sum += I.at<uchar>(i, j);  // Для монохромного изображения
            else {
                const Vec3b &pix = I.at<Vec3b>(i, j);  // Для цветного изображения
                sum += pix[0] + pix[1] + pix[2];  // Суммируем значения каналов
            }
        }
        Proj.at<float>(i) = float(sum);  // Записываем результат в массив проекции
    }

    // Нормализация проекции
    Proj /= 256 * I.channels();  // Делим на 256 (максимальная интенсивность) и количество каналов

    // Поиск минимальных и максимальных значений проекции
    double ProjMin, ProjMax;
    minMaxLoc(Proj, &ProjMin, &ProjMax);

    // Создание изображения для графика
    Mat ProjI(256, I.rows, CV_8UC3, Scalar(255, 255, 255));  // Белый фон для графика

    // Рисование графика проекции
    DrawGraph<float>(ProjI, Proj, Scalar(0, 0, 0), ProjMax);

    // Транспонирование и переворот изображения (для отображения вертикально)
    transpose(ProjI, ProjI);
    flip(ProjI, ProjI, 1);

    // Создание окна с возможностью изменения размера
    namedWindow("Проекция по оси Oy", WINDOW_NORMAL);
    resizeWindow("Проекция по оси Oy", 800, 600);  // Устанавливаем размер окна

    // Отображение результата
    imshow("Проекция по оси Oy", ProjI);

    // Ожидание нажатия клавиши для закрытия окна
    while (true) {
        int key = waitKey(0);  // Ожидание нажатия любой клавиши
        if (key == 'q' || key == 'Q') {  // Проверка нажатия 'q' или 'Q'
            break;
        }
    }

    return 0;
}
