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

optional<float> SM72445::getInputCurrent(void) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t iInAdcResult = (reg1.value() >> 0) & 0x3FFu;
	float	 iInReal	  = convertAdcResultToPinVoltage(iInAdcResult, 10u) / this->iInGain;
	return iInReal;
}

optional<float> SM72445::getInputVoltage(void) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t vInAdcResult = (reg1.value() >> 10) & 0x3FFu;
	float	 vInReal	  = convertAdcResultToPinVoltage(vInAdcResult, 10u) / this->vInGain;
	return vInReal;
}

optional<float> SM72445::getOutputCurrent(void) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t iOutAdcResult = (reg1.value() >> 20u) & 0x3FFu;
	float	 iOutReal	   = convertAdcResultToPinVoltage(iOutAdcResult, 10u) / this->iOutGain;
	return iOutReal;
}

optional<float> SM72445::getOutputVoltage(void) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t vOutAdcResult = (reg1.value() >> 30u) & 0x3FFu;
	float	 vOutReal	   = convertAdcResultToPinVoltage(vOutAdcResult, 10u) / this->vOutGain;
	return vOutReal;
}

optional<float> SM72445::getAnalogueChannelVoltage(AnalogueChannel channel) const {
	const auto adcResult = getAnalogueChannelAdcResult(channel);

	if (!adcResult) return nullopt;

	const float voltage = convertAdcResultToPinVoltage(adcResult.value(), 10u);
	return voltage;
}

optional<uint16_t> SM72445::getAnalogueChannelAdcResult(AnalogueChannel channel) const {
	auto reg0 = this->i2c->read(this->deviceAddress, MemoryAddress::REG0);

	if (!reg0) return nullopt;

	uint16_t adcResult = (reg0.value() >> (static_cast<uint8_t>(channel) >> 1) * 10u) & 0x3FFu;
	return adcResult;
}

float SM72445::convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const {
	// ! adcResult is not checked for valid range with respect to resolution here.
	// Ensure proper masking before calling this function.
	const float maxAdcResult = (1u << resolution) - 1u;
	float		voltage		 = adcResult / maxAdcResult * this->vDDA;
	return voltage;
}
