#ifndef UTILS_H
#define UTILS_H

#include <json/json.h>

#include <QComboBox>
#include <QGraphicsView>
#include <opencv2/core/mat.hpp>

#include "Shared/Network.h"

namespace CarQt::Utils {
/*
 * Get a key from a Json::Value object
 * @tparam T the type of the key
 * @param root the Json::Value object
 * @param key the key to get
 * @param keyDefault the default value if the key is not found
 * @return the value of the key or the default value if the key is not found
*/
template <typename T>
inline T jsonGetKey(const Json::Value &root, const char *key,
                    const T &keyDefault) {
    if (root.isMember(key)) return static_cast<T>(root[key].asInt());

    return keyDefault;
}

/*
 * Specialization for Json::Value
 */
template <>
inline Json::Value jsonGetKey<Json::Value>(const Json::Value &root,
                                           const char *key,
                                           const Json::Value &keyDefault) {
    if (root.isMember(key)) return root[key];

    return keyDefault;
}

/*
 * Specialization for std::string
 */
template <>
inline std::string jsonGetKey<std::string>(const Json::Value &root,
                                           const char *key,
                                           const std::string &keyDefault) {
    if (root.isMember(key)) return root[key].asString();

    return keyDefault;
}

/*
 * Convert a boolean to a QString
 * @param value the boolean to convert
 * @return the QString
 */
inline QString toQString(bool value) { return value ? "true" : "false"; }

/*
 * Set a boolean value to a QComboBox
 * @param comboBox the QComboBox to set the value to
 * @param value the boolean value to set
 */
void setComboBox(QComboBox &comboBox, const QString &value);

/*
 * Show a cv::Mat in a QGraphicsView
 * @param graphicsView the QGraphicsView to show the Mat in
 * @param mat the Mat to show
 */
void showMat(QGraphicsView *graphicsView, const cv::Mat &mat);

/*
 * Convert an IPv4 address to a QString
 * @param address the IPv4 address to convert
 * @return the QString
 */
QString ipv4ToString(RefIPv4 address);
}  // namespace CarQt::Utils

#endif  // UTILS_H
