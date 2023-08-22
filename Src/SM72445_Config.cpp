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
	this->reg3 &= ~0b1ull << 46;
	return *this;
}

static Config::PanelMode getPanelModeFromRegister(const Register reg3) {
	const uint8_t a2Override = reg3 >> 40u & 0x7u;

	switch (a2Override) {
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

static FrequencyMode getFrequencyModeFromRegister(const Register reg3) {
	const Register a2Override = reg3 >> 40u & 0x7u;

	switch (a2Override) {
	case 0x0:
	case 0x3:
	case 0x6:
	case 0x7:
		return FrequencyMode::HIGH;
	case 0x1:
	case 0x4:
		return FrequencyMode::MED;
	case 0x2:
	case 0x5:
	default: // Workaround for compiler warning, should never default given mask above.
		return FrequencyMode::LOW;
	}
}

static Register
setA2Override(FrequencyMode frequencyMode, PanelMode panelMode, Register reg3) {
	// Reference Table 1 in the SM72445 Datasheet for the A2 Override Register Values
	Register a2Override = (frequencyMode == FrequencyMode::HIGH) ? 0x0
						: (frequencyMode == FrequencyMode::MED)	 ? 0x1
																 : 0x2;

	// Shift 3 units up in register table if using H-Bridge.
	if (panelMode == PanelMode::USE_H_BRIDGE) { a2Override += 0x3; }

	// Write new ADC Programming Override Enable bits.
	reg3 &= ~(0x7ull << 40u);
	reg3 |= (a2Override << 40u);

	// Set ADC Programming Override Enable bit. Specified side effect.
	reg3 |= (0x1ull << 46u);

	return reg3;
}

ConfigBuilder &ConfigBuilder::setFrequencyModeOverride(FrequencyMode frequencyMode) {
	const PanelMode panelMode = getPanelModeFromRegister(this->reg3);

	this->reg3 = setA2Override(frequencyMode, panelMode, this->reg3);

	return *this;
}

ConfigBuilder &ConfigBuilder::setPanelModeOverride(PanelMode panelMode) {
	FrequencyMode frequencyMode = getFrequencyModeFromRegister(this->reg3);

	this->reg3 = setA2Override(frequencyMode, panelMode, this->reg3);

	return *this;
}

ConfigBuilder &ConfigBuilder::setMaxOutputCurrentOverride(float current) {
	const uint8_t regOffset = 30u;

	const Register maxOutputCurrentAdcThreshold =
		(current * this->sm72445.iOutGain / this->sm72445.vDDA) * 0x3FFull;

	if (maxOutputCurrentAdcThreshold > 0x3FFu) {
		// Invalid value, exceeds settable range. Default action set to zero.
		this->reg3 &= ~(0x3FFull << regOffset);
		return *this;
	}

	this->reg3 &= ~(0x3FFull << regOffset) | (maxOutputCurrentAdcThreshold << regOffset);

	return *this;
}

ConfigBuilder &ConfigBuilder::setMaxOutputVoltageOverride(float voltage) {
	const uint8_t regOffset = 20u;

	const Register maxOutputVoltageAdcThreshold =
		(voltage * this->sm72445.vOutGain / this->sm72445.vDDA) * 0x3FFull;

	if (maxOutputVoltageAdcThreshold > 0x3FFu) {
		// Invalid value, exceeds settable range. Default action set to zero.
		this->reg3 &= ~(0x3FFull << regOffset);
		return *this;
	}

	this->reg3 &= ~(0x3FFull << regOffset) | (maxOutputVoltageAdcThreshold << regOffset);

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
	return ConfigRegister(this->reg3);
}
