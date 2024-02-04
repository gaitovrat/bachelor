#include "Settings.h"

#include "json/value.h"
#include <QtCore/qdebug.h>
#include <QtCore/qlogging.h>
#include <QtSerialPort/qserialport.h>
#include <json/json.h>

#include <fstream>
#include <optional>

template<typename T>
T get(Json::Value root, const char *key, T keyDefault) {
    if (root.isMember(key))
        return static_cast<T>(root[key].asInt());

    return keyDefault;
}

template<>
Json::Value get<Json::Value>(Json::Value root, const char *key, Json::Value keyDefault) {
    if (root.isMember(key))
        root[key];

    return keyDefault;
}

template<>
const char *get<const char *>(Json::Value root, const char *key, const char *keyDefault) {
    if (root.isMember(key))
        return root[key].asCString();

    return keyDefault;
}

SerialSettings::SerialSettings() : 
    portName(""),
    baudRate(QSerialPort::BaudRate::Baud115200),
    dataBits(QSerialPort::DataBits::Data8),
    parity(QSerialPort::Parity::NoParity),
    stopBits(QSerialPort::StopBits::OneStop)
{}

NetworkSettings::NetworkSettings() : address(""), port(0u) {}

Settings::Settings() : mode(ModeSettings::Serial), serial(), network(), recordDestination("") {}

void Settings::save(const char *filename) {
    Json::Value root, serial, network;
    Json::StreamWriterBuilder writer;
    std::ofstream fout(filename);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filename;
        return;
    }

    serial["portName"] = this->serial.portName.toUtf8().constData();
    serial["baudRate"] = this->serial.baudRate;
    serial["dataBits"] = this->serial.dataBits;
    serial["parity"] = this->serial.parity;
    serial["stopBits"] = this->serial.stopBits;

    network["address"] = this->network.address.toUtf8().constData();
    network["port"] = this->network.port;

    root["mode"] = this->mode;
    root["serial"] = serial;
    root["network"] = network;
    root["recordDestination"] = this->recordDestination.toUtf8().constData();

    fout << Json::writeString(writer, root);
}

std::optional<Settings> Settings::load(const char *filename) {
    std::ifstream fin(filename);
    Json::Reader reader;
    Json::Value root;
    Settings settings;
    SerialSettings serialSettings;
    NetworkSettings networkSettings;

    if (!fin.is_open()) {
        qWarning() << "Unable to open" << filename;
        return std::nullopt;
    }

    if (!reader.parse(fin, root, false)) {
        qWarning() << "Unable to parse" << filename;
        return std::nullopt;
    }

    Json::Value serial = get(root, "serial", Json::Value());
    serialSettings.portName = get(serial, "portName", "");
    serialSettings.baudRate = get(serial, "baudRate", QSerialPort::BaudRate::Baud115200);
    serialSettings.dataBits = get(serial, "dataBits", QSerialPort::DataBits::Data8);
    serialSettings.parity = get(serial, "parity", QSerialPort::Parity::NoParity);
    serialSettings.stopBits = get(serial, "stopBits", QSerialPort::StopBits::OneStop);

    Json::Value network = get(root, "network", Json::Value());
    networkSettings.address = get(network, "address", "");
    networkSettings.port = get<uint32_t>(network, "port", 0u);

    settings.mode = get(root, "mode", ModeSettings::Serial);
    settings.recordDestination = get(root, "recordDestination", "");
    settings.serial = serialSettings;
    settings.network = networkSettings;

    return settings;
}