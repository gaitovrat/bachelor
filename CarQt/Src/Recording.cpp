#include "Recording.h"
#include "Shared/CameraData.h"

#include <fstream>

#include <json/json.h>
#include <opencv2/opencv.hpp>
#include <qdebug.h>
#include <string>

using namespace CarQt;

Recording::Recording() : start(std::chrono::system_clock::now()) {}

void Recording::save(const QString &path) {
    Json::Value root;
    Json::StreamWriterBuilder writer;
    std::chrono::time_point<std::chrono::system_clock> end(
        std::chrono::system_clock::now());
    std::vector<const uint16_t *> lines;

    std::string filename =
        "recordng-" +
        std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                           start.time_since_epoch())
                           .count()) +
        "-" +
        std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                           end.time_since_epoch())
                           .count());

    std::string filepath = path.toStdString() + "/" + filename + ".json";
    std::string imageFilepath = path.toStdString() + "/" + filename + ".png";
    std::ofstream fout(filepath);

    if (!fout.is_open()) {
        qWarning() << "Unable to open" << filepath;
        return;
    }

    for (const Entry &entry : entries) {
        Json::Value jsonEntry, jsonData, jsonAccel, jsonMag, jsonGyro,
            jsonCamera, jsonMotor, jsonSteer, jsonSensor;

        lines.push_back(entry.data.cameraData.line);

        jsonCamera["regionsCount"] = entry.data.cameraData.regionsCount;
        jsonCamera["regionsListSize"] =
            std::to_string(entry.data.cameraData.regionsListSize);
        jsonCamera["unchangedLeft"] = entry.data.cameraData.unchangedLeft;
        jsonCamera["unchangedRight"] = entry.data.cameraData.unchangedRight;
        jsonCamera["hasLeft"] = entry.data.cameraData.hasLeft;
        jsonCamera["hasRight"] = entry.data.cameraData.hasRight;
        jsonCamera["leftDistance"] = entry.data.cameraData.leftDistance;
        jsonCamera["rightDistance"] = entry.data.cameraData.rightDistance;

        jsonMotor["leftSpeed"] = entry.data.motorData.leftSpeed;
        jsonMotor["rightSpeed"] = entry.data.motorData.rightSpeed;
        jsonMotor["state"] = entry.data.motorData.state;

        jsonSteer["angle"] = entry.data.steerData.angle;
        jsonSteer["servoPosition"] = entry.data.steerData.servoPosition;

        jsonAccel["x"] = entry.data.sensorData.accel.x;
        jsonAccel["y"] = entry.data.sensorData.accel.y;
        jsonAccel["z"] = entry.data.sensorData.accel.z;

        jsonMag["x"] = entry.data.sensorData.mag.x;
        jsonMag["y"] = entry.data.sensorData.mag.y;
        jsonMag["z"] = entry.data.sensorData.mag.z;

        jsonGyro["x"] = entry.data.sensorData.gyro.x;
        jsonGyro["y"] = entry.data.sensorData.gyro.y;
        jsonGyro["z"] = entry.data.sensorData.gyro.z;

        jsonSensor["accel"] = jsonAccel;
        jsonSensor["mag"] = jsonMag;
        jsonSensor["gyro"] = jsonGyro;

        jsonData["camera"] = jsonCamera;
        jsonData["motor"] = jsonMotor;
        jsonData["steer"] = jsonSteer;
        jsonData["sensor"] = jsonSensor;
        jsonData["timestamp"] = entry.data.timestamp;

        jsonEntry["data"] = jsonData;
        jsonEntry["time"] = std::chrono::duration_cast<std::chrono::seconds>(
                                entry.time.time_since_epoch())
                                .count();

        root.append(jsonEntry);
    }

    fout << Json::writeString(writer, root);

    int historySize = static_cast<int>(lines.size());
    cv::Mat image(historySize, Shared::Image::LINE_LENGTH, CV_8UC1,
                  cv::Scalar(0));
    for (int i = 0; i < lines.size(); ++i) {
        for (int j = 0; j < Shared::Image::LINE_LENGTH; ++j) {
            image.at<uint8_t>(i, j) = lines[i][j];
        }
    }
    cv::imwrite(imageFilepath, image);
}

void Recording::add(const Shared::Data &data) {
    Entry entry;
    entry.time = std::chrono::system_clock::now();
    entry.data = data;

    entries.push_back(entry);
}
