/*
 * FXAS21002.cpp
 *
 *  Created on: Nov 10, 2023
 *      Author: ratmirgaitov
 */

#include "FXAS21002.h"

#include "pin_mux.h"

static constexpr uint8_t CTRL_REG0 = 0x0DU;
static constexpr uint8_t CTRL_REG1 = 0x13U;
static constexpr uint8_t CTRL_REG3 = 0x15U;
static constexpr uint8_t F_SETUP = 0x09U;
static constexpr uint8_t STATUS = 0x00U;

using namespace MCU;

FXAS21002::FXAS21002(FXAS21002::Range range) : BaseSensor(I2C0),
		m_eviceAddress(0x21), m_range(range) {
}

status_t FXAS21002::Init() {
	status_t status = 0;
	uint8_t value = 0;
	uint8_t active = 0x03U;

	BOARD_InitGYRO_I2CPins();
	BaseSensor::Init();

	// Set stand by mode
	status = ReadRegister(CTRL_REG1, &value, sizeof(value));
	if (status != kStatus_Success) {
		return status;
	}

	value &= ~active;
	status = WriteRegister(CTRL_REG1, &value);
	if (status != kStatus_Success) {
		return status;
	}

	// Disable FIFO
	value = 0;
	status = WriteRegister(F_SETUP, &value);
	if (status != kStatus_Success) {
		return status;
	}

	// Configure auto-increment
	value = 0x8U;
	status = WriteRegister(CTRL_REG3, &value);
	if (status != kStatus_Success) {
		return status;
	}

	// Configure range
	value = m_range;
	status = WriteRegister(CTRL_REG0, &value);
	if (status != kStatus_Success) {
		return status;
	}

	// Set Active mode
	uint8_t dr = 0x04U;
	value = dr | active;
	status = WriteRegister(CTRL_REG1, &value);
	if (status != kStatus_Success) {
		return status;
	}

	return status;
}

uint8_t FXAS21002::DeviceAddress() const {
	return this->m_eviceAddress;
}

std::optional<Shared::Vec3<uint16_t>> FXAS21002::Read() const {
	Shared::Vec3<uint16_t> data;
	uint8_t buffer[7] = { 0 };

	status_t status = ReadRegister(STATUS, buffer, sizeof(buffer));
	if (status != kStatus_Success) {
		return std::nullopt;
	}

	for (size_t i = 1; i < sizeof(buffer);  i += 2) {
		size_t index = i / 2;
		data.Values[index] = ((int16_t)(((buffer[i] << 8U) | buffer[i + 1])));
	}

	return data;
}
