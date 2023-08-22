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
class SM72445_X : SM72445 {
public:
	using SM72445::SM72445;

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
};
