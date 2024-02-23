#ifndef UDPCONTROL_H
#define UDPCONTROL_H

#include <cstdint>

struct UdpControl {
    int16_t Speed;
    int16_t ServoPosition;

    UdpControl() : Speed(0), ServoPosition(0) {}
};

#endif // UDPCONTROL_H
