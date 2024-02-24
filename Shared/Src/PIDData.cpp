#include "PIDData.h"

PIDData::PIDData(double p, double i, double d) : P(p), I(i), D(d) {}

PIDData::PIDData() : PIDData(0, 0, 0) {}
