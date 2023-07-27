/**
 ******************************************************************************
 * @file			: SM72445.cpp
 * @brief			: Source for SM72445.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using std::nullopt;

float SM72445::getGain(SM72445::ElectricalProperty property) const {
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		return this->iInGain;
	case ElectricalProperty::VOLTAGE_IN:
		return this->vInGain;
	case ElectricalProperty::CURRENT_OUT:
		return this->iOutGain;
	case ElectricalProperty::VOLTAGE_OUT:
		return this->vOutGain;
	default:
		return 0.0;
	}
}

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

	const uint16_t adcResult = (reg1.value() >> (static_cast<uint8_t>(property) * 10u)) & 0x3FFu;

	const float gain = getGain(property);
	if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

	const float measurement = convertAdcResultToPinVoltage(adcResult, 10u) / gain;
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

optional<float> SM72445::getOffset(ElectricalProperty property) const {
	auto reg4 = this->i2c->read(this->deviceAddress, MemoryAddress::REG4);

	if (!reg4) return nullopt;

	const uint16_t adcOffset = (reg4.value() >> (static_cast<uint8_t>(property) * 8u)) & 0xFFu;

	const float gain = getGain(property);
	if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

	const float offset = convertAdcResultToPinVoltage(adcOffset, 8u) / gain;
	return offset;
}

optional<float> SM72445::getCurrentThreshold(CurrentThreshold threshold) const {
	auto reg5 = this->i2c->read(this->deviceAddress, MemoryAddress::REG5);

	if (!reg5) return nullopt;

	const uint16_t adcThreshold = (reg5.value() >> (static_cast<uint8_t>(threshold) * 10u)) & 0x3FFu;

	const float gain =
		(threshold == CurrentThreshold::CURRENT_IN_HIGH || threshold == CurrentThreshold::CURRENT_IN_LOW)
			? getGain(ElectricalProperty::CURRENT_IN)
		: (threshold == CurrentThreshold::CURRENT_OUT_HIGH || threshold == CurrentThreshold::CURRENT_OUT_LOW)
			? getGain(ElectricalProperty::CURRENT_OUT)
			: 0.0f;

	if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

	const float thresholdCurrent = convertAdcResultToPinVoltage(adcThreshold, 10u) / gain;
	return thresholdCurrent;
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
