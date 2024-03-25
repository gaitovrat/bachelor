#ifndef UTILS_H
#define UTILS_H

#include <QComboBox>
#include <QGraphicsView>
#include <json/json.h>
#include <opencv2/core/mat.hpp>

namespace CarQt::Utils {
template <typename T>
inline T jsonGetKey(const Json::Value &root, const char *key,
                    const T &keyDefault) {
    if (root.isMember(key))
        return static_cast<T>(root[key].asInt());

    return keyDefault;
}

template <>
inline Json::Value jsonGetKey<Json::Value>(const Json::Value &root,
                                           const char *key,
                                           const Json::Value &keyDefault) {
    if (root.isMember(key))
        return root[key];

    return keyDefault;
}

template <>
inline std::string jsonGetKey<std::string>(const Json::Value &root,
                                           const char *key,
                                           const std::string &keyDefault) {
    if (root.isMember(key))
        return root[key].asString();

    return keyDefault;
}

inline QString toQString(bool value) { return value ? "true" : "false"; }

void setComboBox(QComboBox &comboBox, const QString &value);

void showMat(QGraphicsView *graphicsView, const cv::Mat &mat);
} // namespace CarQt::Utils
#endif // UTILS_H
