/**
 ******************************************************************************
 * @file			: SM72445_X.hpp
 * @brief			: SM72445 Extended Functionality Class
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include "SM72445.hpp"

/**
 * @brief Extended interface for the SM72445 including convenient (albeit inefficient)
 * methods for single operations.
 *
 */
class SM72445_X : public SM72445 {
private:
	const float vDDA;

	const float vInGain;
	const float vOutGain;
	const float iInGain;
	const float iOutGain;

public:
	struct Config;
	class ConfigBuilder;

public:
	SM72445_X(
		I2C			 &i2c,
		DeviceAddress deviceAddress,
		float		  vInGain,	  // Input Voltage Gain = vInAdc : vInReal
		float		  vOutGain,	  // Output Voltage Gain = vOutAdc : vOutReal
		float		  iInGain,	  // Input Current Gain = iInAdc : iInReal
		float		  iOutGain,	  // Output Current Gain = iOutAdc : iOutReal
		float		  vDDA = 5.0f // Analog Supply Voltage
	);

	/**
	 * @brief Get the Configuration of the SM72445.
	 *
	 * @return The configuration, if successful.
	 */
	optional<Config> getConfig(void) const;

	/**
	 * @brief Set the Configuration of the SM72445.
	 *
	 * @param configRegister The configuration to set. See Sm72445::Config for builder.
	 * @return optional<Register> The value written to Reg5, if the write was successful.
	 */
	optional<Register> setConfig(ConfigRegister configRegister) const;

	/**
	 * @brief Get the Input Current measured by the SM72445.
	 *
	 * @return optional<float> The input current, if successful.
	 * @ref SM72445 Datasheet, Page 12, reg1.
	 */
	optional<float> getInputCurrent(void) const;

	/**
	 * @brief Get the Input Voltage measured by the SM72445.
	 *
	 * @return optional<float> The input voltage, if successful.
	 * @ref SM72445 Datasheet, Page 12, reg1.
	 */
	optional<float> getInputVoltage(void) const;

	/**
	 * @brief Get the Output Current measured by the SM72445.
	 *
	 * @return optional<float> The output current, if successful.
	 * @ref SM72445 Datasheet, Page 12, reg1.
	 */
	optional<float> getOutputCurrent(void) const;

	/**
	 * @brief Get the Output Voltage measured by the SM72445.
	 *
	 * @return optional<float> The output voltage, if successful.
	 * @ref SM72445 Datasheet, Page 12, reg1.
	 */
	optional<float> getOutputVoltage(void) const;

	/**
	 * @brief Get an Analogue Configuration Channel Pin Voltage.
	 *
	 * @param channel The channel to read. @ref SM72445 Datasheet, Page 12.
	 * @return optional<float> The pin voltage, if successful.
	 * @ref SM72445 Datasheet, Page 12, reg0.
	 */
	optional<float> getAnalogueChannelVoltage(AnalogueChannel channel) const;

	/**
	 * @brief Get the ADC measurement offset for a given electrical property.
	 *
	 * @param property The electrical property to get the offset for.
	 * @return optional<float> The offset, if successful.
	 * @note Voltage measurements are returned in Volts.
	 * @note Current measurements are returned in Amps.
	 * @ref SM72445 Datasheet, Page 12, reg4.
	 */
	optional<float> getOffset(ElectricalProperty property) const;

	/**
	 * @brief Get the set thresholds for MPPT power conversion.
	 *
	 * @param threshold The threshold to get the value for.
	 * @return optional<float> The threshold value, in Amps, if successful.
	 */
	optional<float> getCurrentThreshold(CurrentThreshold threshold) const;

	/**
	 * @brief Get all electrical measurements from the SM72445.
	 *
	 * @return The measurements, indexed by ElectricalProperty, if successful.
	 * @note Voltage measurements are returned in Volts.
	 * @note Current measurements are returned in Amps.
	 */
	optional<array<float, 4>> getElectricalMeasurements(void) const;

	/**
	 * @brief Get the Analogue Configuration Channel Pin Voltages.
	 *
	 * @return The pin voltages, indexed by AnalogueChannel, if successful.
	 */
	optional<array<float, 4>> getAnalogueChannelVoltages(void) const;

	/**
	 * @brief Get the ADC measurement offsets for all electrical properties.
	 *
	 * @return optional<array<float, 4>> The offsets indexed by ElectricalProperty, if
	 * successful.
	 * @note Voltage measurements are returned in Volts.
	 * @note Current measurements are returned in Amps.
	 * @ref SM72445 Datasheet, Page 12, reg4.
	 */
	optional<array<float, 4>> getOffsets(void) const;

	/**
	 * @brief Get all set thresholds for MPPT power conversion.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<array<float, 4>> getCurrentThresholds(void) const;

	/**
	 * @brief Get a Configuration Builder Object.
	 *
	 * @param fetchCurrentConfig If true, the current configuration will be fetched from
	 * the SM72445 and used to initialise the state of the ConfigBuilder.
	 * @return A ConfigBuilder object.
	 */
	ConfigBuilder getConfigBuilder(bool fetchCurrentConfig = false) const;

	/**
	 * @brief Convert an SM72445 binary ADC result to the pin voltage, given the assumed
	 * supply voltage reference vDDA.
	 *
	 * @param adcResult The ADC Result to convert.
	 * @param resolution The resolution (in bits) of the ADC measurement.
	 * @return float The apparent pin voltage.
	 */
	float convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const;

private:
	float			getGain(SM72445::ElectricalProperty property) const;
	float			getGain(SM72445::CurrentThreshold threshold) const;
	constexpr float getGain(SM72445::AnalogueChannel threshold) const {
		(void)threshold; // TODO : Improve this workaround
		return 1.0f;
	};

private:
#ifdef SM72445_GTEST_TESTING
	friend class SM72445_Test;

	FRIEND_TEST(SM72445_Test, constructorAssignsArguments);

	FRIEND_TEST(SM72445_GainTest, getGainNormallyReturnsCorrespondingGainValue);
	FRIEND_TEST(SM72445_GainTest, getGainReturnsZeroIfGivenPropertyInvalid);
#endif
};

#include "Private/SM72445_Config.hpp"
#include "Private/SM72445_ConfigBuilder.hpp"
