/**
 ******************************************************************************
 * @file			: SM72445_Thresholds.test.hpp
 * @brief			: SM72445 Tests for Thresholds related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.test.hpp"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Eq;
using ::testing::Return;

using Register		= SM72445::Register;
using MemoryAddress = SM72445::MemoryAddress;

using CurrentThreshold = SM72445::CurrentThreshold;

using std::nullopt;

class SM72445_Thresholds : public SM72445_X_Test {};

TEST_F(SM72445_Thresholds, getThresholdRegisterValuesNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFul));

	auto thresholdRegisterValues = sm72445.getThresholdRegister().value();
	EXPECT_EQ(thresholdRegisterValues[CurrentThreshold::CURRENT_OUT_LOW], 0x01EFu);
	EXPECT_EQ(thresholdRegisterValues[CurrentThreshold::CURRENT_OUT_HIGH], 0x02F3u);
	EXPECT_EQ(thresholdRegisterValues[CurrentThreshold::CURRENT_IN_LOW], 0x009Au);
	EXPECT_EQ(thresholdRegisterValues[CurrentThreshold::CURRENT_IN_HIGH], 0x019Eu);
}

TEST_F(SM72445_Thresholds, getThresholdRegisterValuesReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getThresholdRegister(), nullopt);
}

TEST_F(SM72445_Thresholds, getCurrentThresholdsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFull));

	auto thresholds = sm72445.getCurrentThresholds().value();
	EXPECT_FLOAT_EQ(
		thresholds[static_cast<uint8_t>(CurrentThreshold::CURRENT_OUT_LOW)],
		4.838709f
	);
	EXPECT_FLOAT_EQ(
		thresholds[static_cast<uint8_t>(CurrentThreshold::CURRENT_OUT_HIGH)],
		7.380254f
	);
	EXPECT_FLOAT_EQ(
		thresholds[static_cast<uint8_t>(CurrentThreshold::CURRENT_IN_LOW)],
		1.505376f
	);
	EXPECT_FLOAT_EQ(
		thresholds[static_cast<uint8_t>(CurrentThreshold::CURRENT_IN_HIGH)],
		4.046921f
	);
}

TEST_F(SM72445_Thresholds, getCurrentThresholdsReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getCurrentThresholds(), nullopt);
}

TEST_F(SM72445_X_Test, getOutputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 0)))
		.WillOnce(Return(~(~Register(0x3FFul) << 0)))
		.WillOnce(Return(~(~Register(0x2AAul) << 0)))
		.WillOnce(Return(~(~Register(0x155ul) << 0)));

	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(),
		0.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(),
		10.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Thresholds, getOutputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 10)))
		.WillOnce(Return(~(~Register(0x3FFul) << 10)))
		.WillOnce(Return(~(~Register(0x2AAul) << 10)))
		.WillOnce(Return(~(~Register(0x155ul) << 10)));

	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(),
		0.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(),
		10.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Thresholds, getInputLowCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 20)))
		.WillOnce(Return(~(~Register(0x3FFul) << 20)))
		.WillOnce(Return(~(~Register(0x2AAul) << 20)))
		.WillOnce(Return(~(~Register(0x155ul) << 20)));

	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(),
		0.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(),
		10.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Thresholds, getInputHighCurrentThresholdNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG5)))
		.WillOnce(Return(~(~Register(0x000ul) << 30)))
		.WillOnce(Return(~(~Register(0x3FFul) << 30)))
		.WillOnce(Return(~(~Register(0x2AAul) << 30)))
		.WillOnce(Return(~(~Register(0x155ul) << 30)));

	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(),
		0.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(),
		10.0f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Thresholds, getOutputLowThresholdReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_IN_LOW), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_HIGH), nullopt);
	EXPECT_EQ(sm72445.getCurrentThreshold(CurrentThreshold::CURRENT_OUT_LOW), nullopt);
}

TEST_F(SM72445_Thresholds, getOutputLowThresholdReturnsNulloptIfGivenPropertyInvalid) {
	EXPECT_CALL(i2c, read).Times(AnyNumber()).WillRepeatedly(Return(0x0ull));
	EXPECT_EQ(sm72445.getCurrentThreshold(static_cast<CurrentThreshold>(0xFFu)), nullopt);
}
