#include "Utils.h"
#include <QGraphicsPixmapItem>

using namespace CarQt;



void Utils::SetComboBox(QComboBox& comboBox, const QString& value) {
    int index = comboBox.findText(value);
    if (index == -1)
        return;

    comboBox.setCurrentIndex(index);
}

void Utils::ShowMat(QGraphicsView *graphicsView, const cv::Mat& mat) {
    QGraphicsScene *scene = graphicsView->scene();
    scene->clear();

    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)));

    item->setPos(0, 0);
    graphicsView->setScene(scene);
    graphicsView->show();
}
