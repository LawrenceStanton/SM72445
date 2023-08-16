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

SM72445::Reg0::Reg0(Register reg)
	: ADC0(static_cast<uint16_t>((reg >> 00u) & 0x3FFu)), //
	  ADC2(static_cast<uint16_t>((reg >> 10u) & 0x3FFu)), //
	  ADC4(static_cast<uint16_t>((reg >> 20u) & 0x3FFu)), //
	  ADC6(static_cast<uint16_t>((reg >> 30u) & 0x3FFu)) {}

SM72445::Reg0::operator Register() const {
	auto reg = static_cast<Register>(this->ADC0) << 0u	//
			 | static_cast<Register>(this->ADC2) << 10u //
			 | static_cast<Register>(this->ADC4) << 20u //
			 | static_cast<Register>(this->ADC6) << 30u;
	return reg;
}

uint16_t SM72445::Reg0::operator[](AnalogueChannel channel) const {
	switch (channel) {
	case AnalogueChannel::CH0:
		return this->ADC0;
	case AnalogueChannel::CH2:
		return this->ADC2;
	case AnalogueChannel::CH4:
		return this->ADC4;
	case AnalogueChannel::CH6:
		return this->ADC6;
	default:
		return 0;
	}
}

SM72445::Reg1::Reg1(Register reg)
	: iIn{static_cast<uint16_t>((reg >> 00u) & 0x3FFu)},  //
	  vIn{static_cast<uint16_t>((reg >> 10u) & 0x3FFu)},  //
	  iOut{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)}, //
	  vOut{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)} {}

SM72445::Reg1::operator Register() const {
	auto reg = static_cast<Register>(this->iIn) << 0u	//
			 | static_cast<Register>(this->vIn) << 10u	//
			 | static_cast<Register>(this->iOut) << 20u //
			 | static_cast<Register>(this->vOut) << 30u;
	return reg;
}

uint16_t SM72445::Reg1::operator[](ElectricalProperty property) const {
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		return this->iIn;
	case ElectricalProperty::VOLTAGE_IN:
		return this->vIn;
	case ElectricalProperty::CURRENT_OUT:
		return this->iOut;
	case ElectricalProperty::VOLTAGE_OUT:
		return this->vOut;
	default:
		return 0u;
	}
}
SM72445::Reg3::Reg3(Register reg)
	: overrideAdcProgramming{!!((reg >> 46u) & 0x1u)},		 //
	  a2Override{static_cast<uint8_t>((reg >> 40u) & 0x7u)}, //
	  iOutMax{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)}, //
	  vOutMax{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)}, //
	  tdOff{static_cast<uint8_t>((reg >> 17u) & 0x7u)},		 //
	  tdOn{static_cast<uint8_t>((reg >> 14u) & 0x7u)},		 //
	  dcOpen{static_cast<uint16_t>((reg >> 5u) & 0x1FFu)},	 //
	  passThroughSelect{!!((reg >> 4u) & 0x1u)},			 //
	  passThroughManual{!!((reg >> 3u) & 0x1u)},			 //
	  bbReset{!!((reg >> 2u) & 0x1u)},						 //
	  clkOeManual{!!((reg >> 1u) & 0x1u)},					 //
	  openLoopOperation{!!(reg & 0x1u)} {}

SM72445::Reg3::operator Register() const {
	const Register reg = (static_cast<Register>(this->overrideAdcProgramming & 0x1u) << 46u)
					   | (static_cast<Register>(0b1ull << 43u))						   // Reserved bit
					   | (static_cast<Register>(this->a2Override) << 40u)			   //
					   | (static_cast<Register>(this->iOutMax) << 30u)				   //
					   | (static_cast<Register>(this->vOutMax) << 20u)				   //
					   | (static_cast<Register>(this->tdOff) << 17u)				   //
					   | (static_cast<Register>(this->tdOn) << 14u)					   //
					   | (static_cast<Register>(this->dcOpen) << 5u)				   //
					   | (static_cast<Register>(this->passThroughSelect & 0x1u) << 4u) //
					   | (static_cast<Register>(this->passThroughManual & 0x1u) << 3u) //
					   | (static_cast<Register>(this->bbReset & 0x1u) << 2u)		   //
					   | (static_cast<Register>(this->clkOeManual & 0x1u) << 1u)	   //
					   | (static_cast<Register>(this->openLoopOperation & 0x1u));
	return reg;
};

SM72445::Reg4::Reg4(Register reg)
	: iInOffset{static_cast<uint8_t>((reg >> 0u) & 0xFFu)},	  //
	  vInOffset{static_cast<uint8_t>((reg >> 8u) & 0xFFu)},	  //
	  iOutOffset{static_cast<uint8_t>((reg >> 16u) & 0xFFu)}, //
	  vOutOffset{static_cast<uint8_t>((reg >> 24u) & 0xFFu)} {}

SM72445::Reg4::operator Register() const {
	return static_cast<Register>(static_cast<Register>(this->iInOffset) << 0u)	 //
		 | static_cast<Register>(static_cast<Register>(this->vInOffset) << 8u)	 //
		 | static_cast<Register>(static_cast<Register>(this->iOutOffset) << 16u) //
		 | static_cast<Register>(static_cast<Register>(this->vOutOffset) << 24u);
}

uint8_t SM72445::Reg4::operator[](ElectricalProperty property) const {
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		return this->iInOffset;
	case ElectricalProperty::VOLTAGE_IN:
		return this->vInOffset;
	case ElectricalProperty::CURRENT_OUT:
		return this->iOutOffset;
	case ElectricalProperty::VOLTAGE_OUT:
		return this->vOutOffset;
	default:
		return 0u;
	}
}

SM72445::Reg5::Reg5(Register reg)
	: iOutLow{static_cast<uint16_t>((reg >> 0u) & 0x3FFu)},	  //
	  iOutHigh{static_cast<uint16_t>((reg >> 10u) & 0x3FFu)}, //
	  iInLow{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)},	  //
	  iInHigh{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)} {}

SM72445::Reg5::operator Register() const {
	Register reg = static_cast<Register>(this->iOutLow) << 0u	//
				 | static_cast<Register>(this->iOutHigh) << 10u //
				 | static_cast<Register>(this->iInLow) << 20u	//
				 | static_cast<Register>(this->iInHigh) << 30u;
	return reg;
}

uint16_t SM72445::Reg5::operator[](CurrentThreshold threshold) const {
	switch (threshold) {
	case CurrentThreshold::CURRENT_IN_LOW:
		return this->iInLow;
	case CurrentThreshold::CURRENT_IN_HIGH:
		return this->iInHigh;
	case CurrentThreshold::CURRENT_OUT_LOW:
		return this->iOutLow;
	case CurrentThreshold::CURRENT_OUT_HIGH:
		return this->iOutHigh;
	default:
		return 0u;
	}
}
