/*
 * FXOS8700CQ.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXOS8700CQ_H_
#define SENSORS_FXOS8700CQ_H_

#include <cstdint>
#include <optional>

#include "Sensor/BaseSensor.h"
#include "Shared/Vec.h"

namespace MCU {
class FXOS8700CQ : public BaseSensor {
  public:
    enum Range { G_2 = 0, G_4, G_8 };

    struct Data {
        Shared::Vec3<uint16_t> accel;
        Shared::Vec3<uint16_t> mag;
    };

  private:
    uint8_t address;
    Range range;
    Data data;

  public:
    FXOS8700CQ(FXOS8700CQ::Range range = Range::G_4);

    virtual ~FXOS8700CQ() = default;

    status_t init() override;

    uint8_t deviceAddress() const override;

    std::optional<FXOS8700CQ::Data> read() const;

    uint8_t getDataRegister() const override;

    uint8_t *getData() override;

    uint32_t getDataSize() const override;

    status_t setupRegisters() override;
};
} // namespace MCU

#endif /* SENSORS_FXOS8700CQ_H_ */
