/*
3. Профили. Выбрать произвольное изображение, содержащие штрих-код.
Выполнить построение профиля изображения вдоль штрих-кода.
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat I = imread("images.jpeg", IMREAD_GRAYSCALE);

    // Проверка успешности загрузки изображения
    if (I.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    // Бинаризация изображения с применением автоматического порогового значения Otsu
    Mat binaryImage;
    threshold(I, binaryImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Определение центральной линии для профиля штрих-кода
    int profileLine = I.rows / 2; // Высота линии профиля - середина изображения
    Mat profile = Mat::zeros(1, I.cols, CV_32F); // Массив для хранения значений профиля

    // Суммируем интенсивности пикселей вдоль горизонтальной области вокруг центральной линии
    int profileBand = 10; // Полу-ширина зоны, используемой для усреднения
    for (int j = 0; j < I.cols; j++) {
        float sum = 0;
        for (int i = profileLine - profileBand; i <= profileLine + profileBand; i++) {
            sum += binaryImage.at<uchar>(i, j); // Суммируем интенсивности пикселей
        }
        profile.at<float>(0, j) = sum / (2 * profileBand + 1); // Считаем среднюю интенсивность
    }

    // Нормализация профиля для отображения
    normalize(profile, profile, 0, 255, NORM_MINMAX);

    // Создаем изображение для графического отображения профиля
    Mat profileImg = Mat::zeros(100, I.cols, CV_8UC3); // Пустое изображение для профиля
    for (int j = 0; j < I.cols; j++) {
        int intensity = cvRound(profile.at<float>(0, j)); // Преобразуем значение профиля в целое число
        line(profileImg, Point(j, profileImg.rows), Point(j, profileImg.rows - intensity), Scalar(255, 255, 255), 1); // Рисуем линию интенсивности
    }

    // Отображение исходного изображения и профиля
    namedWindow("Изображение с штрих-кодом", WINDOW_NORMAL);
    resizeWindow("Изображение с штрих-кодом", 800, 600);
    namedWindow("Профиль изображения", WINDOW_NORMAL);
    resizeWindow("Профиль изображения", 800, 600);

    imshow("Изображение с штрих-кодом", I);         // Отображение оригинального изображения
    imshow("Профиль изображения", profileImg);      // Отображение построенного профиля

    // Ожидание нажатия клавиши для завершения программы
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
