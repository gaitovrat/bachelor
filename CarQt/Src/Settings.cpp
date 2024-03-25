#include "Settings.h"

#include <fstream>
#include <optional>

#include <json/json.h>

#include "Utils.h"

using namespace CarQt;

Settings::Network::Network() : address(QHostAddress::Any), port(5000u) {}

Settings::Settings() : network(), recordDestination(".") {}

void Settings::save(const char *filename) const {
    Json::Value root, serial, network;
    Json::StreamWriterBuilder writer;
    std::ofstream fout(filename);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filename;
        return;
    }

    network["address"] = this->network.address.toString().toUtf8().constData();
    network["port"] = this->network.port;

    root["serial"] = serial;
    root["network"] = network;
    root["recordDestination"] = this->recordDestination.toUtf8().constData();

    fout << Json::writeString(writer, root);
}

Settings Settings::load(const char *filename) {
    std::ifstream fin(filename);
    Json::Reader reader;
    Json::Value root;
    Settings settings;
    Network network;

    if (!fin.is_open()) {
        qWarning() << "Unable to open" << filename;
        return {};
    }

    if (!reader.parse(fin, root, false)) {
        qWarning() << "Unable to parse" << filename;
        return {};
    }

    Json::Value networkValue =
        Utils::jsonGetKey(root, "network", Json::Value());
    QHostAddress address(QString::fromStdString(
        Utils::jsonGetKey<std::string>(networkValue, "address", "")));
    network.address = address;
    network.port = Utils::jsonGetKey<uint32_t>(networkValue, "port", 0u);

    settings.recordDestination = QString::fromStdString(
        Utils::jsonGetKey<std::string>(root, "recordDestination", ""));
    settings.network = network;

    return settings;
}
