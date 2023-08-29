/**
 ******************************************************************************
 * @file			: SM72445.test.cpp
 * @brief			: Miscellaneous tests for SM72445.cpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, constructorAssignsArguments) {
	ASSERT_EQ(&sm72445.i2c, &i2c);
	ASSERT_EQ(sm72445.deviceAddress, DeviceAddress::ADDR001);
	ASSERT_EQ(sm72445.vInGain, .5f);
	ASSERT_EQ(sm72445.vOutGain, .5f);
	ASSERT_EQ(sm72445.iInGain, .5f);
	ASSERT_EQ(sm72445.iOutGain, .5f);
	ASSERT_EQ(sm72445.vDDA, 5.0f);
}

TEST_F(SM72445_Test, convertAdcResultToPinVoltageNormallyConvertsValue) {
	// ? This test only considers the specific resolution cases of 8 and 10 bits, the only
	// ones used by the SM72445.

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

TEST(SM72445_GainTest, getGainNormallyReturnsCorrespondingGainValue) {
	const float vInGain = 1.0f, vOutGain = 2.0f, iInGain = 3.0f, iOutGain = 4.0f;

	SM72445_X sm72445Gain{
		*(SM72445::I2C *)(nullptr),
		DeviceAddress::ADDR001,
		vInGain,
		vOutGain,
		iInGain,
		iOutGain};
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::VOLTAGE_IN), vInGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::VOLTAGE_OUT), vOutGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::CURRENT_IN), iInGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::CURRENT_OUT), iOutGain);

	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_IN_HIGH), iInGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_IN_LOW), iInGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_OUT_HIGH), iOutGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_OUT_LOW), iOutGain);
}

TEST(SM72445_GainTest, getGainReturnsZeroIfGivenPropertyInvalid) {
	SM72445_X sm72445Gain{
		*(SM72445::I2C *)(nullptr),
		DeviceAddress::ADDR001,
		1.0f,
		1.0f,
		1.0f,
		1.0f};
	EXPECT_EQ(sm72445Gain.getGain(static_cast<ElectricalProperty>(0xFFu)), 0.0f);
	EXPECT_EQ(sm72445Gain.getGain(static_cast<CurrentThreshold>(0xAAu)), 0.0f);
}
