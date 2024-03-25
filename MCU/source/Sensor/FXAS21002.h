/*
 * FXAS21002.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXAS21002_H_
#define SENSORS_FXAS21002_H_

#include <optional>

#include "Sensor/BaseSensor.h"
#include "Shared/Vec.h"

namespace MCU {
class FXAS21002 : public BaseSensor {
  public:
    enum Range { DPS_2000 = 0, DPS_1000, DPS_500, DPS_250 };

  private:
    uint8_t address;
    FXAS21002::Range range;

  public:
    FXAS21002(FXAS21002::Range range = Range::DPS_1000);

    virtual ~FXAS21002() = default;

    status_t init() override;

    uint8_t deviceAddress() const override;

    std::optional<Shared::Vec3<uint16_t>> read() const;
};
} // namespace MCU

#endif /* SENSORS_FXAS21002_H_ */
