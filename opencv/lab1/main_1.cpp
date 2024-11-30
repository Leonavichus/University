/*
1. Гистограммы. Выбрать произвольное слабоконтрастное изображение.
Выполнить выравнивание гистограммы и растяжение контраста,
использовать рассмотренные преобразования.
Сравнить полученные результаты.
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

// Функция для экспоненциального преобразования
Mat exponentialTransform(const Mat& src, double gamma) {
    Mat dst;
    src.convertTo(dst, CV_32F, 1.0 / 255.0);  // Нормализация изображения к диапазону [0, 1]
    pow(dst, gamma, dst);                     // Применение экспоненциального преобразования
    dst.convertTo(dst, CV_8U, 255);           // Возвращение к диапазону [0, 255] и 8-битному формату
    return dst;
}

// Функция для гиперболического преобразования
Mat hyperbolicTransform(const Mat& src, double alpha) {
    Mat dst;
    src.convertTo(dst, CV_32F); // Конвертация в 32-битный формат
    dst = alpha * dst;          // Масштабирование с коэффициентом alpha
    Mat expPos, expNeg;
    exp(dst, expPos);           // Вычисление e^(alpha * dst)
    exp(-dst, expNeg);          // Вычисление e^(-alpha * dst)
    dst = (expPos - expNeg) / (expPos + expNeg); // Гиперболическое преобразование
    return dst;
}

// Функция для преобразования по закону степени (2/3)
Mat powerTransform(const Mat& src, double power) {
    Mat dst;
    src.convertTo(dst, CV_32F, 1.0 / 255.0); // Нормализация к диапазону [0, 1]
    pow(dst, power, dst);                    // Преобразование по степени
    dst.convertTo(dst, CV_8U, 255);          // Обратная конвертация к диапазону [0, 255]
    return dst;
}

int main() {
    // Загрузка изображения
    Mat I = imread("background.png", IMREAD_COLOR);
    if (I.empty()) {
        cerr << "Ошибка: не удалось загрузить изображение!" << endl;
        return -1;
    }

    // Преобразование в оттенки серого
    Mat gray;
    cvtColor(I, gray, COLOR_BGR2GRAY);

    // 1. Выравнивание гистограммы
    Mat equalized;
    equalizeHist(gray, equalized);

    // 2. Растяжение динамического диапазона
    Mat stretched;
    normalize(gray, stretched, 0, 255, NORM_MINMAX);

    // 3. Экспоненциальное преобразование
    Mat exponential = exponentialTransform(gray, 2.0);

    // 4. Преобразование по закону степени (2/3)
    Mat power = powerTransform(gray, 2.0 / 3.0);

    // 5. Гиперболическое преобразование
    Mat hyperbolic = hyperbolicTransform(gray, 5.0);

    // Отображение всех изображений
    namedWindow("Оригинальное изображение", WINDOW_NORMAL); // Создание окна для оригинального изображения
    resizeWindow("Оригинальное изображение", 800, 600);    // Изменение размера окна
    imshow("Оригинальное изображение", gray);               // Показ изображения

    namedWindow("Выравнивание гистограммы", WINDOW_NORMAL);
    resizeWindow("Выравнивание гистограммы", 800, 600);
    imshow("Выравнивание гистограммы", equalized);          // Показ изображения после выравнивания гистограммы

    namedWindow("Растяжение динамического диапазона", WINDOW_NORMAL);
    resizeWindow("Растяжение динамического диапазона", 800, 600);
    imshow("Растяжение динамического диапазона", stretched); // Показ изображения после растяжения диапазона

    namedWindow("Экспоненциальное преобразование", WINDOW_NORMAL);
    resizeWindow("Экспоненциальное преобразование", 800, 600);
    imshow("Экспоненциальное преобразование", exponential);  // Показ изображения после экспоненциального преобразования

    namedWindow("Преобразование по закону степени 2/3", WINDOW_NORMAL);
    resizeWindow("Преобразование по закону степени 2/3", 800, 600);
    imshow("Преобразование по закону степени 2/3", power);   // Показ изображения после преобразования по степени 2/3

    namedWindow("Гиперболическое преобразование", WINDOW_NORMAL);
    resizeWindow("Гиперболическое преобразование", 800, 600);
    imshow("Гиперболическое преобразование", hyperbolic);    // Показ изображения после гиперболического преобразования

    // Ожидание нажатия клавиши для закрытия окон
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }
    return 0;
}
