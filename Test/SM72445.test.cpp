/**
 ******************************************************************************
 * @file			: SM72445.test.cpp
 * @brief			: GTest Unit Testing for SM72445.cpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#define SM72445_GTEST_TESTING

#include "SM72445.hpp"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnArg;

using MemoryAddress = SM72445::I2C::MemoryAddress;
using Register		= SM72445::I2C::Register;

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
	MockedI2C i2c;
	SM72445	  sm72445{&i2c, SM72445::I2C::DeviceAddress::ADDR001, .5f, .5f, .5f, .5f};

	inline void disableI2C(void) {
		ON_CALL(i2c, read).WillByDefault(Return(nullopt));
		ON_CALL(i2c, write).WillByDefault(Return(nullopt));
	}
};

TEST_F(SM72445_Test, constructorAssignsArguments) {
	ASSERT_EQ(sm72445.i2c, &i2c);
	ASSERT_EQ(sm72445.deviceAddress, SM72445::I2C::DeviceAddress::ADDR001);
	ASSERT_EQ(sm72445.vInGain, .5f);
	ASSERT_EQ(sm72445.vOutGain, .5f);
	ASSERT_EQ(sm72445.iInGain, .5f);
	ASSERT_EQ(sm72445.iOutGain, .5f);
	ASSERT_EQ(sm72445.vDDA, 5.0f);
}

TEST_F(SM72445_Test, getInputCurrentNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FC00ul) << 0)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'F3FFul) << 0)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FEAAul) << 0)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FD55ul) << 0)));
	EXPECT_FLOAT_EQ(sm72445.getInputCurrent().value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getInputCurrent().value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getInputCurrent().value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getInputCurrent().value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputCurrentReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getInputCurrent(), nullopt);
}

TEST_F(SM72445_Test, getInputVoltageNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FC00ul) << 10)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'F3FFul) << 10)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FEAAul) << 10)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FD55ul) << 10)));
	EXPECT_FLOAT_EQ(sm72445.getInputVoltage().value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getInputVoltage().value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getInputVoltage().value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getInputVoltage().value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getInputVoltage(), nullopt);
}

TEST_F(SM72445_Test, getOutputCurrentNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FC00ul) << 20)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'F3FFul) << 20)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FEAAul) << 20)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FD55ul) << 20)));
	EXPECT_FLOAT_EQ(sm72445.getOutputCurrent().value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputCurrent().value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputCurrent().value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOutputCurrent().value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputCurrentReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOutputCurrent(), nullopt);
}

TEST_F(SM72445_Test, getOutputVoltageNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FC00ul) << 30)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'F3FFul) << 30)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FEAAul) << 30)))
		.WillOnce(Return(~(~Register(0xFFFF'FFFF'FFFF'FD55ul) << 30)));
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOutputVoltage(), nullopt);
}
