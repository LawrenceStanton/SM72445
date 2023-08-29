/**
 ******************************************************************************
 * @file			: SM72445_ConfigBuilder.hpp
 * @brief			: Configuration builder object for SM72445.
 * @note 			: This file is included as part of SM72445_X.hpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

struct SM72445_X::ConfigBuilder {
	using FrequencyMode = Config::FrequencyMode;
	using PanelMode		= Config::PanelMode;
	using DeadTime		= Config::DeadTime;

private:
	const SM72445_X &sm72445;
	Reg3			 reg3;

public:
	/**
	 * @brief Reset the ADC Programming Override Enable bit.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &resetAdcProgrammingOverrideEnable(void);

	/**
	 * @brief Set the ADC Programming Frequency Override.
	 *
	 * @param frequencyMode The frequency mode to set.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &setFrequencyModeOverride(FrequencyMode frequencyMode);

	/**
	 * @brief Set the ADC programming Panel Mode Override.
	 *
	 * @param panelMode
	 * @return This ConfigBuilder.
	 * @note Calling this method also sets the ADC Programming Override Enable bit.
	 */
	ConfigBuilder &setPanelModeOverride(PanelMode panelMode);

	/**
	 * @brief Set the Max Output Current Override.
	 *
	 * @param current The current to set, in Amps.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &setMaxOutputCurrentOverride(float current);

	/**
	 * @brief Set the Max Output Voltage Override
	 *
	 * @param voltage The voltage to set, in Volts.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &setMaxOutputVoltageOverride(float voltage);

	/**
	 * @brief Set the Dead Time Off Time.
	 *
	 * @param deadTime The dead time to set.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &setDeadTimeOffTimeOverride(DeadTime deadTime);

	/**
	 * @brief Set the Dead Time On Time Override
	 *
	 * @param deadTime The dead time to set.
	 * @return
	 */
	ConfigBuilder &setDeadTimeOnTimeOverride(DeadTime deadTime);

	/**
	 * @brief Reset the Panel Mode Override Enable bit.
	 * @return This ConfigBuilder.
	 */
	ConfigBuilder &resetPanelModeRegisterOverrideEnable(void);

	/**
	 * @brief Set the Panel Mode Register Override
	 *
	 * @return This ConfigBuilder.
	 * @note Calling this method also sets the Panel Mode Override Enable bit.
	 */
	ConfigBuilder &setPanelModeRegisterOverride(bool override);

	// Config &setBbReset(bool reset); // Unsupported
	// void setClockOutputManualEnable(bool enable); // Unsupported
	// void setOpenLoopOperation(bool enable); // Unsupported

	/**
	 * @brief "Build" the Configuration Register.
	 *
	 * @return The binary value to write to the SM72445's configuration register.
	 */
	ConfigRegister build(void) const;

private:
	friend class SM72445_X;
	explicit ConfigBuilder(const SM72445_X &sm72445, SM72445::Reg3 reg3 = Reg3());
};
