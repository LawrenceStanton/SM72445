/**
 ******************************************************************************
 * @file			: SM72445_Config.cpp
 * @brief			: Source for SM72445 Configuration Structure
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using Register		= SM72445::Register;
using Config		= SM72445::Config;
using ConfigBuilder = SM72445::ConfigBuilder;

using PanelMode		= Config::PanelMode;
using FrequencyMode = Config::FrequencyMode;

ConfigBuilder::ConfigBuilder(const SM72445 &sm72445, SM72445::Reg3 reg3)
	: sm72445(sm72445), reg3(reg3) {}

ConfigBuilder &ConfigBuilder::resetAdcProgrammingOverrideEnable(void) {
	this->reg3.overrideAdcProgramming = false;
	return *this;
}

static Config::PanelMode getPanelModeFromBits(const uint8_t bits) {
	switch (bits & 0x7u) {
	case 0x3:
	case 0x4:
	case 0x5:
		return Config::PanelMode::USE_H_BRIDGE;
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x6:
	case 0x7:
	default: // Workaround for compiler warning, should never default given mask above.
		return Config::PanelMode::USE_SWITCH;
	}
}

static FrequencyMode getFrequencyModeFromBits(const uint8_t bits) {
	switch (bits & 0x7u) { // Any excess bits will be filtered out.
	case 0x1:
	case 0x4:
		return FrequencyMode::MED;
	case 0x2:
	case 0x5:
		return FrequencyMode::LOW;
	case 0x0:
	case 0x3:
	case 0x6:
	case 0x7:
	default: // Workaround for compiler warning, should never default given mask above.
		return FrequencyMode::HIGH;
	}
}

// TODO try const variables
static uint8_t setA2Override(FrequencyMode frequencyMode, PanelMode panelMode) {
	// Reference Table 1 in the SM72445 Datasheet for the A2 Override Register Values
	uint8_t a2Override = (frequencyMode == FrequencyMode::HIGH) ? 0x0
					   : (frequencyMode == FrequencyMode::MED)	? 0x1
																: 0x2;

	// Shift 3 units up in register table if using H-Bridge.
	if (panelMode == PanelMode::USE_H_BRIDGE) { a2Override += 0x3; }

	return a2Override;
}

ConfigBuilder &ConfigBuilder::setFrequencyModeOverride(FrequencyMode frequencyMode) {
	const PanelMode panelMode		  = getPanelModeFromBits(this->reg3.a2Override);
	this->reg3.a2Override			  = setA2Override(frequencyMode, panelMode);
	this->reg3.overrideAdcProgramming = true; // Specified side effect.
	return *this;
}

ConfigBuilder &ConfigBuilder::setPanelModeOverride(PanelMode panelMode) {
	FrequencyMode frequencyMode		  = getFrequencyModeFromBits(this->reg3.a2Override);
	this->reg3.a2Override			  = setA2Override(frequencyMode, panelMode);
	this->reg3.overrideAdcProgramming = true; // Specified side effect.
	return *this;
}

ConfigBuilder &ConfigBuilder::setMaxOutputCurrentOverride(float current) {
	const uint16_t maxOutputCurrentAdcThreshold =
		current * this->sm72445.iOutGain / this->sm72445.vDDA * 0x3FFull;

	if (maxOutputCurrentAdcThreshold > 0x3FFu) {
		// Invalid value, exceeds settable range. Default action set to zero.
		this->reg3.iOutMax = 0x0u;
		return *this;
	}

	this->reg3.iOutMax				  = maxOutputCurrentAdcThreshold;
	this->reg3.overrideAdcProgramming = true; // Specified side effect.
	return *this;
}

ConfigBuilder &ConfigBuilder::setMaxOutputVoltageOverride(float voltage) {
	const uint16_t maxOutputVoltageAdcThreshold =
		voltage * this->sm72445.vOutGain / this->sm72445.vDDA * 0x3FFull;

	if (maxOutputVoltageAdcThreshold > 0x3FFu) {
		// Invalid value, exceeds settable range. Default action set to zero.
		this->reg3.vOutMax = 0x0u;
		return *this;
	}

	this->reg3.vOutMax				  = maxOutputVoltageAdcThreshold;
	this->reg3.overrideAdcProgramming = true; // Specified side effect.
	return *this;
}

ConfigBuilder &ConfigBuilder::setDeadTimeOffTimeOverride(float milliseconds) {
	// TODO
	return *this;
}

ConfigBuilder &ConfigBuilder::setDeadTimeOnTimeOverride(float milliseconds) {
	// TODO
	return *this;
}

ConfigBuilder &ConfigBuilder::resetPanelModeRegisterOverrideEnable(void) {
	// TODO
	return *this;
}

ConfigBuilder &ConfigBuilder::setPanelModeRegisterOverride(bool override) {
	// TODO
	return *this;
}

ConfigBuilder::ConfigRegister ConfigBuilder::build(void) const {
	return ConfigRegister(Register(this->reg3));
}
