/**
 ******************************************************************************
 * @file			: SM72445_Config.hpp
 * @brief			: Configuration object for the SM72445.
 * @note 			: This file is included as part of SM72445.hpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

class SM72445_X::Config {
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

	DeadTime tdOff;
	DeadTime tdOn;

	bool panelModeOverrideEnable; // Panel Mode Override Enable
	bool panelModeOverride;		  // Panel Mode Override

	bool clockOutputManualEnable; // Pin 5 Clock Output Enable

	bool openLoopOperation;

private:
	friend class SM72445_X;
	explicit Config(const SM72445_X &sm72445, const Reg3 &reg3);
};

// TODO: Restore constexpr specifiers.
