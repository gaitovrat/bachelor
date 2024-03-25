//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_PIDDATA_H
#define UDPSEND_PIDDATA_H

namespace Shared {
struct PIDData {
    const double p;
    const double i;
    const double d;

    double input;
    double output;
    double setPoint;

    PIDData(double p, double i, double d);

    PIDData();

    PIDData(const PIDData &other);

    void reset();
};
} // namespace Shared

#endif // UDPSEND_PIDDATA_H
