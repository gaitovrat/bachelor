/*
 * FXAS21002.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXAS21002_H_
#define SENSORS_FXAS21002_H_

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
	FXAS21002(FXAS21002::Range range);

	virtual ~FXAS21002() = default;

	void Init() override;

	uint8_t DeviceAddress() const override;

	Vec3 Read() const;

private:
	uint8_t m_DeviceAddress;
	FXAS21002::Range m_Range;
};

#endif /* SENSORS_FXAS21002_H_ */
