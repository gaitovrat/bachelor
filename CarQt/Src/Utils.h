#ifndef UTILS_H
#define UTILS_H

#include <QComboBox>
#include <QGraphicsView>
#include <json/json.h>
#include <opencv2/core/mat.hpp>

namespace CarQt::Utils {
template <typename T>
inline T JsonGetKey(const Json::Value &root, const char *key,
                    const T &keyDefault) {
    if (root.isMember(key))
        return static_cast<T>(root[key].asInt());

    return keyDefault;
}

template <>
inline Json::Value JsonGetKey<Json::Value>(const Json::Value &root,
                                           const char *key,
                                           const Json::Value &keyDefault) {
    if (root.isMember(key))
        return root[key];

    return keyDefault;
}

template <>
inline std::string JsonGetKey<std::string>(const Json::Value &root,
                                           const char *key,
                                           const std::string &keyDefault) {
    if (root.isMember(key))
        return root[key].asString();

    return keyDefault;
}

inline QString ToQString(bool value) { return value ? "true" : "false"; }

void SetComboBox(QComboBox &comboBox, const QString &value);

void ShowMat(QGraphicsView *graphicsView, const cv::Mat &mat);
} // namespace CarQt::Utils
#endif // UTILS_H
