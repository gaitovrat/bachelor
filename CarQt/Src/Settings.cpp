#include "Settings.h"

#include <fstream>
#include <optional>

#include <json/json.h>

#include "Utils.h"

Settings::Serial::Serial() :
    portName(""),
    baudRate(QSerialPort::BaudRate::Baud115200),
    dataBits(QSerialPort::DataBits::Data8),
    parity(QSerialPort::Parity::NoParity),
    stopBits(QSerialPort::StopBits::OneStop)
{}

Settings::Network::Network() : address(QHostAddress::Any), port(8080u) {}

Settings::Settings() :
    mode(Mode::Serial),
    serial(),
    network(),
    recordDestination(".") {}

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

    network["address"] = this->network.address.toString().toUtf8().constData();
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
    struct Serial serialSettings;
    struct Network networkSettings;

    if (!fin.is_open()) {
        qWarning() << "Unable to open" << filename;
        return std::nullopt;
    }

    if (!reader.parse(fin, root, false)) {
        qWarning() << "Unable to parse" << filename;
        return std::nullopt;
    }

    Json::Value serial = Utils::jsonGetKey(root, "serial", Json::Value());
    serialSettings.portName = QString::fromStdString(Utils::jsonGetKey<std::string>(serial, "portName", ""));
    serialSettings.baudRate = Utils::jsonGetKey(serial, "baudRate", QSerialPort::BaudRate::Baud115200);
    serialSettings.dataBits = Utils::jsonGetKey(serial, "dataBits", QSerialPort::DataBits::Data8);
    serialSettings.parity = Utils::jsonGetKey(serial, "parity", QSerialPort::Parity::NoParity);
    serialSettings.stopBits = Utils::jsonGetKey(serial, "stopBits", QSerialPort::StopBits::OneStop);

    Json::Value network = Utils::jsonGetKey(root, "network", Json::Value());
    QHostAddress address(QString::fromStdString(Utils::jsonGetKey<std::string>(network, "address", "")));
    networkSettings.address = address;
    networkSettings.port = Utils::jsonGetKey<uint32_t>(network, "port", 0u);

    settings.mode = Utils::jsonGetKey(root, "mode", Mode::Serial);
    settings.recordDestination = QString::fromStdString(Utils::jsonGetKey<std::string>(root, "recordDestination", ""));
    settings.serial = serialSettings;
    settings.network = networkSettings;

    return settings;
}

const char *Settings::modeToString(Mode mode) {
    switch (mode) {
    case Mode::Network:
        return "Network";
    case Mode::Serial:
        return "Serial";
    default:
        return "";
    }
}

Settings::Mode Settings::stringToMode(const QString& value) {
    if (value == "Network")
        return Mode::Network;
    return Mode::Serial;
}
