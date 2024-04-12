#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include <json/json.h>

#include "Shared/Data.h"
#include "Shared/Filter.h"
#include "Shared/Utils.h"

int main(const int argc, const char **argv) {
    Json::Reader reader;
    Json::Value root;
    Json::Value outRoot;
    Json::StreamWriterBuilder writer;
    std::ifstream fin;
    std::ofstream fout;
    std::vector<Shared::Data> data;
    Shared::Filter filterAccelX;
    Shared::Filter filterAccelY;
    Shared::Filter filterAccelZ;
    Shared::Filter filterMagX;
    Shared::Filter filterMagY;
    Shared::Filter filterMagZ;
    Shared::Filter filterGyroX;
    Shared::Filter filterGyroY;
    Shared::Filter filterGyroZ;

    if (argc < 2) {
        std::cerr << "Usage: ApplyFilter" << argv[0] << " <data.json>"
                  << std::endl;
        return 1;
    }

    fin.open(argv[1]);
    std::cout << "Reading from " << argv[1] << std::endl;
    fout.open("out.json");
    if (!fin.is_open()) {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        return 1;
    }
    if (!fout.is_open()) {
        std::cerr << "Error: could not open file out.json" << std::endl;
        return 1;
    }
    if (!reader.parse(fin, root, false)) {
        std::cerr << "Error: could not parse JSON file " << argv[1]
                  << std::endl;
        return 1;
    }

    data = Shared::Utils::jsonToData(root);
    for (int i = 0; i < data.size(); ++i) {
        filterAccelX.add(data[i].accel.x);
        filterAccelY.add(data[i].accel.y);
        filterAccelZ.add(data[i].accel.z);

        filterMagX.add(data[i].mag.x);
        filterMagY.add(data[i].mag.y);
        filterMagZ.add(data[i].mag.z);

        filterGyroX.add(data[i].gyro.x);
        filterGyroY.add(data[i].gyro.y);
        filterGyroZ.add(data[i].gyro.z);

        data[i].accel.x = filterAccelX.movingAverage();
        data[i].accel.y = filterAccelY.movingAverage();
        data[i].accel.z = filterAccelZ.movingAverage();

        data[i].mag.x = filterMagX.movingAverage();
        data[i].mag.y = filterMagY.movingAverage();
        data[i].mag.z = filterMagZ.movingAverage();

        data[i].gyro.x = filterGyroX.movingAverage();
        data[i].gyro.y = filterGyroY.movingAverage();
        data[i].gyro.z = filterGyroZ.movingAverage();
    }

    outRoot = Shared::Utils::dataToJson(data);
    std::cout << "Writing to out.json" << std::endl;
    fout << Json::writeString(writer, outRoot);

    return 0;
}
