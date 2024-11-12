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
    src.convertTo(dst, CV_32F, 1.0 / 255.0);
    pow(dst, gamma, dst);
    dst.convertTo(dst, CV_8U, 255);
    return dst;
}

// Функция для преобразования по закону Рэлея
cv::Mat rayleighTransform(const cv::Mat& src, double sigma) {
    cv::Mat dst;
    src.convertTo(dst, CV_32F);
    dst = dst.mul(dst) / (2 * sigma * sigma);
    cv::exp(-dst, dst);
    dst = 1 - dst;
    return dst;
}

// Функция для гиперболического преобразования
cv::Mat hyperbolicTransform(const cv::Mat& src, double alpha) {
    cv::Mat dst;
    src.convertTo(dst, CV_32F);
    dst = alpha * dst;
    cv::Mat expPos, expNeg;
    cv::exp(dst, expPos);          // e^(alpha * dst)
    cv::exp(-dst, expNeg);         // e^(-alpha * dst)
    dst = (expPos - expNeg) / (expPos + expNeg);
    return dst;
}

int main() {
    // Загрузка изображения
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

    // 2. Растяжение динамического диапазона
    Mat stretched;
    normalize(gray, stretched, 0, 255, NORM_MINMAX);

    // 3. Экспоненциальное преобразование
    Mat exponential = exponentialTransform(gray, 2.0);

    // 4. Преобразование по закону Рэлея
    Mat rayleigh = rayleighTransform(gray, 0.5);

    // 5. Преобразование по закону степени (2/3)
    Mat power;
    gray.convertTo(power, CV_32F, 1.0 / 255.0);
    pow(power, 2.0 / 3.0, power);
    power.convertTo(power, CV_8U, 255);

    // 6. Гиперболическое преобразование
    Mat hyperbolic = hyperbolicTransform(gray, 5.0);

    // Отображение всех изображений
    namedWindow("Оригинальное изображение", WINDOW_NORMAL);
    resizeWindow("Оригинальное изображение", 800, 600);
    imshow("Оригинальное изображение", gray);

    namedWindow("Выравнивание гистограммы", WINDOW_NORMAL);
    resizeWindow("Выравнивание гистограммы", 800, 600);
    imshow("Выравнивание гистограммы", equalized);

    namedWindow("Растяжение динамического диапазона", WINDOW_NORMAL);
    resizeWindow("Растяжение динамического диапазона", 800, 600);
    imshow("Растяжение динамического диапазона", stretched);

    namedWindow("Экспоненциальное преобразование", WINDOW_NORMAL);
    resizeWindow("Экспоненциальное преобразование", 800, 600);
    imshow("Экспоненциальное преобразование", exponential);

    namedWindow("Преобразование по закону Рэлея", WINDOW_NORMAL);
    resizeWindow("Преобразование по закону Рэлея", 800, 600);
    imshow("Преобразование по закону Рэлея", rayleigh);

    namedWindow("Преобразование по закону степени 2/3", WINDOW_NORMAL);
    resizeWindow("Преобразование по закону степени 2/3", 800, 600);
    imshow("Преобразование по закону степени 2/3", power);

    namedWindow("Гиперболическое преобразование", WINDOW_NORMAL);
    resizeWindow("Гиперболическое преобразование", 800, 600);
    imshow("Гиперболическое преобразование", hyperbolic);

    // Ожидание нажатия клавиши для закрытия окон
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }
    return 0;
}
