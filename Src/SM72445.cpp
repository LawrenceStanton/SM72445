/**
 ******************************************************************************
 * @file			: SM72445.cpp
 * @brief			: Source for SM72445.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using std::nullopt;

SM72445::SM72445(
	I2C			 *i2c,
	DeviceAddress deviceAddress,
	float		  vInGain,
	float		  vOutGain,
	float		  iInGain,
	float		  iOutGain,
	float		  vDDA
)
	: i2c(i2c), deviceAddress(deviceAddress),									  //
	  vInGain(vInGain), vOutGain(vOutGain), iInGain(iInGain), iOutGain(iOutGain), //
	  vDDA(vDDA) {}

optional<float> SM72445::getInputVoltage(void) {
	auto reg1 = i2c->read(deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t vInAdcResult = (reg1.value() & 0x3FFu);
	float	 vInReal	  = vInAdcResult / this->vInGain / 0x3FF.0p0 * this->vDDA;
	return vInReal;
}

optional<float> SM72445::getInputCurrent(void) {
	auto reg1 = i2c->read(deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t iInAdcResult = (reg1.value() >> 10u) & 0x3FFu;
	float	 iInReal	  = iInAdcResult / this->iInGain / 0x3FF.0p0 * this->vDDA;
	return iInReal;
}

optional<float> SM72445::getOutputVoltage(void) {
	auto reg1 = i2c->read(deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t vOutAdcResult = (reg1.value() >> 20u) & 0x3FFu;
	float	 vOutReal	   = vOutAdcResult / this->vOutGain / 0x3FF.0p0 * this->vDDA;
	return vOutReal;
}

optional<float> SM72445::getOutputCurrent(void) {
	auto reg1 = i2c->read(deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t iOutAdcResult = (reg1.value() >> 30u) & 0x3FFu;
	float	 iOutReal	   = iOutAdcResult / this->iOutGain / 0x3FF.0p0 * this->vDDA;
	return iOutReal;
}
