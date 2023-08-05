/**
 ******************************************************************************
 * @file			: SM72445.test.cpp
 * @brief			: GTest Unit Testing for SM72445.cpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define SM72445_GTEST_TESTING

#include "SM72445.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnArg;

using Register		= SM72445::I2C::Register;
using MemoryAddress = SM72445::I2C::MemoryAddress;
using DeviceAddress = SM72445::I2C::DeviceAddress;

using AnalogueChannel	 = SM72445::AnalogueChannel;
using CurrentThreshold	 = SM72445::CurrentThreshold;
using ElectricalProperty = SM72445::ElectricalProperty;

using std::nullopt;

class MockedI2C : public SM72445::I2C {
public:
	MOCK_METHOD(
		optional<Register>, //
		read,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress),
		(final)
	);
	MOCK_METHOD(
		optional<Register>,
		write,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress, Register data),
		(final)
	);
};

class SM72445_Test : public ::testing::Test {
public:
	MockedI2C i2c{};
	SM72445	  sm72445{i2c, DeviceAddress::ADDR001, .5f, .5f, .5f, .5f};

	void disableI2C(void) {
		ON_CALL(i2c, read).WillByDefault(Return(nullopt));
		ON_CALL(i2c, write).WillByDefault(Return(nullopt));
	}
};

class SM72445_X_Test : public SM72445_Test {
public:
	SM72445_X sm72445{i2c, DeviceAddress::ADDR001, .5f, .5f, .5f, .5f};
};
