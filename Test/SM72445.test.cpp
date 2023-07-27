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

// ? getElectricalMeasurements() Normal Tests exercised by public API test cases.

TEST_F(SM72445_Test, getElectricalMeasurementReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getElectricalMeasurement(SM72445::ElectricalProperty::CURRENT_IN), nullopt);
	EXPECT_EQ(sm72445.getElectricalMeasurement(SM72445::ElectricalProperty::VOLTAGE_IN), nullopt);
	EXPECT_EQ(sm72445.getElectricalMeasurement(SM72445::ElectricalProperty::CURRENT_OUT), nullopt);
	EXPECT_EQ(sm72445.getElectricalMeasurement(SM72445::ElectricalProperty::VOLTAGE_OUT), nullopt);
}

TEST_F(SM72445_Test, getElectricalMeasurementReturnsNulloptIfPropertyIsInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0000'0000'0000'0000ul));
	EXPECT_EQ(sm72445.getElectricalMeasurement(static_cast<SM72445::ElectricalProperty>(0xFF)), nullopt);
}

TEST_F(SM72445_Test, getInputCurrentNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1)))
		.WillOnce(Return(~(~Register(0xFC00ul) << 0)))
		.WillOnce(Return(~(~Register(0xF3FFul) << 0)))
		.WillOnce(Return(~(~Register(0xFEAAul) << 0)))
		.WillOnce(Return(~(~Register(0xFD55ul) << 0)));
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
		.WillOnce(Return(~(~Register(0xFC00ul) << 10)))
		.WillOnce(Return(~(~Register(0xF3FFul) << 10)))
		.WillOnce(Return(~(~Register(0xFEAAul) << 10)))
		.WillOnce(Return(~(~Register(0xFD55ul) << 10)));
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
		.WillOnce(Return(~(~Register(0xFC00ul) << 20)))
		.WillOnce(Return(~(~Register(0xF3FFul) << 20)))
		.WillOnce(Return(~(~Register(0xFEAAul) << 20)))
		.WillOnce(Return(~(~Register(0xFD55ul) << 20)));
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
		.WillOnce(Return(~(~Register(0xFC00ul) << 30)))
		.WillOnce(Return(~(~Register(0xF3FFul) << 30)))
		.WillOnce(Return(~(~Register(0xFEAAul) << 30)))
		.WillOnce(Return(~(~Register(0xFD55ul) << 30)));
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOutputVoltage().value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOutputVoltage(), nullopt);
}

TEST_F(SM72445_Test, getAnalogueChannelAdcResultNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))) //
		.Times(4)
		.WillRepeatedly(Return(0x0123'4567'89AB'CDEFull));

	EXPECT_EQ(sm72445.getAnalogueChannelAdcResult(SM72445::AnalogueChannel::CH0).value(), 0x01EFu);
	EXPECT_EQ(sm72445.getAnalogueChannelAdcResult(SM72445::AnalogueChannel::CH2).value(), 0x02F3u);
	EXPECT_EQ(sm72445.getAnalogueChannelAdcResult(SM72445::AnalogueChannel::CH4).value(), 0x009Au);
	EXPECT_EQ(sm72445.getAnalogueChannelAdcResult(SM72445::AnalogueChannel::CH6).value(), 0x019Eu);
}

TEST_F(SM72445_Test, getAnalogueChannelAdcResultReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelAdcResult(SM72445::AnalogueChannel::CH0), nullopt);
}

TEST_F(SM72445_Test, convertAdcResultToPinVoltageNormallyConvertsValue) {
	// ? This test only considers the specific resolution cases of 8 and 10 bits, the only ones used by the SM72445.

	// 8-bit Resolution
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x0000u, 8), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x00FFu, 8), 5.0f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x0055u, 8), 1.6666666f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x00AAu, 8), 3.3333333f);

	// 10-bit Resolution
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x0000u, 10), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x03FFu, 10), 5.0f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x0155u, 10), 1.6666666f);
	EXPECT_FLOAT_EQ(sm72445.convertAdcResultToPinVoltage(0x02AAu, 10), 3.3333333f);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltageNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))) //
		.Times(4)
		.WillRepeatedly(Return(0x0123'4567'89AB'CDEFull));

	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH0).value(), 2.4193548f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH2).value(), 3.6901271f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH4).value(), 0.7526882f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH6).value(), 2.0234604f);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH0), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH2), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH4), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(SM72445::AnalogueChannel::CH6), nullopt);
}

