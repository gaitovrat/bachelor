/*
 * FXAS21002.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXAS21002_H_
#define SENSORS_FXAS21002_H_

#include <optional>

#include "ASensor.h"
#include "Vec.h"

class FXAS21002 : public ASensor {
public:
	enum Range {
		DPS_2000 = 0,
		DPS_1000,
		DPS_500,
		DPS_250
	};

private:
	uint8_t deviceAddress;
	FXAS21002::Range range;

public:

	FXAS21002(FXAS21002::Range range);

	virtual ~FXAS21002() = default;

	status_t init() override;

	uint8_t getDeviceAddress() const override;

	std::optional<Vec3> read() const;
};

#endif /* SENSORS_FXAS21002_H_ */
