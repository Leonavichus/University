#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main() {

    Mat I = imread("background.png");  // Загрузите изображение

    if (I.empty()) {  // Проверка на успешную загрузку изображения
        cerr << "Ошибка: не удалось загрузить изображение!" << endl;
        return -1;
    }

    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange[] = {range};

    vector<Mat> bgrChannels;
    split(I, bgrChannels);

    Mat bHist, gHist, rHist;

    calcHist(&bgrChannels[0], 1, 0, Mat(), bHist, 1, &histSize, histRange);
    calcHist(&bgrChannels[1], 1, 0, Mat(), gHist, 1, &histSize, histRange);
    calcHist(&bgrChannels[2], 1, 0, Mat(), rHist, 1, &histSize, histRange);

    // Увеличиваем высоту и ширину изображения для отображения гистограммы
    int histImageHeight = 800;
    int histImageWidth = 1024;
    int binWidth = cvRound((double) histImageWidth / histSize);

    // Нормализация гистограмм с увеличенным диапазоном, чтобы линии стали заметнее
    normalize(bHist, bHist, 0, histImageHeight * 0.9, NORM_MINMAX);
    normalize(gHist, gHist, 0, histImageHeight * 0.9, NORM_MINMAX);
    normalize(rHist, rHist, 0, histImageHeight * 0.9, NORM_MINMAX);

    // Создание изображения для отображения гистограммы (увеличенный размер тёмной области)
    Mat histImage(histImageHeight, histImageWidth, CV_8UC3, Scalar(50, 50, 50));

    for (int i = 1; i < histSize; i++) {
        line(histImage,
             Point(binWidth * (i - 1), histImageHeight - cvRound(bHist.at<float>(i - 1))),
             Point(binWidth * i, histImageHeight - cvRound(bHist.at<float>(i))),
             Scalar(255, 0, 0), 2);  // Синий канал

        line(histImage,
             Point(binWidth * (i - 1), histImageHeight - cvRound(gHist.at<float>(i - 1))),
             Point(binWidth * i, histImageHeight - cvRound(gHist.at<float>(i))),
             Scalar(0, 255, 0), 2);  // Зеленый канал

        line(histImage,
             Point(binWidth * (i - 1), histImageHeight - cvRound(rHist.at<float>(i - 1))),
             Point(binWidth * i, histImageHeight - cvRound(rHist.at<float>(i))),
             Scalar(0, 0, 255), 2);  // Красный канал
    }

    // Создание окна с возможностью изменения размера
    namedWindow("Гистограмма", WINDOW_NORMAL);
    resizeWindow("Гистограмма", 1200, 900);  // Устанавливаем размер окна

    // Отображение гистограммы
    imshow("Гистограмма", histImage);

    // Ожидание нажатия клавиши 'q' для закрытия окна
    while (true) {
        int key = waitKey(0);  // Ожидание нажатия любой клавиши
        if (key == 'q' || key == 'Q') {  // Проверка нажатия 'q' или 'Q'
            break;
        }
    }

    return 0;
}
