/**
 ******************************************************************************
 * @file			: SM72445_Config.hpp
 * @brief			: Configuration objects for the SM72445.
 * @note 			: This file is included as part of SM72445.hpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

class SM72445::Config {
private:
	const SM72445 &sm72445;

public:
	enum class FrequencyMode : uint8_t {
		LOW	 = 110, // kHz
		MED	 = 135, // kHz
		HIGH = 215, // kHz
	};

	enum class PanelMode : uint8_t {
		USE_SWITCH,
		USE_H_BRIDGE,
	};

	enum class DeadTime : uint8_t {
		ZERO  = 0u,
		ONE	  = 1u,
		TWO	  = 2u,
		THREE = 3u,
		FOUR  = 4u,
		FIVE  = 5u,
		SIX	  = 6u,
		MAX	  = 7u
	};

	bool overrideAdcProgramming; // Override ADC Programming Enable

	FrequencyMode frequencyMode;
	PanelMode	  panelMode;

	float iOutMax; // Override Maximum Output Current in Amps
	float vOutMax; // Override Maximum Output Voltage in Volts

	float tdOff;
	float tdOn; // Dead Time On Time in Milliseconds

	bool panelModeOverrideEnable; // Panel Mode Override Enable
	bool panelModeOverride;		  // Panel Mode Override

	bool clockOutputManualEnable; // Pin 5 Clock Output Enable

	bool openLoopOperation;

private:
	friend class SM72445;
	explicit Config(const SM72445 &sm72445, const Reg3 &reg3);
	explicit Config(const SM72445 &sm72445, const Register &reg3);
};

// TODO: Restore constexpr specifiers.
struct SM72445::ConfigBuilder {
	typedef Register ConfigRegister;
	using FrequencyMode = Config::FrequencyMode;
	using PanelMode		= Config::PanelMode;
	using DeadTime		= Config::DeadTime;

private:
	const SM72445 &sm72445;
	Reg3		   reg3;

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
	friend class SM72445;
	explicit ConfigBuilder(const SM72445 &sm72445, SM72445::Reg3 reg3 = Reg3());

#ifdef SM72445_GTEST_TESTING
	friend class SM72445_ConfigTest;
#endif
};
