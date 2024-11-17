/*
3. «Склейка» изображений. Выбрать два изображения (снимки с фотокамеры, фрагменты сканированного изображения и пр.), на которых имеется область пересечения.
Выполнить коррекцию второго изображения для его перевода в систему координат первого; затем выполнить автоматическую «склейку» из двух изображений в одно.
*/

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Считывание изображений
    Mat topPart = imread("1.jpg", IMREAD_COLOR); // Верхняя часть
    Mat botPart = imread("2.jpg", IMREAD_COLOR); // Нижняя часть

    if (topPart.empty() || botPart.empty()) {
        cerr << "Ошибка загрузки изображений!" << endl;
        return -1;
    }

    // Размер шаблона для поиска пересечения
    int templ_size = 10;
    Mat templ = topPart(Rect(0, topPart.rows - templ_size - 1, topPart.cols, templ_size));

    // Вычисляем коэффициенты корреляции между шаблоном и нижней частью изображения
    Mat res;
    matchTemplate(botPart, templ, res, TM_CCOEFF);

    // Ищем максимальное значение корреляции (точка склейки)
    double min_val, max_val;
    Point min_loc, max_loc;
    minMaxLoc(res, &min_val, &max_val, &min_loc, &max_loc);

    // Создаем итоговое изображение с учетом пересечения
    Mat result_img = Mat::zeros(topPart.rows + botPart.rows - max_loc.y - templ_size, topPart.cols, topPart.type());

    // Копируем верхнюю часть изображения
    topPart.copyTo(result_img(Rect(0, 0, topPart.cols, topPart.rows)));

    // Копируем нижнюю часть изображения, начиная с найденной точки склейки
    botPart(Rect(0, max_loc.y + templ_size, botPart.cols, botPart.rows - max_loc.y - templ_size))
        .copyTo(result_img(Rect(0, topPart.rows, botPart.cols, botPart.rows - max_loc.y - templ_size)));

    // Отображаем результат
    namedWindow("Результат склейки", WINDOW_NORMAL);
    imshow("Результат склейки", result_img);
    while (true) {
        int key = waitKey(0);
        if (key == 'q' || key == 'Q') {
            break;
        }
    }

    return 0;
}
