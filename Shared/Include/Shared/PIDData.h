//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_PIDDATA_H
#define UDPSEND_PIDDATA_H

namespace Shared {
struct PIDData {
    double P;
    double I;
    double D;

    double Input;
    double Output;
    double SetPoint;

    PIDData(double p, double i, double d);

    PIDData();
};
}

#endif //UDPSEND_PIDDATA_H
