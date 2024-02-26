#include "Settings.h"

#include <fstream>
#include <optional>

#include <json/json.h>

#include "Utils.h"

using namespace CarQt;

Settings::Serial::Serial()
    : PortName(""), BaudRate(QSerialPort::BaudRate::Baud115200),
      DataBits(QSerialPort::DataBits::Data8),
      Parity(QSerialPort::Parity::NoParity),
      StopBits(QSerialPort::StopBits::OneStop) {}

Settings::Network::Network() : Address(QHostAddress::Any), Port(8080u) {}

Settings::Settings()
    : AppMode(Mode::Serial), ClientSerial(), ClientNetwork(),
      RecordDestination(".") {}

void Settings::Save(const char *filename) {
    Json::Value root, serial, network;
    Json::StreamWriterBuilder writer;
    std::ofstream fout(filename);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filename;
        return;
    }

    serial["portName"] = ClientSerial.PortName.toUtf8().constData();
    serial["baudRate"] = ClientSerial.BaudRate;
    serial["dataBits"] = ClientSerial.DataBits;
    serial["parity"] = ClientSerial.Parity;
    serial["stopBits"] = ClientSerial.StopBits;

    network["address"] = ClientNetwork.Address.toString().toUtf8().constData();
    network["port"] = ClientNetwork.Port;

    root["mode"] = AppMode;
    root["serial"] = serial;
    root["network"] = network;
    root["recordDestination"] = RecordDestination.toUtf8().constData();

    fout << Json::writeString(writer, root);
}

std::optional<Settings> Settings::Load(const char *filename) {
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

    Json::Value serial = Utils::JsonGetKey(root, "serial", Json::Value());
    serialSettings.PortName = QString::fromStdString(
        Utils::JsonGetKey<std::string>(serial, "portName", ""));
    serialSettings.BaudRate = Utils::JsonGetKey(
        serial, "baudRate", QSerialPort::BaudRate::Baud115200);
    serialSettings.DataBits =
        Utils::JsonGetKey(serial, "dataBits", QSerialPort::DataBits::Data8);
    serialSettings.Parity =
        Utils::JsonGetKey(serial, "parity", QSerialPort::Parity::NoParity);
    serialSettings.StopBits =
        Utils::JsonGetKey(serial, "stopBits", QSerialPort::StopBits::OneStop);

    Json::Value network = Utils::JsonGetKey(root, "network", Json::Value());
    QHostAddress address(QString::fromStdString(
        Utils::JsonGetKey<std::string>(network, "address", "")));
    networkSettings.Address = address;
    networkSettings.Port = Utils::JsonGetKey<uint32_t>(network, "port", 0u);

    settings.AppMode = Utils::JsonGetKey(root, "mode", Mode::Serial);
    settings.RecordDestination = QString::fromStdString(
        Utils::JsonGetKey<std::string>(root, "recordDestination", ""));
    settings.ClientSerial = serialSettings;
    settings.ClientNetwork = networkSettings;

    return settings;
}

const char *Settings::ModeToString(Mode mode) {
    switch (mode) {
    case Mode::Network:
        return "Network";
    case Mode::Serial:
        return "Serial";
    default:
        return "";
    }
}

Settings::Mode Settings::ModeFromString(const QString &value) {
    if (value == "Network")
        return Mode::Network;
    return Mode::Serial;
}
