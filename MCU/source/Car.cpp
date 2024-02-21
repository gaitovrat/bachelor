/*
 * Car.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: ratmir
 */

#include <Car.h>

#include <fsl_debug_console.h>

extern "C" {
	int DbgConsole_SendData(uint8_t *ch, size_t size);
}

void Car::run() {
	std::optional<FXOS8700CQ::Data> fxosData = this->fxos.read();
	if (fxosData.has_value()) {
		this->data.accel = fxosData->accel;
		this->data.mag = fxosData->mag;
	}

	std::optional<Vec3> fxasData = this->fxas.read();
	if (fxasData.has_value()) {
		this->data.gyro = *fxasData;
	}
	DbgConsole_SendData((uint8_t *)&this->data, sizeof(this->data));
}

void Car::init() {
	this->tfs.InitAll();
	this->tfs.setLEDs(0b0001);
	this->fxas.init();
	this->fxos.init();
#ifdef CAR_ENET
	this->enet.init(1024, 8080);
#endif

	this->tfs.setLEDs(0b1111);
}
