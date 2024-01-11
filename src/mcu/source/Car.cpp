/*
 * Car.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: ratmir
 */

#include <Car.h>

#include <fsl_debug_console.h>

void Car::run() {
	PRINTF("run\r\n");
}

void Car::init() {
	this->fxas.init();
	this->fxos.init();
#ifdef CAR_ENET
	this->enet.init(1024, 8080);
#endif
}
