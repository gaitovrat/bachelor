#include "Shared/PIDData.h"

using namespace Shared;

PIDData::PIDData(const double p, const double i, const double d)
    : p(p), i(i), d(d), input(0), output(0), setPoint(0) {}

PIDData::PIDData() : PIDData(.0, .0, .0) {}

PIDData::PIDData(const PIDData &other) : PIDData(other.p, other.i, other.d) {
    this->input = other.input;
    this->output = other.output;
    this->setPoint = other.setPoint;
}

void PIDData::reset() {
    this->input = 0;
    this->output = 0;
    this->setPoint = 0;
}
