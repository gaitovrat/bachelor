#include "Recording.h"

#include <fstream>

#include <json/json.h>
#include <qdebug.h>
#include <string>

using namespace CarQt;

Recording::Recording() : Start(std::chrono::system_clock::now()) {}

void Recording::Save(const QString &path,
                     const std::vector<QGraphicsView *> &views) {
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
                           .count());

    for (int i = 0; i < views.size(); ++i) {
        views[i]->grab().save(
            QString::fromStdString(path.toStdString() + "/" + filename +
                                   "_image" + std::to_string(i) + ".png"),
            "PNG");
    }
    std::string filepath = path.toStdString() + "/" + filename + ".json";
    std::ofstream fout(filepath);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filepath;
        return;
    }

    for (const Entry &entry : entries) {
        Json::Value jsonEntry, jsonData, jsonAccel, jsonMag, jsonGyro,
            jsonCamera, jsonMotor, jsonSteer, jsonSteerPID, jsonSensor;

        jsonCamera["regionsCount"] = entry.data.CarCameraData.RegionsCount;
        jsonCamera["regionsListSize"] =
            std::to_string(entry.data.CarCameraData.RegionsListSize);
        jsonCamera["unchangedLeft"] = entry.data.CarCameraData.UnchangedLeft;
        jsonCamera["unchangedRight"] = entry.data.CarCameraData.UnchangedRight;
        jsonCamera["hasLeft"] = entry.data.CarCameraData.HasLeft;
        jsonCamera["hasRight"] = entry.data.CarCameraData.HasRight;
        jsonCamera["leftDistance"] = entry.data.CarCameraData.LeftDistance;
        jsonCamera["rightDistance"] = entry.data.CarCameraData.RightDistance;

        jsonMotor["leftSpeed"] = entry.data.CarMotorData.LeftSpeed;
        jsonMotor["rightSpeed"] = entry.data.CarMotorData.RightSpeed;
        jsonMotor["mode"] = entry.data.CarMotorData.RideMode;
        jsonMotor["state"] = entry.data.CarMotorData.CarMotorState;

        jsonSteer["angle"] = entry.data.CarSteerData.Angle;
        jsonSteer["servoPosition"] = entry.data.CarSteerData.ServoPosition;
        jsonSteerPID["i"] = entry.data.CarSteerData.SteerPIDData.I;
        jsonSteerPID["p"] = entry.data.CarSteerData.SteerPIDData.P;
        jsonSteerPID["d"] = entry.data.CarSteerData.SteerPIDData.D;
        jsonSteerPID["input"] = entry.data.CarSteerData.SteerPIDData.Input;
        jsonSteerPID["output"] = entry.data.CarSteerData.SteerPIDData.Output;
        jsonSteerPID["setPoint"] =
            entry.data.CarSteerData.SteerPIDData.SetPoint;
        jsonSteer["pid"] = jsonSteerPID;

        jsonAccel["x"] = entry.data.CarSensorData.accel.X;
        jsonAccel["y"] = entry.data.CarSensorData.accel.Y;
        jsonAccel["z"] = entry.data.CarSensorData.accel.Z;

        jsonMag["x"] = entry.data.CarSensorData.mag.X;
        jsonMag["y"] = entry.data.CarSensorData.mag.Y;
        jsonMag["z"] = entry.data.CarSensorData.mag.Z;

        jsonGyro["x"] = entry.data.CarSensorData.gyro.X;
        jsonGyro["y"] = entry.data.CarSensorData.gyro.Y;
        jsonGyro["z"] = entry.data.CarSensorData.gyro.Z;

        jsonSensor["accel"] = jsonAccel;
        jsonSensor["mag"] = jsonMag;
        jsonSensor["gyro"] = jsonGyro;

        jsonData["camera"] = jsonCamera;
        jsonData["motor"] = jsonMotor;
        jsonData["steer"] = jsonSteer;
        jsonData["sensor"] = jsonSensor;
        jsonData["timestamp"] = entry.data.Timestamp;

        jsonEntry["data"] = jsonData;
        jsonEntry["time"] = std::chrono::duration_cast<std::chrono::seconds>(
                                entry.Time.time_since_epoch())
                                .count();

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
