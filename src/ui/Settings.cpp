#include "Settings.h"

#include <QtCore/qdebug.h>
#include <QtCore/qlogging.h>
#include <json/json.h>

#include <fstream>
#include <optional>

void Settings::save(const char *filename) {
    Json::Value root;
    Json::StreamWriterBuilder writer;
    std::ofstream fout(filename);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filename;
        return;
    }

    root["portName"] = this->portName.toUtf8().constData();

    fout << Json::writeString(writer, root);
}

std::optional<Settings> Settings::load(const char *filename) {
    std::ifstream fin(filename);
    Json::Reader reader;
    Json::Value root;
    Settings settings;

    if (!fin.is_open()) {
        qWarning() << "Unable to open" << filename;
        return std::nullopt;
    }

    if (!reader.parse(fin, root, false)) {
        qWarning() << "Unable to parse" << filename;
        return std::nullopt;
    }

    if (root.isMember("portName")) {
        settings.portName = root["portName"].asCString();
    }

    return settings;
}