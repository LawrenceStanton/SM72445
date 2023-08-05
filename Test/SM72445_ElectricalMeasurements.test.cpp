/**
 ******************************************************************************
 * @file			: SM72445_ElectricalMeasurements.test.cpp
 * @brief			: Tests for SM72445 Electrical Measurements related methods.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST_F(SM72445_Test, getElectricalMeasurementAdcResultsNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1))).WillOnce(Return(0x0123'4567'89AB'CDEFul));

	auto adcResults = sm72445.getElectricalMeasurementsAdcResults().value();
	EXPECT_EQ(adcResults[ElectricalProperty::CURRENT_IN], 0x01EFu);
	EXPECT_EQ(adcResults[ElectricalProperty::VOLTAGE_IN], 0x02F3u);
	EXPECT_EQ(adcResults[ElectricalProperty::CURRENT_OUT], 0x009Au);
	EXPECT_EQ(adcResults[ElectricalProperty::VOLTAGE_OUT], 0x019Eu);
}

TEST_F(SM72445_Test, getElectricalMeasurementAdcResultsReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getElectricalMeasurementsAdcResults(), nullopt);
}

TEST_F(SM72445_Test, getElectricalMeasurementNormallyReturnsValue) {
	EXPECT_CALL(i2c, read(_, Eq(MemoryAddress::REG1))).WillOnce(Return(0x0123'4567'89AB'CDEFul));
	auto measurements = sm72445.getElectricalMeasurements().value();
	EXPECT_FLOAT_EQ(measurements[static_cast<uint8_t>(ElectricalProperty::CURRENT_IN)], 4.838709f);
	EXPECT_FLOAT_EQ(measurements[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_IN)], 7.380254f);
	EXPECT_FLOAT_EQ(measurements[static_cast<uint8_t>(ElectricalProperty::CURRENT_OUT)], 1.505376f);
	EXPECT_FLOAT_EQ(measurements[static_cast<uint8_t>(ElectricalProperty::VOLTAGE_OUT)], 4.046921f);
}

TEST_F(SM72445_Test, getElectricalMeasurementReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getElectricalMeasurements(), nullopt);
}

TEST_F(SM72445_X_Test, getInputCurrentNormallyReturnsValue) {
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

TEST_F(SM72445_X_Test, getInputCurrentReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getInputCurrent(), nullopt);
}

TEST_F(SM72445_X_Test, getInputVoltageNormallyReturnsValue) {
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

TEST_F(SM72445_X_Test, getInputVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getInputVoltage(), nullopt);
}

TEST_F(SM72445_X_Test, getOutputCurrentNormallyReturnsValue) {
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

TEST_F(SM72445_X_Test, getOutputCurrentReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOutputCurrent(), nullopt);
}

TEST_F(SM72445_X_Test, getOutputVoltageNormallyReturnsValue) {
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

TEST_F(SM72445_X_Test, getOutputVoltageReturnsNulloptIfI2CReadFails) {
	disableI2C();
	EXPECT_EQ(sm72445.getOutputVoltage(), nullopt);
}
