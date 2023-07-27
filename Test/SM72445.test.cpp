/**
 ******************************************************************************
 * @file			: SM72445.test.cpp
 * @brief			: Miscellaneous tests for SM72445.cpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, constructorAssignsArguments) {
	ASSERT_EQ(sm72445.i2c, &i2c);
	ASSERT_EQ(sm72445.deviceAddress, DeviceAddress::ADDR001);
	ASSERT_EQ(sm72445.vInGain, .5f);
	ASSERT_EQ(sm72445.vOutGain, .5f);
	ASSERT_EQ(sm72445.iInGain, .5f);
	ASSERT_EQ(sm72445.iOutGain, .5f);
	ASSERT_EQ(sm72445.vDDA, 5.0f);
}

TEST(SM72445_GainTest, getGainNormallyReturnsCorrespondingGainValue) {
	const float vinGain = 1.0f, voutGain = 2.0f, iinGain = 3.0f, ioutGain = 4.0f;

	SM72445 sm72445Gain{nullptr, DeviceAddress::ADDR001, vinGain, voutGain, iinGain, ioutGain};
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::VOLTAGE_IN), vinGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::VOLTAGE_OUT), voutGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::CURRENT_IN), iinGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(ElectricalProperty::CURRENT_OUT), ioutGain);

	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_IN_HIGH), iinGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_IN_LOW), iinGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_OUT_HIGH), ioutGain);
	EXPECT_FLOAT_EQ(sm72445Gain.getGain(CurrentThreshold::CURRENT_OUT_LOW), ioutGain);
}

TEST(SM72445_GainTest, getGainReturnsZeroIfGivenPropertyInvalid) {
	SM72445 sm72445Gain{nullptr, DeviceAddress::ADDR001, 1.0f, 1.0f, 1.0f, 1.0f};
	EXPECT_EQ(sm72445Gain.getGain(static_cast<ElectricalProperty>(0xFFu)), 0.0f);
	EXPECT_EQ(sm72445Gain.getGain(static_cast<CurrentThreshold>(0xAAu)), 0.0f);
}
