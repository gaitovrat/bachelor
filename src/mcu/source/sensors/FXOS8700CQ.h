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

#include "Vec.h"
#include "sensors/ASensor.h"

class FXOS8700CQ : public ASensor {
public:
	enum Range {
		G_2 = 0,
		G_4,
		G_8
	};

	struct Data {
		Vec3 accel;
		Vec3 mag;
	};

private:
	uint8_t deviceAddress;
	Range range;

public:
	FXOS8700CQ(FXOS8700CQ::Range range);

	virtual ~FXOS8700CQ() = default;

	status_t init() override;

	uint8_t getDeviceAddress() const override;

	std::optional<FXOS8700CQ::Data> read() const;
};

#endif /* SENSORS_FXOS8700CQ_H_ */
