/*
 * FXOS8700CQ.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXOS8700CQ_H_
#define SENSORS_FXOS8700CQ_H_

#include <vector>
#include <cstdint>

#include "Vec.h"
#include "sensors/ASensor.h"

class FXOS8700CQ : public ASensor {
public:
	enum Range {
		G_2 = 0b00,
		G_4 = 0b01,
		G_8 = 0b10
	};

	struct Data {
		Vec3 Accel;
		Vec3 Mag;
	};

	FXOS8700CQ(FXOS8700CQ::Range range);

	virtual ~FXOS8700CQ() = default;

	void Init() override;

	uint8_t DeviceAddress() const override;

	FXOS8700CQ::Data Read() const;

private:
	uint8_t m_DeviceAddress;
	Range m_Range;
};

#endif /* SENSORS_FXOS8700CQ_H_ */
