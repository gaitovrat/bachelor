#ifndef IRSENSOR_H_
#define IRSENSOR_H_

#include <cstdint>

#include "Shared/MotorState.h"
#include "tfc.h"

namespace MCU {
class IRSensor {
    static constexpr uint8_t MAX_CROSS_TIMER = 40;
    static constexpr uint32_t WHITE_IR_BOUND = 2000;

  public:
    IRSensor(TFC &tfc);

    virtual ~IRSensor() = default;

    Shared::MotorState CheckState(Shared::MotorState motorState);

  private:
    uint8_t m_lineCrossBit;
    uint8_t m_leftLineCrossTimer;
    uint8_t m_middleLineCrossTimer;
    uint8_t m_rightLineCrossTimer;
    TFC &m_tfc;
};
} // namespace MCU

#endif /* IRSENSOR_H_ */
