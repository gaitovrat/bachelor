/*
 * FXOS8700CQ.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXOS8700CQ_H_
#define SENSORS_FXOS8700CQ_H_

#include <optional>
#include <cstdint>

#include "Sensor/BaseSensor.h"
#include "Vec.h"

class FXOS8700CQ : public BaseSensor {
public:
	enum Range {
		G_2 = 0,
		G_4,
		G_8
	};

	struct Data {
		Vec3<uint16_t> accel;
		Vec3<uint16_t> mag;
	};

public:
	FXOS8700CQ(FXOS8700CQ::Range range = Range::G_4);

	virtual ~FXOS8700CQ() = default;

	status_t Init() override;

	uint8_t DeviceAddress() const override;

	std::optional<FXOS8700CQ::Data> Read() const;
private:
	uint8_t m_deviceAddress;
	Range m_range;
};

#endif /* SENSORS_FXOS8700CQ_H_ */