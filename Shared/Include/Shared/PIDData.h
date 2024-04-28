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

    /*
     * Constructor
     * @param p the proportional value
     * @param i the integral value
     * @param d the derivative value
     */
    PIDData(double p, double i, double d);

    /*
     * Default constructor
     */
    PIDData();

    /*
     * Destructor
     */
    virtual ~PIDData() = default;

    /*
     * Reset the PID data
     */
    void reset();
};
}  // namespace Shared

#endif  // UDPSEND_PIDDATA_H
