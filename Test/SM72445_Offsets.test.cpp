/**
 ******************************************************************************
 * @file			: SM72445_Offsets.test.cpp
 * @brief			: SM72445 Tests for Offsets related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, getOffsetRegisterValuesNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4))).WillOnce(Return(0x0123'4567'89AB'CDEFul));

	auto registerValues = sm72445.getOffsetRegister().value();
	EXPECT_EQ(registerValues[ElectricalProperty::CURRENT_IN], 0xEFu);
	EXPECT_EQ(registerValues[ElectricalProperty::VOLTAGE_IN], 0xCDu);
	EXPECT_EQ(registerValues[ElectricalProperty::CURRENT_OUT], 0xABu);
	EXPECT_EQ(registerValues[ElectricalProperty::VOLTAGE_OUT], 0x89u);
}

TEST_F(SM72445_Test, getOffsetRegisterValuesReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffsetRegister(), nullopt);
}

TEST_F(SM72445_Test, getOffsetsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4))).WillOnce(Return(0x0123'4567'89AB'CDEFul));
	auto offsets = sm72445.getOffsets().value();
	EXPECT_FLOAT_EQ(offsets[static_cast<uint8_t>(ElectricalProperty::CURRENT_IN)], 9.372549f);
	EXPECT_FLOAT_EQ(offsets[static_cast<uint8_t>(ElectricalProperty::CURRENT_OUT)], 6.705883f);
	EXPECT_FLOAT_EQ(offsets[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_IN)], 8.039216f);
	EXPECT_FLOAT_EQ(offsets[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_OUT)], 5.372549f);
}

TEST_F(SM72445_Test, getOffsetsReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffsets(), nullopt);
}

TEST_F(SM72445_X_Test, getInputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 0)))
		.WillOnce(Return(~(~Register(0xFFul) << 0)))
		.WillOnce(Return(~(~Register(0xAAul) << 0)))
		.WillOnce(Return(~(~Register(0x55ul) << 0)));
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN).value(), 3.3333333f);
}

TEST_F(SM72445_X_Test, getInputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 8)))
		.WillOnce(Return(~(~Register(0xFFul) << 8)))
		.WillOnce(Return(~(~Register(0xAAul) << 8)))
		.WillOnce(Return(~(~Register(0x55ul) << 8)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN).value(), 3.3333333f);
}

TEST_F(SM72445_X_Test, getOutputCurrentOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 16)))
		.WillOnce(Return(~(~Register(0xFFul) << 16)))
		.WillOnce(Return(~(~Register(0xAAul) << 16)))
		.WillOnce(Return(~(~Register(0x55ul) << 16)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT).value(), 3.3333333f);
}

TEST_F(SM72445_X_Test, getOutputVoltageOffsetNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG4)))
		.WillOnce(Return(~(~Register(0x00ul) << 24)))
		.WillOnce(Return(~(~Register(0xFFul) << 24)))
		.WillOnce(Return(~(~Register(0xAAul) << 24)))
		.WillOnce(Return(~(~Register(0x55ul) << 24)));

	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 0.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 10.0f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 6.6666666f);
	EXPECT_FLOAT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT).value(), 3.3333333f);
}

TEST_F(SM72445_X_Test, getOffsetReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_IN), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::CURRENT_OUT), nullopt);
	EXPECT_EQ(sm72445.getOffset(ElectricalProperty::VOLTAGE_OUT), nullopt);
}

TEST_F(SM72445_X_Test, getOffsetReturnsNulloptIfGivenPropertyInvalid) {
	ON_CALL(i2c, read).WillByDefault(Return(0x0ul));
	EXPECT_EQ(sm72445.getOffset(static_cast<ElectricalProperty>(0xFFu)), nullopt);
}
