/*
 * Pin.h
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#ifndef PIN_H_
#define PIN_H_

#include <fsl_port.h>
#include <fsl_gpio.h>
#include <stdint.h>

class Pin
{
public:
	Pin(PORT_Type *PPort, GPIO_Type *PGpio, uint32_t pin,
			const gpio_pin_config_t& gpioConfig, const port_pin_config_t& portConfig);

	virtual ~Pin() = default;

	void Init();

	void Write(uint8_t value);

	uint8_t Read() const;

private:
	PORT_Type *mpPort;
	GPIO_Type *mpGpio;
	const uint32_t mPin;

	gpio_pin_config_t mGpioConfig;
	port_pin_config_t mPortConfig;
};

#endif /* PIN_H_ */
