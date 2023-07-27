/**
 ******************************************************************************
 * @file			: SM72445_Thresholds.test.hpp
 * @brief			: SM72445 Tests for Thresholds related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, getOutputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 0)))
		.WillOnce(Return(~(~Register(0x3FFul) << 0)))
		.WillOnce(Return(~(~Register(0x2AAul) << 0)))
		.WillOnce(Return(~(~Register(0x155ul) << 0)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getOutputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 10)))
		.WillOnce(Return(~(~Register(0x3FFul) << 10)))
		.WillOnce(Return(~(~Register(0x2AAul) << 10)))
		.WillOnce(Return(~(~Register(0x155ul) << 10)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 20)))
		.WillOnce(Return(~(~Register(0x3FFul) << 20)))
		.WillOnce(Return(~(~Register(0x2AAul) << 20)))
		.WillOnce(Return(~(~Register(0x155ul) << 20)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(), 3.3333333f);
}

TEST_F(SM72445_Test, getInputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 30)))
		.WillOnce(Return(~(~Register(0x3FFul) << 30)))
		.WillOnce(Return(~(~Register(0x2AAul) << 30)))
		.WillOnce(Return(~(~Register(0x155ul) << 30)));

	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(), 3.3333333f);
}

// ? getOutputLowThreshold() Normal Tests exercised by above individual Electrical Property test cases.

TEST_F(SM72445_Test, getOutputLowThresholdReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW), nullopt);
}

TEST_F(SM72445_Test, getOutputLowThresholdReturnsNulloptIfGivenPropertyInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0ul));
	EXPECT_EQ(sm72445.getCurrentThreshold(static_cast<CurrentThreshold>(0xFFu)), nullopt);
}
