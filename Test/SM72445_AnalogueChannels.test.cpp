/**
 ******************************************************************************
 * @file			: SM72445_AnalogueChannels.test.cpp
 * @brief			: SM72445 Tests for Analogue Configuration Channels related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, getAnalogueChannelAdcResultsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))).WillOnce(Return(0x0123'4567'89AB'CDEFull));

	auto adcResults = sm72445.getAnalogueChannelAdcResults().value();

	EXPECT_EQ(adcResults[static_cast<uint8_t>(AnalogueChannel::CH0)], 0x01EFu);
	EXPECT_EQ(adcResults[static_cast<uint8_t>(AnalogueChannel::CH2)], 0x02F3u);
	EXPECT_EQ(adcResults[static_cast<uint8_t>(AnalogueChannel::CH4)], 0x009Au);
	EXPECT_EQ(adcResults[static_cast<uint8_t>(AnalogueChannel::CH6)], 0x019Eu);
}

TEST_F(SM72445_Test, getAnalogueChannelAdcResultsReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelAdcResults(), nullopt);
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

TEST_F(SM72445_Test, getAnalogueChannelVoltagesNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))).WillOnce(Return(0x0123'4567'89AB'CDEFull));

	auto voltages = sm72445.getAnalogueChannelVoltages().value();
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH0)], 2.4193548f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH2)], 3.6901271f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH4)], 0.7526882f);
	EXPECT_FLOAT_EQ(voltages[static_cast<uint8_t>(AnalogueChannel::CH6)], 2.0234604f);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltagesReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelVoltages(), nullopt);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltageNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG0))) //
		.Times(4)
		.WillRepeatedly(Return(0x0123'4567'89AB'CDEFull));

	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH0).value(), 2.4193548f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH2).value(), 3.6901271f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH4).value(), 0.7526882f);
	EXPECT_FLOAT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH6).value(), 2.0234604f);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH0), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH2), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH4), nullopt);
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(AnalogueChannel::CH6), nullopt);
}

TEST_F(SM72445_Test, getAnalogueChannelVoltageReturnsNulloptIfChannelIsInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0ull));
	EXPECT_EQ(sm72445.getAnalogueChannelVoltage(static_cast<AnalogueChannel>(0xFF)), nullopt);
}
