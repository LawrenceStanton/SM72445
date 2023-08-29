/**
 ******************************************************************************
 * @file			: SM72445_X.cpp
 * @brief			: SM72445_X Extended Features Class Source
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.hpp"

using Register = SM72445::Register;

using Config = SM72445_X::Config;

using std::nullopt;

SM72445_X::SM72445_X(
	I2C			 &i2c,
	DeviceAddress deviceAddress,
	float		  vInGain,
	float		  vOutGain,
	float		  iInGain,
	float		  iOutGain,
	float		  vDDA
)
	: SM72445(i2c, deviceAddress),												  //
	  vInGain{vInGain}, vOutGain{vOutGain}, iInGain{iInGain}, iOutGain{iOutGain}, //
	  vDDA{vDDA} {}

optional<Config> SM72445_X::getConfig(void) const {
	auto regValues = getConfigRegister();

	if (!regValues) return nullopt;

	Config config(*this, *regValues);
	return config;
}

optional<Register> SM72445_X::setConfig(ConfigRegister configRegister) const {
	return this->i2c.write(this->deviceAddress, MemoryAddress::REG3, configRegister);
}

static inline optional<float> getOptionalIndexOrNullopt(
	const optional<const array<float, 4>> &measurements,
	uint8_t								   index
);

optional<float> SM72445_X::getInputCurrent(void) const {
	return getOptionalIndexOrNullopt( //
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::CURRENT_IN)
	);
}

optional<float> SM72445_X::getInputVoltage(void) const {
	return getOptionalIndexOrNullopt( //
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::VOLTAGE_IN)
	);
}

optional<float> SM72445_X::getOutputCurrent(void) const {
	return getOptionalIndexOrNullopt(
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::CURRENT_OUT)
	);
}

optional<float> SM72445_X::getOutputVoltage(void) const {
	return getOptionalIndexOrNullopt(
		getElectricalMeasurements(),
		static_cast<uint8_t>(ElectricalProperty::VOLTAGE_OUT)
	);
}

optional<float> SM72445_X::getAnalogueChannelVoltage(AnalogueChannel channel) const {
	return getOptionalIndexOrNullopt(
		getAnalogueChannelVoltages(),
		static_cast<uint8_t>(channel)
	);
}

optional<float> SM72445_X::getOffset(ElectricalProperty property) const {
	auto offsets = getOffsets();
	if (!offsets) return nullopt;
	else {
		if (static_cast<uint8_t>(property) >= (*offsets).size()) return nullopt;
		else return offsets.value()[static_cast<uint8_t>(property)];
	}
}

optional<float> SM72445_X::getCurrentThreshold(CurrentThreshold threshold) const {
	return getOptionalIndexOrNullopt(
		getCurrentThresholds(),
		static_cast<uint8_t>(threshold)
	);
}

static inline optional<float> getOptionalIndexOrNullopt(
	const optional<const array<float, 4>> &measurements,
	uint8_t								   index
) {
	if (!measurements) return nullopt;
	if (index >= measurements.value().size()) return nullopt;
	return (*measurements)[index];
}

optional<array<float, 4>> SM72445_X::getElectricalMeasurements(void) const {
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

optional<array<float, 4>> SM72445_X::getAnalogueChannelVoltages(void) const {
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
		const uint16_t adcResult = (*regValues)[property];

		const float voltage = convertAdcResultToPinVoltage(adcResult, 10u);

		voltages[static_cast<uint8_t>(property)] = voltage;
	}

	return voltages;
}

optional<array<float, 4>> SM72445_X::getOffsets(void) const {
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
		const uint16_t adcOffset = (*regValues)[property];

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float offset = convertAdcResultToPinVoltage(adcOffset, 8u) / gain;

		offsets[static_cast<uint8_t>(property)] = offset;
	}

	return offsets;
}

optional<array<float, 4>> SM72445_X::getCurrentThresholds(void) const {
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
		const uint16_t adcThreshold = (*thresholdRegValues)[property];

		const float gain = getGain(property);
		if (gain == 0.0f) return nullopt; // Protect against divide by zero error.

		const float threshold = convertAdcResultToPinVoltage(adcThreshold, 10u) / gain;

		thresholds[static_cast<uint8_t>(property)] = threshold;
	}

	return thresholds;
}

SM72445_X::ConfigBuilder SM72445_X::getConfigBuilder(bool fetchCurrentConfig) const {
	if (fetchCurrentConfig) {
		auto regValues = getConfigRegister();

		if (regValues) return ConfigBuilder(*this, *regValues);
	}
	ConfigBuilder configBuilder(*this);
	return configBuilder;
}

float SM72445_X::convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution)
	const {
	// ! adcResult is not checked for valid range with respect to resolution here.
	// Ensure proper masking before calling this function.
	const float maxAdcResult = (1u << resolution) - 1u;
	float		voltage		 = adcResult / maxAdcResult * this->vDDA;
	return voltage;
}

float SM72445_X::getGain(SM72445::ElectricalProperty property) const {
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

float SM72445_X::getGain(SM72445::CurrentThreshold threshold) const {
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
