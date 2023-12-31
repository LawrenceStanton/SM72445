/**
 ******************************************************************************
 * @file			: SM72445_Config.cpp
 * @brief			: Source for SM72445 Configuration Structure
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.hpp"

using Register		= SM72445::Register;
using Config		= SM72445_X::Config;
using ConfigBuilder = SM72445_X::ConfigBuilder;

using PanelMode		= Config::PanelMode;
using FrequencyMode = Config::FrequencyMode;
using DeadTime		= Config::DeadTime;

static PanelMode	 getPanelModeFromBits(const uint8_t bits);
static FrequencyMode getFrequencyModeFromBits(const uint8_t bits);

Config::Config(const SM72445_X &sm72445, const Reg3 &reg3)
	: sm72445(sm72445),													  //
	  overrideAdcProgramming(reg3.overrideAdcProgramming),				  //
	  frequencyMode(getFrequencyModeFromBits(reg3.a2Override)),			  //
	  panelMode(getPanelModeFromBits(reg3.a2Override)),					  //
	  iOutMax(reg3.iOutMax * sm72445.vDDA / sm72445.iOutGain / 0x3FFull), //
	  vOutMax(reg3.vOutMax * sm72445.vDDA / sm72445.vOutGain / 0x3FFull), //
	  tdOff(static_cast<DeadTime>(reg3.tdOff)),							  //
	  tdOn(static_cast<DeadTime>(reg3.tdOn)),							  //
	  panelModeOverrideEnable(reg3.passThroughSelect),					  //
	  panelModeOverride(reg3.passThroughManual),						  //
	  bbReset(reg3.bbReset),											  //
	  clockOutputManualEnable(reg3.clkOeManual),						  //
	  openLoopOperation(reg3.openLoopOperation) {}

ConfigBuilder::ConfigBuilder(const SM72445_X &sm72445, Reg3 reg3)
	: sm72445(sm72445), reg3(reg3) {}

ConfigBuilder &ConfigBuilder::resetAdcProgrammingOverrideEnable(void) {
	this->reg3.overrideAdcProgramming = false;
	return *this;
}

static PanelMode getPanelModeFromBits(const uint8_t bits) {
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

	if (current < 0.0f || maxOutputCurrentAdcThreshold > 0x3FFu) {
		// Invalid value, outside settable range. Default action set to zero.
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

	if (voltage < 0.0f || maxOutputVoltageAdcThreshold > 0x3FFu) {
		// Invalid value, outside settable range. Default action set to zero.
		this->reg3.vOutMax = 0x0u;
		return *this;
	}

	this->reg3.vOutMax				  = maxOutputVoltageAdcThreshold;
	this->reg3.overrideAdcProgramming = true; // Specified side effect.
	return *this;
}

ConfigBuilder &ConfigBuilder::setDeadTimeOffTimeOverride(DeadTime deadTime) {
	this->reg3.tdOff = static_cast<uint8_t>(deadTime);
	return *this;
}

ConfigBuilder &ConfigBuilder::setDeadTimeOnTimeOverride(DeadTime deadTime) {
	this->reg3.tdOn = static_cast<uint8_t>(deadTime);
	return *this;
}

ConfigBuilder &ConfigBuilder::resetPanelModeRegisterOverrideEnable(void) {
	this->reg3.passThroughSelect = false;
	return *this;
}

ConfigBuilder &ConfigBuilder::setPanelModeRegisterOverride(bool override) {
	this->reg3.passThroughManual = override;
	this->reg3.passThroughSelect = true; // Specified side effect.
	return *this;
}

ConfigBuilder &SM72445_X::ConfigBuilder::setBbReset(bool reset) {
	this->reg3.bbReset = reset;
	return *this;
}
SM72445::ConfigRegister ConfigBuilder::build(void) const {
	return ConfigRegister(Register(this->reg3));
}
