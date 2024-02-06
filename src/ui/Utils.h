#ifndef UTILS_H
#define UTILS_H

#include <json/json.h>
#include <QComboBox>

namespace Utils {
    template<typename T>
    inline T jsonGetKey(const Json::Value& root, const char *key, const T& keyDefault) {
        if (root.isMember(key))
            return static_cast<T>(root[key].asInt());

        return keyDefault;
    }

    template<>
    inline Json::Value jsonGetKey<Json::Value>(const Json::Value& root, const char *key, const Json::Value& keyDefault) {
        if (root.isMember(key))
            return root[key];

        return keyDefault;
    }

    template<>
    inline std::string jsonGetKey<std::string>(const Json::Value& root, const char *key, const std::string& keyDefault) {
        if (root.isMember(key))
            return root[key].asString();

        return keyDefault;
    }

    void setComboBox(QComboBox& comboBox, const QString& value);
}
#endif // UTILS_H
