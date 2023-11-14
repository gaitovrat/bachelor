/*
 * FXOS8700CQ.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef SENSORS_FXOS8700CQ_H_
#define SENSORS_FXOS8700CQ_H_

#include "sensors/ASensor.h"

#include <vector>
#include <cstdint>

class FXOS8700CQ : public ASensor
{
public:

	FXOS8700CQ();

	virtual ~FXOS8700CQ() = default;

	void Init() override;

	uint8_t WhoAmIRegister() const override;

	const std::vector<uint8_t>& DeviceAddresses() const override;

private:
	std::vector<uint8_t> mAddresses;
};

#endif /* SENSORS_FXOS8700CQ_H_ */
