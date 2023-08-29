/**
 ******************************************************************************
 * @file			: SM72445_Offsets.test.cpp
 * @brief			: SM72445 Tests for Offsets related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.test.hpp"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Eq;
using ::testing::Return;

using Register			 = SM72445::Register;
using MemoryAddress		 = SM72445::MemoryAddress;
using ElectricalProperty = SM72445::ElectricalProperty;

using std::nullopt;

class SM72445_Offsets : public SM72445_X_Test {};

TEST_F(SM72445_Offsets, getOffsetRegisterValuesNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFul));

	auto registerValues = sm72445.getOffsetRegister().value();
	EXPECT_EQ(registerValues[ElectricalProperty::CURRENT_IN], 0xEFu);
	EXPECT_EQ(registerValues[ElectricalProperty::VOLTAGE_IN], 0xCDu);
	EXPECT_EQ(registerValues[ElectricalProperty::CURRENT_OUT], 0xABu);
	EXPECT_EQ(registerValues[ElectricalProperty::VOLTAGE_OUT], 0x89u);
}

TEST_F(SM72445_Offsets, getOffsetRegisterValuesReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffsetRegister(), nullopt);
}

TEST_F(SM72445_Offsets, getOffsetsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(0x0123'4567'89AB'CDEFul));
	auto offsets = sm72445.getOffsets().value();
	EXPECT_FLOAT_EQ(
		offsets[static_cast<uint8_t>(ElectricalProperty::CURRENT_IN)],
		9.372549f
	);
	EXPECT_FLOAT_EQ(
		offsets[static_cast<uint8_t>(ElectricalProperty::CURRENT_OUT)],
		6.705883f
	);
	EXPECT_FLOAT_EQ(
		offsets[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_IN)],
		8.039216f
	);
	EXPECT_FLOAT_EQ(
		offsets[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_OUT)],
		5.372549f
	);
}

TEST_F(SM72445_Offsets, getOffsetsReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffsets(), nullopt);
}

TEST_F(SM72445_Offsets, getInputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 0)))
		.WillOnce(Return(~(~Register(0xFFul) << 0)))
		.WillOnce(Return(~(~Register(0xAAul) << 0)))
		.WillOnce(Return(~(~Register(0x55ul) << 0)));
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Offsets, getInputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 8)))
		.WillOnce(Return(~(~Register(0xFFul) << 8)))
		.WillOnce(Return(~(~Register(0xAAul) << 8)))
		.WillOnce(Return(~(~Register(0x55ul) << 8)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Offsets, getOutputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 16)))
		.WillOnce(Return(~(~Register(0xFFul) << 16)))
		.WillOnce(Return(~(~Register(0xAAul) << 16)))
		.WillOnce(Return(~(~Register(0x55ul) << 16)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Offsets, getOutputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 24)))
		.WillOnce(Return(~(~Register(0xFFul) << 24)))
		.WillOnce(Return(~(~Register(0xAAul) << 24)))
		.WillOnce(Return(~(~Register(0x55ul) << 24)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(),
		6.6666666f
	);
	EXPECT_FLOAT_EQ(
		sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(),
		3.3333333f
	);
}

TEST_F(SM72445_Offsets, getOffsetReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT), nullopt);
}

TEST_F(SM72445_Offsets, getOffsetReturnsNulloptIfGivenPropertyInvalid) {
	EXPECT_CALL(i2c, read).Times(AnyNumber()).WillRepeatedly(Return(0x0ull));
	EXPECT_EQ(sm72445.getOffset(static_cast<ElectricalProperty>(0xFFu)), nullopt);
}
