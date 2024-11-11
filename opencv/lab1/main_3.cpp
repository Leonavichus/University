/*
3. Профили. Выбрать произвольное изображение, содержащие штрих-код.
Выполнить построение профиля изображения вдоль штрих-кода.
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Загрузка изображения с штрих-кодом
    Mat I = imread("images.jpeg", IMREAD_GRAYSCALE); // Замените на путь к вашему изображению

    // Проверка успешности загрузки изображения
    if (I.empty()) {
        cerr << "Ошибка загрузки изображения!" << endl;
        return -1;
    }

    // Бинаризация изображения (черно-белое)
    Mat binaryImage;
    threshold(I, binaryImage, 128, 255, THRESH_BINARY); // Порог для бинаризации

    // Построение горизонтального профиля вдоль изображения (по линии через штрих-код)
    int profileHeight = I.rows / 2; // Высота линии, вдоль которой строим профиль
    Mat profile = Mat::zeros(1, I.cols, CV_32F);

    // Суммируем интенсивности пикселей вдоль горизонтальной линии
    for (int j = 0; j < I.cols; j++) {
        for (int i = profileHeight - 10; i < profileHeight + 10; i++) { // Берем небольшую область вдоль линии
            profile.at<float>(0, j) += binaryImage.at<uchar>(i, j); // Сумма интенсивности (пикселей черного цвета)
        }
    }

    // Нормализация профиля для лучшего отображения
    normalize(profile, profile, 0, 255, NORM_MINMAX);

    // Создаем изображение для отображения профиля
    Mat profileImg = Mat::zeros(100, I.cols, CV_8UC3);
    for (int i = 0; i < I.cols; i++) {
        line(profileImg, Point(i, profileImg.rows), Point(i, profileImg.rows - cvRound(profile.at<float>(0, i))), Scalar(255, 255, 255), 1);
    }

    // Отображение изображения и профиля
    namedWindow("Изображение с штрих-кодом", WINDOW_NORMAL);
    resizeWindow("Изображение с штрих-кодом", 800, 600);
    namedWindow("Профиль изображения", WINDOW_NORMAL);
    resizeWindow("Профиль изображения", 800, 600);

    imshow("Изображение с штрих-кодом", I);
    imshow("Профиль изображения", profileImg);

    // Ожидание нажатия клавиши для закрытия окон
    while (true) {
        int key = waitKey(0);  // Ожидание нажатия любой клавиши
        if (key == 'q' || key == 'Q') {  // Проверка нажатия 'q' или 'Q'
            break;
        }
    }

    return 0;
}
