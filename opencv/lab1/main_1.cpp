/*
1. Гистограммы. Выбрать произвольное слабоконтрастное изображение.
Выполнить выравнивание гистограммы и растяжение контраста,
использовать рассмотренные преобразования.
Сравнить полученные результаты.
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Загрузите изображение (слабоконтрастное)
    Mat I = imread("background.png", IMREAD_COLOR);
    if (I.empty()) {
        cerr << "Ошибка: не удалось загрузить изображение!" << endl;
        return -1;
    }

    // Преобразование в оттенки серого для упрощения
    Mat gray;
    cvtColor(I, gray, COLOR_BGR2GRAY);

    // 1. Выравнивание гистограммы
    Mat equalized;
    equalizeHist(gray, equalized);

    // 2. Растяжение контраста (масштабируем интенсивности на полный диапазон)
    Mat stretched;
    gray.convertTo(stretched, CV_32F, 1.0 / 255.0); // Нормализация значений в диапазон [0, 1]
    normalize(stretched, stretched, 0, 255, NORM_MINMAX); // Растяжение контраста
    stretched.convertTo(stretched, CV_8U); // Преобразуем обратно в 8-битное изображение

    // Отображение всех изображений
    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    resizeWindow("Оригинальное изображение", 800, 600);
    namedWindow("Выравнивание гистограммы", WINDOW_NORMAL);
    resizeWindow("Выравнивание гистограммы", 800, 600);
    namedWindow("Растяжение контраста", WINDOW_NORMAL);
    resizeWindow("Растяжение контраста", 800, 600);

    imshow("Оригинальное изображение", gray);
    imshow("Выравнивание гистограммы", equalized);
    imshow("Растяжение контраста", stretched);

    // Ожидание нажатия клавиши для закрытия окон
    while (true) {
        int key = waitKey(0);  // Ожидание нажатия любой клавиши
        if (key == 'q' || key == 'Q') {  // Проверка нажатия 'q' или 'Q'
            break;
        }
    }
    return 0;
}
