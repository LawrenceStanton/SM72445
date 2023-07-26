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

optional<float> SM72445::getElectricalMeasurement(ElectricalProperty property) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	uint16_t adcResult = (reg1.value() >> (static_cast<uint8_t>(property) * 10u)) & 0x3FFu;

	float gain = 0.0f;
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		gain = this->iInGain;
		break;
	case ElectricalProperty::VOLTAGE_IN:
		gain = this->vInGain;
		break;
	case ElectricalProperty::CURRENT_OUT:
		gain = this->iOutGain;
		break;
	case ElectricalProperty::VOLTAGE_OUT:
		gain = this->vOutGain;
		break;
	default:
		return nullopt;
	}

	float measurement = convertAdcResultToPinVoltage(adcResult, 10u) / gain;
	return measurement;
}

optional<float> SM72445::getInputCurrent(void) const {
	return getElectricalMeasurement(ElectricalProperty::CURRENT_IN);
}

optional<float> SM72445::getInputVoltage(void) const {
	return getElectricalMeasurement(ElectricalProperty::VOLTAGE_IN);
}

optional<float> SM72445::getOutputCurrent(void) const {
	return getElectricalMeasurement(ElectricalProperty::CURRENT_OUT);
}

optional<float> SM72445::getOutputVoltage(void) const {
	return getElectricalMeasurement(ElectricalProperty::VOLTAGE_OUT);
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
