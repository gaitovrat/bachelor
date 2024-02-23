#include "Utils.h"

#include <QGraphicsPixmapItem>

void Utils::ShowMat(QGraphicsView *graphicsView, const cv::Mat& mat) {
    QGraphicsScene *scene = graphicsView->scene();
    scene->clear();

    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)));

    item->setPos(0, 0);
    graphicsView->setScene(scene);
    graphicsView->show();
}
