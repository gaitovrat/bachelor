#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QtCore/qstring.h>

#include <optional>

struct Settings {
    QString portName;

    void save(const char *filename);

    static std::optional<Settings> load(const char *filename);
};

#endif