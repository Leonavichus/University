#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    // Загрузка изображения
    Mat I = imread("background.png");
    if (I.empty()) {
        cerr << "Ошибка: не удалось загрузить изображение!" << endl;
        return -1;
    }

    // Параметр для управления изменением интенсивности
    float alfa = 0.5;
    Mat Inew;

    // Преобразование к типу с плавающей точкой, если изображение в формате 8-бит
    if (I.depth() == CV_8U)
        I.convertTo(Inew, CV_32F, 1.0 / 255);
    else
        Inew = I.clone();

    // Разделение изображения на цветовые каналы
    vector<Mat> I_BGR;
    split(Inew, I_BGR);

    // Обработка каждого канала отдельно
    for (int k = 0; k < I_BGR.size(); k++) {
        // Создание временного изображения для хранения результатов
        Mat channelNew = Mat(I_BGR[k].rows, I_BGR[k].cols, I_BGR[k].type());

        // Поиск минимального и максимального значений яркости
        double Imin, Imax;
        minMaxLoc(I_BGR[k], &Imin, &Imax);

        // Изменение интенсивности каждого пикселя
        for (int i = 0; i < I_BGR[k].rows; i++) {
            for (int j = 0; j < I_BGR[k].cols; j++) {
                float intensity = (I_BGR[k].at<float>(i, j) - Imin) / (Imax - Imin);
                channelNew.at<float>(i, j) = pow(intensity, alfa);
            }
        }

        // Замена обработанного канала
        I_BGR[k] = channelNew;
    }

    // Объединение каналов обратно в одно изображение
    merge(I_BGR, Inew);

    // Преобразование обратно в 8-битное изображение, если исходное было 8-битным
    if (I.depth() == CV_8U)
        Inew.convertTo(Inew, CV_8U, 255);

    // Увеличение размера окна для отображения изображения
    namedWindow("Оригинал", WINDOW_NORMAL);  // Окно с возможностью изменения размера
    resizeWindow("Оригинал", 800, 600);  // Устанавливаем размер окна
    imshow("Оригинал", I);

    // Увеличение размера окна для отображения обработанного изображения
    namedWindow("Обработанное изображение", WINDOW_NORMAL);  // Окно с возможностью изменения размера
    resizeWindow("Обработанное изображение", 800, 600);  // Устанавливаем размер окна
    imshow("Обработанное изображение", Inew);

    while (true) {
        int key = waitKey(0);  // Ожидание нажатия любой клавиши
        if (key == 'q' || key == 'Q') {  // Проверка нажатия 'q' или 'Q'
            break;
        }
    }

    return 0;
}
