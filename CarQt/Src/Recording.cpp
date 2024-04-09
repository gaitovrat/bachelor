#include "Recording.h"

#include <fstream>

#include <json/json.h>
#include <opencv2/opencv.hpp>
#include <qdebug.h>
#include <string>

#include "Shared/Utils.h"

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
        Json::Value jsonEntry;

        lines.push_back(entry.data.line);

        jsonEntry["data"] = Shared::Utils::dataToJson(entry.data);
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
