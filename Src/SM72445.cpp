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

float SM72445::getGain(SM72445::CurrentThreshold threshold) const {
	switch (threshold) {
	case CurrentThreshold::CURRENT_OUT_LOW:
	case CurrentThreshold::CURRENT_OUT_HIGH:
		return this->iOutGain;
	case CurrentThreshold::CURRENT_IN_LOW:
	case CurrentThreshold::CURRENT_IN_HIGH:
		return this->iInGain;
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

optional<array<float, 4>> SM72445::getElectricalMeasurements(void) const {
	auto reg1 = this->i2c->read(this->deviceAddress, MemoryAddress::REG1);

	if (!reg1) return nullopt;

	const array properties = {
		ElectricalProperty::CURRENT_IN,
		ElectricalProperty::VOLTAGE_IN,
		ElectricalProperty::CURRENT_OUT,
		ElectricalProperty::VOLTAGE_OUT};
	array<float, 4> measurements;

	for (auto property : properties) {
		const uint16_t adcResult = (reg1.value() >> (static_cast<uint8_t>(property) * 10u)) & 0x3FFu;

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float measurement = convertAdcResultToPinVoltage(adcResult, 10u) / gain;

		measurements[static_cast<uint8_t>(property)] = measurement;
	}

	return measurements;
}

static inline optional<float>
getOptionalIndexOrNullopt(const optional<const array<float, 4>> &measurements, uint8_t index) {
	if (!measurements) return nullopt;
	if (index >= measurements.value().size()) return nullopt;
	return measurements.value()[index];
}

optional<float> SM72445::getInputCurrent(void) const {
	return getOptionalIndexOrNullopt( //
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::CURRENT_IN)
	);
}

optional<float> SM72445::getInputVoltage(void) const {
	return getOptionalIndexOrNullopt( //
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::VOLTAGE_IN)
	);
}

optional<float> SM72445::getOutputCurrent(void) const {
	return getOptionalIndexOrNullopt(
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::CURRENT_OUT)
	);
}

optional<float> SM72445::getOutputVoltage(void) const {
	return getOptionalIndexOrNullopt(
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::VOLTAGE_OUT)
	);
}

optional<array<float, 4>> SM72445::getAnalogueChannelVoltages(void) const {
	auto adcResults = getAnalogueChannelAdcResults();

	if (!adcResults) return nullopt;

	array<float, 4> voltages;
	const array		properties = {
		AnalogueChannel::CH0,
		AnalogueChannel::CH2,
		AnalogueChannel::CH4,
		AnalogueChannel::CH6,
	};

	for (auto property : properties) {
		const uint16_t adcResult = adcResults.value()[static_cast<uint8_t>(property)];

		const float voltage = convertAdcResultToPinVoltage(adcResult, 10u);

		voltages[static_cast<uint8_t>(property)] = voltage;
	}

	return voltages;
}

optional<float> SM72445::getAnalogueChannelVoltage(AnalogueChannel channel) const {
	return getOptionalIndexOrNullopt(getAnalogueChannelVoltages(), static_cast<uint8_t>(channel));
}

optional<array<float, 4>> SM72445::getOffsets(void) const {
	const auto reg4 = this->i2c->read(this->deviceAddress, MemoryAddress::REG4);

	if (!reg4) return nullopt;

	const array properties = {
		ElectricalProperty::CURRENT_IN,
		ElectricalProperty::VOLTAGE_IN,
		ElectricalProperty::CURRENT_OUT,
		ElectricalProperty::VOLTAGE_OUT,
	};

	array<float, 4> offsets;

	for (auto property : properties) {
		const uint16_t adcOffset = (reg4.value() >> (static_cast<uint8_t>(property) * 8u)) & 0xFFu;

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float offset						= convertAdcResultToPinVoltage(adcOffset, 8u) / gain;
		offsets[static_cast<uint8_t>(property)] = offset;
	}

	return offsets;
}

optional<float> SM72445::getOffset(ElectricalProperty property) const {
	auto offsets = getOffsets();
	if (!offsets) return nullopt;
	else {
		if (static_cast<uint8_t>(property) >= offsets.value().size()) return nullopt;
		else return offsets.value()[static_cast<uint8_t>(property)];
	}
}

optional<float> SM72445::getCurrentThreshold(CurrentThreshold threshold) const {
	auto reg5 = this->i2c->read(this->deviceAddress, MemoryAddress::REG5);

	if (!reg5) return nullopt;

	const uint16_t adcThreshold = (reg5.value() >> (static_cast<uint8_t>(threshold) * 10u)) & 0x3FFu;

	const float gain = getGain(threshold);

	if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

	const float thresholdCurrent = convertAdcResultToPinVoltage(adcThreshold, 10u) / gain;
	return thresholdCurrent;
}

optional<array<uint16_t, 4>> SM72445::getAnalogueChannelAdcResults(void) const {
	auto reg0 = this->i2c->read(this->deviceAddress, MemoryAddress::REG0);

	if (!reg0) return nullopt;

	array<uint16_t, 4> adcResults;
	const array		   channels = {
		   AnalogueChannel::CH0,
		   AnalogueChannel::CH2,
		   AnalogueChannel::CH4,
		   AnalogueChannel::CH6,
	   };

	for (auto channel : channels) {
		const uint16_t adcResult				  = (reg0.value() >> static_cast<uint8_t>(channel) * 10u) & 0x3FFu;
		adcResults[static_cast<uint8_t>(channel)] = adcResult;
	}

	return adcResults;
}

float SM72445::convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const {
	// ! adcResult is not checked for valid range with respect to resolution here.
	// Ensure proper masking before calling this function.
	const float maxAdcResult = (1u << resolution) - 1u;
	float		voltage		 = adcResult / maxAdcResult * this->vDDA;
	return voltage;
}
