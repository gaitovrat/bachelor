#include "Recording.h"

#include <fstream>

#include <json/json.h>
#include <qdebug.h>

using namespace CarQt;

Recording::Recording() : Start(std::chrono::system_clock::now()) {}

void Recording::Save(const QString &path) {
    Json::Value root;
    Json::StreamWriterBuilder writer;
    std::chrono::time_point<std::chrono::system_clock> end(
        std::chrono::system_clock::now());

    std::string filename =
        "recordng-" +
        std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                           Start.time_since_epoch())
                           .count()) +
        "-" +
        std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                           end.time_since_epoch())
                           .count()) +
        ".json";
    std::string filepath = path.toStdString() + "/" + filename;
    std::ofstream fout(filepath);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filepath;
        return;
    }

    for (const Entry &entry : entries) {
        Json::Value jsonEntry, jsonData, jsonAccel, jsonMag, jsonGyro;

        jsonAccel["x"] = entry.data.CarSensorData.accel.X;
        jsonAccel["y"] = entry.data.CarSensorData.accel.Y;
        jsonAccel["z"] = entry.data.CarSensorData.accel.Z;

        jsonMag["x"] = entry.data.CarSensorData.mag.X;
        jsonMag["y"] = entry.data.CarSensorData.mag.Y;
        jsonMag["z"] = entry.data.CarSensorData.mag.Z;

        jsonGyro["x"] = entry.data.CarSensorData.gyro.X;
        jsonGyro["y"] = entry.data.CarSensorData.gyro.Y;
        jsonGyro["z"] = entry.data.CarSensorData.gyro.Z;

        jsonData["accel"] = jsonAccel;
        jsonData["mag"] = jsonMag;
        jsonData["gyro"] = jsonGyro;

        jsonEntry["time"] =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                entry.Time.time_since_epoch())
                .count();
        jsonEntry["data"] = jsonData;

        root.append(jsonEntry);
    }

    fout << Json::writeString(writer, root);
}

void Recording::Add(const Shared::Data &data) {
    Entry entry;
    entry.Time = std::chrono::system_clock::now();
    entry.data = data;

    entries.push_back(entry);
}
