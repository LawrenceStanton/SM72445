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

#include "SM72445_X.hpp"

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
	SM72445	  sm72445{i2c, SM72445::DeviceAddress::ADDR001};

	void disableI2C() {
		using std::nullopt;
		using ::testing::AnyNumber;
		using ::testing::Return;

		EXPECT_CALL(i2c, read).Times(AnyNumber()).WillRepeatedly(Return(nullopt));
		EXPECT_CALL(i2c, write).Times(AnyNumber()).WillRepeatedly(Return(nullopt));
	}
};
