#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QGraphicsView>
#include <opencv2/core/mat.hpp>

namespace Utils {
inline QString ToQString(bool value) {
    return value ? "true" : "false";
}

void ShowMat(QGraphicsView *graphicsView, const cv::Mat& mat);
}

#endif // UTILS_H
