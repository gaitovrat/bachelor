#include "Shared/PIDData.h"

using namespace Shared;

PIDData::PIDData(double p, double i, double d) : P(p), I(i), D(d), Input(0), SetPoint(0), Output(0) {}

PIDData::PIDData() : PIDData(0, 0, 0) {}
