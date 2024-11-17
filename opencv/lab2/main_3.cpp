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
    Mat topPart = imread("1.jpg", IMREAD_COLOR); // Верхняя часть изображения
    Mat botPart = imread("2.jpg", IMREAD_COLOR); // Нижняя часть изображения

    // Проверка на успешную загрузку изображений
    if (topPart.empty() || botPart.empty()) {
        cerr << "Ошибка загрузки изображений!" << endl;
        return -1;
    }

    // Определяем размер шаблона для поиска точки пересечения
    int templ_size = 10; // Высота шаблона (в пикселях)
    if (topPart.rows < templ_size) {
        cerr << "Верхняя часть изображения слишком мала для указанного размера шаблона!" << endl;
        return -1;
    }

    // Извлекаем нижнюю часть верхнего изображения в качестве шаблона
    Mat templ = topPart(Rect(0, topPart.rows - templ_size, topPart.cols, templ_size));

    // Поиск шаблона в нижнем изображении
    Mat res;
    matchTemplate(botPart, templ, res, TM_CCOEFF); // TM_CCOEFF используется для вычисления корреляции

    // Определение позиции максимального значения корреляции (точки пересечения)
    double min_val, max_val;
    Point min_loc, max_loc;
    minMaxLoc(res, &min_val, &max_val, &min_loc, &max_loc); // Получаем координаты точки максимума

    // Создаем итоговое изображение с учетом найденной точки склейки
    int overlap_y = max_loc.y + templ_size; // Y-координата начала области пересечения
    int result_height = topPart.rows + botPart.rows - overlap_y; // Общая высота результата
    Mat result_img = Mat::zeros(result_height, topPart.cols, topPart.type()); // Итоговое изображение

    // Копируем верхнюю часть изображения в итоговое изображение
    topPart.copyTo(result_img(Rect(0, 0, topPart.cols, topPart.rows)));

    // Копируем нижнюю часть изображения, начиная с точки пересечения
    botPart(Rect(0, overlap_y, botPart.cols, botPart.rows - overlap_y))
        .copyTo(result_img(Rect(0, topPart.rows, botPart.cols, botPart.rows - overlap_y)));

    // Отображаем результат
    namedWindow("Результат склейки", WINDOW_NORMAL);
    imshow("Результат склейки", result_img);

    // Сохранение результата
    imwrite("stitched_result.jpg", result_img); // Сохраняем итоговое изображение в файл

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
