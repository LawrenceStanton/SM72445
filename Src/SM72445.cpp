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
	I2C			 &i2c,
	DeviceAddress deviceAddress,
	float		  vInGain,
	float		  vOutGain,
	float		  iInGain,
	float		  iOutGain,
	float		  vDDA
)
	: i2c{i2c}, deviceAddress{deviceAddress},									  //
	  vInGain{vInGain}, vOutGain{vOutGain}, iInGain{iInGain}, iOutGain{iOutGain}, //
	  vDDA{vDDA} {}

optional<array<float, 4>> SM72445::getElectricalMeasurements(void) const {
	auto regValues = getElectricalMeasurementsRegister();

	if (!regValues) return nullopt;

	const array properties = {
		ElectricalProperty::CURRENT_IN,
		ElectricalProperty::VOLTAGE_IN,
		ElectricalProperty::CURRENT_OUT,
		ElectricalProperty::VOLTAGE_OUT,
	};
	array<float, 4> measurements;

	for (auto property : properties) {
		auto adcResult = regValues.value()[property];

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float measurement = convertAdcResultToPinVoltage(adcResult, 10u) / gain;

		measurements[static_cast<uint8_t>(property)] = measurement;
	}

	return measurements;
}

optional<array<float, 4>> SM72445::getAnalogueChannelVoltages(void) const {
	auto regValues = getAnalogueChannelRegister();

	if (!regValues) return nullopt;

	array<float, 4> voltages;
	const array		properties = {
		AnalogueChannel::CH0,
		AnalogueChannel::CH2,
		AnalogueChannel::CH4,
		AnalogueChannel::CH6,
	};

	for (auto property : properties) {
		const uint16_t adcResult = regValues.value()[property];

		const float voltage = convertAdcResultToPinVoltage(adcResult, 10u);

		voltages[static_cast<uint8_t>(property)] = voltage;
	}

	return voltages;
}

optional<array<float, 4>> SM72445::getOffsets(void) const {
	auto regValues = getOffsetRegister();

	if (!regValues) return nullopt;

	const array properties = {
		ElectricalProperty::CURRENT_IN,
		ElectricalProperty::VOLTAGE_IN,
		ElectricalProperty::CURRENT_OUT,
		ElectricalProperty::VOLTAGE_OUT,
	};

	array<float, 4> offsets;

	for (auto property : properties) {
		const uint16_t adcOffset = regValues.value()[property];

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float offset = convertAdcResultToPinVoltage(adcOffset, 8u) / gain;

		offsets[static_cast<uint8_t>(property)] = offset;
	}

	return offsets;
}

optional<array<float, 4>> SM72445::getCurrentThresholds(void) const {
	const auto thresholdRegValues = getThresholdRegister();

	if (!thresholdRegValues) return nullopt;

	array<float, 4> thresholds;
	const array		properties = {
		CurrentThreshold::CURRENT_OUT_LOW,
		CurrentThreshold::CURRENT_OUT_HIGH,
		CurrentThreshold::CURRENT_IN_LOW,
		CurrentThreshold::CURRENT_IN_HIGH,
	};

	for (auto property : properties) {
		const uint16_t adcThreshold = thresholdRegValues.value()[property];

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float threshold = convertAdcResultToPinVoltage(adcThreshold, 10u) / gain;

		thresholds[static_cast<uint8_t>(property)] = threshold;
	}
	return thresholds;
}

template <typename Reg> optional<Reg> SM72445::getRegister(SM72445::MemoryAddress memoryAddress) const {
	auto transmission = this->i2c.read(this->deviceAddress, memoryAddress);

	if (!transmission) return nullopt;

	Reg reg{*transmission};
	return reg;
}

optional<SM72445::Reg0> SM72445::getAnalogueChannelRegister(void) const {
	return getRegister<Reg0>(MemoryAddress::REG0);
}

optional<SM72445::Reg1> SM72445::getElectricalMeasurementsRegister(void) const {
	return getRegister<Reg1>(MemoryAddress::REG1);
}

optional<SM72445::Reg3> SM72445::getConfigRegister(void) const {
	return getRegister<Reg3>(MemoryAddress::REG3);
}

optional<SM72445::Reg4> SM72445::getOffsetRegister(void) const {
	return getRegister<Reg4>(MemoryAddress::REG4);
}

optional<SM72445 ::Reg5> SM72445::getThresholdRegister(void) const {
	return getRegister<Reg5>(MemoryAddress::REG5);
}

float SM72445::convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const {
	// ! adcResult is not checked for valid range with respect to resolution here.
	// Ensure proper masking before calling this function.
	const float maxAdcResult = (1u << resolution) - 1u;
	float		voltage		 = adcResult / maxAdcResult * this->vDDA;
	return voltage;
}

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
