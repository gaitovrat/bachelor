#ifndef UTILS_H_
#define UTILS_H_

#include <cstdint>

namespace MCU {
constexpr uint8_t ledCenter(const int pot) {
    if (pot < -5) return 0x01;

    if (pot > 5) return 0x2;

    return 0x3;
}
}  // namespace MCU

#endif /* UTILS_H_ */
