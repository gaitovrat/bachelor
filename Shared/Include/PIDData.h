//
// Created by ratmirgaitov on 6/14/23.
//

#ifndef UDPSEND_PIDDATA_H
#define UDPSEND_PIDDATA_H

struct PIDData {
    double P;
    double I;
    double D;

    double Input;
    double Output;
    double SetPoint;
};

#endif //UDPSEND_PIDDATA_H
