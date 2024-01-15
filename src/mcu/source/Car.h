/*
 * Car.h
 *
 *  Created on: Jan 11, 2024
 *      Author: ratmir
 */

#ifndef CAR_H_
#define CAR_H_

#include "sensors/FXOS8700CQ.h"
#include "sensors/FXAS21002.h"
#include "Enet.h"
#include "Data.h"

class Car {
	Data data;

	FXAS21002 fxas;
	FXOS8700CQ fxos;
#ifdef CAR_ENET
	Enet enet;
#endif
public:
	virtual ~Car() = default;

	void init();

	void run();

};

#endif /* CAR_H_ */
