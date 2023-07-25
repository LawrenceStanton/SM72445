/**
 ******************************************************************************
 * @file			: SM72445.cpp
 * @brief			: Source for SM72445.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

enum class SM72445::I2C::DeviceAddress : uint8_t {
	// ! Note: ADDR000 not supported.
	ADDR001 = 0x1u,
	ADDR010 = 0x2u,
	ADDR011 = 0x3u,
	ADDR100 = 0x4u,
	ADDR101 = 0x5u,
	ADDR110 = 0x6u,
	ADDR111 = 0x7u,
};

enum class MemoryAddress : uint8_t {
	REG0 = 0xE0u, // Analogue Channel Configuration. Read only.
	REG1 = 0xE1u, // Voltage and Current Input/Output Measurements, MPPT Status. Read only.
	REG3 = 0xE3u, // I2C Override Configuration. Read/Write.
	REG4 = 0xE4u, // Voltage and Current Input/Output Offsets. Read/Write.
	REG5 = 0xE5u, // Current Input/Output High/Low Thresholds. Read/Write.
};

SM72445::SM72445(I2C *i2c, DeviceAddress deviceAddress) : i2c(i2c), deviceAddress(deviceAddress) {}