TEST_F(SM72445_Test, getInputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 0)))
		.WillOnce(Return(~(~Register(0xFFul) << 0)))
		.WillOnce(Return(~(~Register(0xAAul) << 0)))
		.WillOnce(Return(~(~Register(0x55ul) << 0)));
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_IN).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_IN).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 8)))
		.WillOnce(Return(~(~Register(0xFFul) << 8)))
		.WillOnce(Return(~(~Register(0xAAul) << 8)))
		.WillOnce(Return(~(~Register(0x55ul) << 8)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_IN).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_IN).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 16)))
		.WillOnce(Return(~(~Register(0xFFul) << 16)))
		.WillOnce(Return(~(~Register(0xAAul) << 16)))
		.WillOnce(Return(~(~Register(0x55ul) << 16)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_OUT).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_OUT).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 24)))
		.WillOnce(Return(~(~Register(0xFFul) << 24)))
		.WillOnce(Return(~(~Register(0xAAul) << 24)))
		.WillOnce(Return(~(~Register(0x55ul) << 24)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_OUT).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_OUT).value(), 3.3333333f);
}

// ? getOffset() Normal Tests exercised by above individual Electrical Property test cases.

TEST_F(SM72445_Test, getOffsetReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::CURRENT_OUT), nullopt);
	EXPECT_EQ(sm72445.getOffset(SM72445::ElectricalProperty::VOLTAGE_OUT), nullopt);
}

TEST_F(SM72445_Test, getOffsetReturnsNulloptIfGivenPropertyInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0ul));
	EXPECT_EQ(sm72445.getOffset(static_cast<SM72445::ElectricalProperty>(0xFFu)), nullopt);
}

TEST_F(SM72445_Test, getOutputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 0)))
		.WillOnce(Return(~(~Register(0x3FFul) << 0)))
		.WillOnce(Return(~(~Register(0x2AAul) << 0)))
		.WillOnce(Return(~(~Register(0x155ul) << 0)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_LOW).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_LOW).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_LOW).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_LOW).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 10)))
		.WillOnce(Return(~(~Register(0x3FFul) << 10)))
		.WillOnce(Return(~(~Register(0x2AAul) << 10)))
		.WillOnce(Return(~(~Register(0x155ul) << 10)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_HIGH).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_HIGH).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_HIGH).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_HIGH).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 20)))
		.WillOnce(Return(~(~Register(0x3FFul) << 20)))
		.WillOnce(Return(~(~Register(0x2AAul) << 20)))
		.WillOnce(Return(~(~Register(0x155ul) << 20)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_LOW).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_LOW).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_LOW).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_LOW).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 30)))
		.WillOnce(Return(~(~Register(0x3FFul) << 30)))
		.WillOnce(Return(~(~Register(0x2AAul) << 30)))
		.WillOnce(Return(~(~Register(0x155ul) << 30)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_HIGH).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_HIGH).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_HIGH).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_HIGH).value(), 3.3333333f);
}

// ? getOutputLowThreshold() Normal Tests exercised by above individual Electrical Property test cases.

TEST_F(SM72445_Test, getOutputLowThresholdReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_IN_LOW), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(SM72445::CurrentThreshold::CURRENT_OUT_LOW), nullopt);
}

TEST_F(SM72445_Test, getOutputLowThresholdReturnsNulloptIfGivenPropertyInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0ul));
	EXPECT_EQ(sm72445.getCurrentThreshold(static_cast<SM72445::CurrentThreshold>(0xFFu)), nullopt);
}
