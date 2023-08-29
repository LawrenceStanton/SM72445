/**
 ******************************************************************************
 * @file			: SM72445_AnalogueChannels.test.cpp
 * @brief			: SM72445 Tests for Analogue Configuration Channels related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.test.hpp"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Eq;
using ::testing::Return;

using MemoryAddress	  = SM72445::MemoryAddress;
using AnalogueChannel = SM72445::AnalogueChannel;

using std::nullopt;

class SM72445_AnalogueChannels : public SM72445_X_Test {};

TEST_F(SM72445_AnalogueChannels, getAnalogueChannelAdcResultsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFull));

	auto adcResults = sm72445.getAnalogueChannelRegister().value();
	EXPECT_EQ(adcResults[AnalogueChannel::CH0], 0x01EFu);
	EXPECT_EQ(adcResults[AnalogueChannel::CH2], 0x02F3u);
	EXPECT_EQ(adcResults[AnalogueChannel::CH4], 0x009Au);
	EXPECT_EQ(adcResults[AnalogueChannel::CH6], 0x019Eu);
}

TEST_F(
	SM72445_AnalogueChannels,
	getAnalogueChannelAdcResultsReturnsNulloptIfI2CReadFails
) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelRegister(), nullopt);
}

TEST_F(SM72445_AnalogueChannels, getAnalogueChannelVoltagesNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFull));

	auto voltages = sm72445.getAnalogueChannelVoltages().value();
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH0)], 2.4193548f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH2)], 3.6901271f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH4)], 0.7526882f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH6)], 2.0234604f);
}

TEST_F(SM72445_AnalogueChannels, getAnalogueChannelVoltagesReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelVoltages(), nullopt);
}

TEST_F(SM72445_AnalogueChannels, getAnalogueChannelVoltageNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))) //
		.Times(4)
		.WillRepeatedly(Return(0x0123'4567'89AB'CDEFull));

	EXPECT_FLOAT_EQ(
		sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH0).value(),
		2.4193548f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH2).value(),
		3.6901271f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH4).value(),
		0.7526882f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH6).value(),
		2.0234604f
	);
}

TEST_F(SM72445_AnalogueChannels, getAnalogueChannelVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH0), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH2), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH4), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH6), nullopt);
}

TEST_F(
	SM72445_AnalogueChannels,
	getAnalogueChannelVoltageReturnsNulloptIfChannelIsInvalid
) {
	EXPECT_CALL(i2c, read).Times(AnyNumber()).WillRepeatedly(Return(0x0ull));
	EXPECT_EQ(
		sm72445.getAnalogueChannelVoltage(static_cast<AnalogueChannel>(0xFF)),
		nullopt
	);
}
