#include "Shared/Utils.h"

using namespace Shared;

#ifdef PC

Json::Value Utils::dataToJson(const Shared::Data &data) {
    Json::Value jsonData, jsonAccel, jsonMag, jsonGyro, jsonCamera, jsonMotor,
        jsonSteer, jsonSensor;

    jsonCamera["regionsCount"] = data.regionsCount;
    jsonCamera["regionsListSize"] = std::to_string(data.regionsListSize);
    jsonCamera["unchangedLeft"] = data.unchangedLeft;
    jsonCamera["unchangedRight"] = data.unchangedRight;
    jsonCamera["hasLeft"] = data.hasLeft;
    jsonCamera["hasRight"] = data.hasRight;
    jsonCamera["leftDistance"] = data.leftDistance;
    jsonCamera["rightDistance"] = data.rightDistance;

    jsonMotor["leftSpeed"] = data.leftSpeed;
    jsonMotor["rightSpeed"] = data.rightSpeed;

    jsonSteer["angle"] = data.angle;
    jsonSteer["servoPosition"] = data.servoPosition;

    jsonAccel["x"] = data.accel.x;
    jsonAccel["y"] = data.accel.y;
    jsonAccel["z"] = data.accel.z;

    jsonMag["x"] = data.mag.x;
    jsonMag["y"] = data.mag.y;
    jsonMag["z"] = data.mag.z;

    jsonGyro["x"] = data.gyro.x;
    jsonGyro["y"] = data.gyro.y;
    jsonGyro["z"] = data.gyro.z;

    jsonSensor["accel"] = jsonAccel;
    jsonSensor["mag"] = jsonMag;
    jsonSensor["gyro"] = jsonGyro;

    jsonData["camera"] = jsonCamera;
    jsonData["motor"] = jsonMotor;
    jsonData["steer"] = jsonSteer;
    jsonData["sensor"] = jsonSensor;
    jsonData["mode"] = data.mode;
    jsonData["timestamp"] = data.timestamp;

    return jsonData;
}

Json::Value Utils::dataToJson(const std::vector<Shared::Data> &data) {
    Json::Value entry;

    for (const auto &el : data) entry.append(Utils::dataToJson(el));

    return entry;
}

std::vector<Shared::Data> Utils::jsonToData(const Json::Value &json) {
    std::vector<Shared::Data> out;
    Json::Value jsonArray;

    if (json.isArray()) {
        jsonArray = json;
    } else {
        jsonArray.append(json);
    }

    for (const auto &entry : jsonArray) {
        Shared::Data data;
        Json::Value el;
        if (entry.isMember("data")) {
            el = entry["data"];
        } else {
            el = entry;
        }

        data.regionsCount = el["camera"]["regionsCount"].asInt();
        data.regionsListSize =
            std::atoi(el["camera"]["regionsListSize"].asString().c_str());
        data.unchangedLeft = el["camera"]["unchangedLeft"].asBool();
        data.unchangedRight = el["camera"]["unchangedRight"].asBool();
        data.hasLeft = el["camera"]["hasLeft"].asBool();
        data.hasRight = el["camera"]["hasRight"].asBool();
        data.leftDistance = el["camera"]["leftDistance"].asInt();
        data.rightDistance = el["camera"]["rightDistance"].asInt();

        data.leftSpeed = el["motor"]["leftSpeed"].asInt();
        data.rightSpeed = el["motor"]["rightSpeed"].asInt();

        data.angle = el["steer"]["angle"].asInt();
        data.servoPosition = el["steer"]["servoPosition"].asInt();

        data.accel.x = el["sensor"]["accel"]["x"].asInt();
        data.accel.y = el["sensor"]["accel"]["y"].asInt();
        data.accel.z = el["sensor"]["accel"]["z"].asInt();

        data.mag.x = el["sensor"]["mag"]["x"].asInt();
        data.mag.y = el["sensor"]["mag"]["y"].asInt();
        data.mag.z = el["sensor"]["mag"]["z"].asInt();

        data.gyro.x = el["sensor"]["gyro"]["x"].asInt();
        data.gyro.y = el["sensor"]["gyro"]["y"].asInt();
        data.gyro.z = el["sensor"]["gyro"]["z"].asInt();

        data.mode = el["mode"].asInt();
        data.timestamp = el["timestamp"].asInt();

        out.push_back(data);
    }

    return out;
}

#endif
