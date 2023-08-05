/**
 ******************************************************************************
 * @file			: SM72445_X.cpp
 * @brief			: SM72445_X Extended Features Class Source
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using std::nullopt;

static inline optional<float>
getOptionalIndexOrNullopt(const optional<const array<float, 4>> &measurements, uint8_t index);

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
	return getOptionalIndexOrNullopt(getAnalogueChannelVoltages(), static_cast<uint8_t>(channel));
}

optional<float> SM72445_X::getOffset(ElectricalProperty property) const {
	auto offsets = getOffsets();
	if (!offsets) return nullopt;
	else {
		if (static_cast<uint8_t>(property) >= offsets.value().size()) return nullopt;
		else return offsets.value()[static_cast<uint8_t>(property)];
	}
}

optional<float> SM72445_X::getCurrentThreshold(CurrentThreshold threshold) const {
	return getOptionalIndexOrNullopt(getCurrentThresholds(), static_cast<uint8_t>(threshold));
}

static inline optional<float>
getOptionalIndexOrNullopt(const optional<const array<float, 4>> &measurements, uint8_t index) {
	if (!measurements) return nullopt;
	if (index >= measurements.value().size()) return nullopt;
	return measurements.value()[index];
}
