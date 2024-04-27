#include "Utils.h"

#include <QGraphicsPixmapItem>

using namespace CarQt;

static constexpr const char *IP4_FORMAT = "%d.%d.%d.%d";

void Utils::setComboBox(QComboBox &comboBox, const QString &value) {
    int index = comboBox.findText(value);
    if (index == -1) return;

    comboBox.setCurrentIndex(index);
}

void Utils::showMat(QGraphicsView *graphicsView, const cv::Mat &mat) {
    QGraphicsScene *scene = graphicsView->scene();
    scene->clear();

    QGraphicsPixmapItem *item = scene->addPixmap(QPixmap::fromImage(
        QImage(mat.data, mat.cols, mat.rows, QImage::Format_RGB888)));

    item->setPos(0, 0);
    graphicsView->setScene(scene);
    graphicsView->show();
}

QString Utils::ipv4ToString(RefIPv4 address) {
    return QString::asprintf(IP4_FORMAT, address[0], address[1], address[2],
                             address[3]);
}
