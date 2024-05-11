#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

#include <json/json.h>

#include "Shared/Data.h"
#include "Shared/Filter.h"
#include "Shared/Utils.h"

enum FilterType {
    MOVING_AVERAGE,
    WINDOWED_SINC,
    SINGLE_POLE,
    FOUR_STAGE,
    CHEBYSHEV2,
    CHEBYSHEV4
};

void apply(std::vector<Shared::Data> data, FilterType type) {
    Shared::Filter filterAccelX;
    std::function<int16_t()> filterAccelXFunc;
    Shared::Filter filterAccelY;
    std::function<int16_t()> filterAccelYFunc;
    Shared::Filter filterAccelZ;
    std::function<int16_t()> filterAccelZFunc;
    Shared::Filter filterMagX;
    std::function<int16_t()> filterMagXFunc;
    Shared::Filter filterMagY;
    std::function<int16_t()> filterMagYFunc;
    Shared::Filter filterMagZ;
    std::function<int16_t()> filterMagZFunc;
    Shared::Filter filterGyroX;
    std::function<int16_t()> filterGyroXFunc;
    Shared::Filter filterGyroY;
    std::function<int16_t()> filterGyroYFunc;
    Shared::Filter filterGyroZ;
    std::function<int16_t()> filterGyroZFunc;
    const char *filename;
    Json::Value outRoot;
    Json::StreamWriterBuilder writer;
    std::ofstream fout;

    switch (type) {
    case MOVING_AVERAGE:
        filename = "MovingAverage.json";
        filterAccelXFunc = [&filterAccelX]() {
            return filterAccelX.movingAverage();
        };
        filterAccelYFunc = [&filterAccelY]() {
            return filterAccelY.movingAverage();
        };
        filterAccelZFunc = [&filterAccelZ]() {
            return filterAccelZ.movingAverage();
        };
        filterMagXFunc = [&filterMagX]() { return filterMagX.movingAverage(); };
        filterMagYFunc = [&filterMagY]() { return filterMagY.movingAverage(); };
        filterMagZFunc = [&filterMagZ]() { return filterMagZ.movingAverage(); };
        filterGyroXFunc = [&filterGyroX]() {
            return filterGyroX.movingAverage();
        };
        filterGyroYFunc = [&filterGyroY]() {
            return filterGyroY.movingAverage();
        };
        filterGyroZFunc = [&filterGyroZ]() {
            return filterGyroZ.movingAverage();
        };
        break;
    case SINGLE_POLE:
        filename = "SinglePole.json";
        filterAccelXFunc = [&filterAccelX]() {
            return filterAccelX.singlePoleRecursive();
        };
        filterAccelYFunc = [&filterAccelY]() {
            return filterAccelY.singlePoleRecursive();
        };
        filterAccelZFunc = [&filterAccelZ]() {
            return filterAccelZ.singlePoleRecursive();
        };
        filterMagXFunc = [&filterMagX]() {
            return filterMagX.singlePoleRecursive();
        };
        filterMagYFunc = [&filterMagY]() {
            return filterMagY.singlePoleRecursive();
        };
        filterMagZFunc = [&filterMagZ]() {
            return filterMagZ.singlePoleRecursive();
        };
        filterGyroXFunc = [&filterGyroX]() {
            return filterGyroX.singlePoleRecursive();
        };
        filterGyroYFunc = [&filterGyroY]() {
            return filterGyroY.singlePoleRecursive();
        };
        filterGyroZFunc = [&filterGyroZ]() {
            return filterGyroZ.singlePoleRecursive();
        };
        break;
    case FOUR_STAGE:
        filename = "FourStage.json";
        filterAccelXFunc = [&filterAccelX]() {
            return filterAccelX.recursiveFourStageLowPass();
        };
        filterAccelYFunc = [&filterAccelY]() {
            return filterAccelY.recursiveFourStageLowPass();
        };
        filterAccelZFunc = [&filterAccelZ]() {
            return filterAccelZ.recursiveFourStageLowPass();
        };
        filterMagXFunc = [&filterMagX]() {
            return filterMagX.recursiveFourStageLowPass();
        };
        filterMagYFunc = [&filterMagY]() {
            return filterMagY.recursiveFourStageLowPass();
        };
        filterMagZFunc = [&filterMagZ]() {
            return filterMagZ.recursiveFourStageLowPass();
        };
        filterGyroXFunc = [&filterGyroX]() {
            return filterGyroX.recursiveFourStageLowPass();
        };
        filterGyroYFunc = [&filterGyroY]() {
            return filterGyroY.recursiveFourStageLowPass();
        };
        filterGyroZFunc = [&filterGyroZ]() {
            return filterGyroZ.recursiveFourStageLowPass();
        };
        break;
    case CHEBYSHEV2:
        filename = "Chebyshev2.json";
        filterAccelXFunc = [&filterAccelX]() {
            return filterAccelX.lowPassChebyshev2pole();
        };
        filterAccelYFunc = [&filterAccelY]() {
            return filterAccelY.lowPassChebyshev2pole();
        };
        filterAccelZFunc = [&filterAccelZ]() {
            return filterAccelZ.lowPassChebyshev2pole();
        };
        filterMagXFunc = [&filterMagX]() {
            return filterMagX.lowPassChebyshev2pole();
        };
        filterMagYFunc = [&filterMagY]() {
            return filterMagY.lowPassChebyshev2pole();
        };
        filterMagZFunc = [&filterMagZ]() {
            return filterMagZ.lowPassChebyshev2pole();
        };
        filterGyroXFunc = [&filterGyroX]() {
            return filterGyroX.lowPassChebyshev2pole();
        };
        filterGyroYFunc = [&filterGyroY]() {
            return filterGyroY.lowPassChebyshev2pole();
        };
        filterGyroZFunc = [&filterGyroZ]() {
            return filterGyroZ.lowPassChebyshev2pole();
        };
        break;
    default:
        return;
    }

    fout.open(filename);
    if (!fout.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

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

        data[i].accel.x = filterAccelXFunc();
        data[i].accel.y = filterAccelYFunc();
        data[i].accel.z = filterAccelZFunc();

        data[i].mag.x = filterMagXFunc();
        data[i].mag.y = filterMagYFunc();
        data[i].mag.z = filterMagZFunc();

        data[i].gyro.x = filterGyroXFunc();
        data[i].gyro.y = filterGyroYFunc();
        data[i].gyro.z = filterGyroZFunc();
    }

    outRoot = Shared::Utils::dataToJson(data);
    std::cout << "Writing to " << filename << std::endl;
    fout << Json::writeString(writer, outRoot);
}

int main(const int argc, const char **argv) {
    Json::Reader reader;
    Json::Value root;
    std::ifstream fin;
    std::vector<Shared::Data> data;

    if (argc < 2) {
        std::cerr << "Usage: ApplyFilter" << argv[0] << " <data.json>"
                  << std::endl;
        return 1;
    }

    fin.open(argv[1]);
    std::cout << "Reading from " << argv[1] << std::endl;
    if (!fin.is_open()) {
        std::cerr << "Error: could not open file " << argv[1] << std::endl;
        return 1;
    }
    if (!reader.parse(fin, root, false)) {
        std::cerr << "Error: could not parse JSON file " << argv[1]
                  << std::endl;
        return 1;
    }

    data = Shared::Utils::jsonToData(root);
    for (int i = 0; i < 6; ++i) {
        apply(data, static_cast<FilterType>(i));
    }

    return 0;
}
