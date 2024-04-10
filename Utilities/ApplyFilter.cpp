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
    Shared::Filter filter;

    if (argc < 2) {
        std::cerr << "Usage: ApplyFilter" << argv[0] << " <data.json>"
                  << std::endl;
        return 1;
    }

    fin.open(argv[1]);
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
    for (const auto &el : data) {
        double acceleration =
            sqrt(pow(el.accel.x, 2) + pow(el.accel.y, 2) + pow(el.accel.z, 2));
        filter.add(static_cast<int16_t>(acceleration));
        outRoot.append(filter.lowPassChebyshev4spole());
    }

    fout << Json::writeString(writer, outRoot);

    return 0;
}
