#include "Recording.h"

#include <fstream>

#include <json/json.h>
#include <qdebug.h>


Recording::Recording() : start(std::chrono::system_clock::now()) {}

void Recording::save(const QString& path) {
	Json::Value root;
	Json::StreamWriterBuilder writer;
	std::chrono::time_point<std::chrono::system_clock> end(std::chrono::system_clock::now());
	
	std::string filename = "recordng-" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(this->start.time_since_epoch()).count()) + "-" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(end.time_since_epoch()).count()) + ".json";
	std::string filepath = path.toStdString() + "/" + filename;
	std::ofstream fout(filepath);

	if (!fout.is_open()) {
		qWarning() << "Unable to open" << filepath;
		return;
	}

	for (const Entry& entry : this->entries) {
		Json::Value jsonEntry, jsonData, jsonAccel, jsonMag, jsonGyro;
		
		jsonAccel["x"] = entry.data.accel.x;
		jsonAccel["y"] = entry.data.accel.y;
		jsonAccel["z"] = entry.data.accel.z;

		jsonMag["x"] = entry.data.mag.x;
		jsonMag["y"] = entry.data.mag.y;
		jsonMag["z"] = entry.data.mag.z;

		jsonGyro["x"] = entry.data.gyro.x;
		jsonGyro["y"] = entry.data.gyro.y;
		jsonGyro["z"] = entry.data.gyro.z;

		jsonData["accel"] = jsonAccel;
		jsonData["mag"] = jsonMag;
		jsonData["gyro"] = jsonGyro;

		jsonEntry["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(entry.time.time_since_epoch()).count();
		jsonEntry["data"] = jsonData;

		root.append(jsonEntry);
	}

	fout << Json::writeString(writer, root);
}

void Recording::add(const Data& data) {
	Entry entry;
	entry.time = std::chrono::system_clock::now();
	entry.data = data;

	this->entries.push_back(entry);
}

