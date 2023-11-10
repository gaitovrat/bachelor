/*
 * Pin.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include <Pin.h>

Pin::Pin(PORT_Type *pPort, GPIO_Type *pGpio, uint32_t pin,
		const gpio_pin_config_t& gpioConfig, const port_pin_config_t& portConfig) :
	mpPort(pPort), mpGpio(pGpio), mPin(pin),
	mGpioConfig(gpioConfig), mPortConfig(portConfig)
{
	assert(mpPort);
	assert(mpGpio);
}

void Pin::Init()
{
    PORT_SetPinConfig(mpPort, mPin, &mPortConfig);
    GPIO_PinInit(mpGpio, mPin, &mGpioConfig);
}

void Pin::Write(uint8_t value)
{
	GPIO_PinWrite(mpGpio, mPin, value);
}

uint8_t Pin::Read() const
{
	return GPIO_PinRead(mpGpio, mPin);
}
