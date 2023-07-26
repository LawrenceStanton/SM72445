/**
 ******************************************************************************
 * @file			: SM72445.hpp
 * @brief			: SM72445 Photovoltaic MPPT Controller Driver
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include <cstdint>
#include <optional>

using std::optional;

class SM72445 {
public:
	/**
	 * @brief I2C interface for the SM72445.
	 *
	 * @details
	 * This interface is used to abstract the I2C communication from the SM72445 class.
	 * A concrete implementation is then aggregated by the SM72445 class.
	 * The concrete implementation of this interface should be provided by the user.
	 * This also allows for the SM72445 class to be tested without the need for a real I2C bus.
	 * The concrete implementation's lifetime must be managed by the user and must outlive the SM72445 class.
	 * It is recommended that the concrete implementation be a static object.
	 */
	class I2C {
	public:
		typedef uint64_t Register;

		/**
		 * @brief Device Address of the SM72445 on the I2C Bus.
		 * @ref SM72445 Datasheet, Page 14.
		 * @note These address values should be left shifted by 1 the LSB R/W bit should be added in the I2C call.
		 */
		enum class DeviceAddress : uint8_t {
			// ! Note: ADDR000 not supported.
			ADDR001 = 0x1u,
			ADDR010 = 0x2u,
			ADDR011 = 0x3u,
			ADDR100 = 0x4u,
			ADDR101 = 0x5u,
			ADDR110 = 0x6u,
			ADDR111 = 0x7u,
		};

		/**
		 * @brief Memory Address of the SM72445 registers.
		 * @note Registers begin at 0xE0 and are offset by the register number.
		 * 		 e.g. reg1 = 0xE1, reg3 = 0xE3, etc.
		 * 		 This is not clearly stated in the datasheet.
		 */
		enum class MemoryAddress : uint8_t {
			REG0 = 0xE0u, // Analogue Channel Configuration. Read only.
			REG1 = 0xE1u, // Voltage and Current Input/Output Measurements, MPPT Status. Read only.
			REG3 = 0xE3u, // I2C Override Configuration. Read/Write.
			REG4 = 0xE4u, // Voltage and Current Input/Output Offsets. Read/Write.
			REG5 = 0xE5u, // Current Input/Output High/Low Thresholds. Read/Write.
		};

		/**
		 * @brief Read a I2C register from the SM72445.
		 *
		 * @param deviceAddress Device Address of the SM72445 on the I2C Bus.
		 * @param memoryAddress Memory Address of the register to read.
		 * @return optional<Register> The value of the register if the read was successful.
		 * @note The SM72445 will return the length of the data in the LSB of the first byte. This must be discarded.
		 * 		  The return value should be an exact copy as described in the SM72445 datasheet pages 12-13.
		 * @note Data is transmitted LSB first.
		 */
		virtual optional<Register> read(DeviceAddress deviceAddress, MemoryAddress memoryAddress) = 0;

		/**
		 * @brief Write a I2C register to the SM72445.
		 *
		 * @param deviceAddress Device Address of the SM72445 on the I2C Bus.
		 * @param memoryAddress Memory Address of the register to write.
		 * @param data The data to write to the register.
		 * @return optional<Register> The value written to the register if the write was successful.
		 * @note The SM72445 expects the length of the data (always 7) first byte transmitted, then the data proceeds.
		 * 		 This length byte must be inserted by this method's implementation.
		 * @note Data is transmitted LSB first.
		 */
		virtual optional<Register> write(DeviceAddress deviceAddress, MemoryAddress memoryAddress, Register data) = 0;
	};

	using DeviceAddress = I2C::DeviceAddress;
	using MemoryAddress = I2C::MemoryAddress;
	using Register		= I2C::Register;

	/**
	 * @brief Enumerable for the Analogue Configuration Channels.
	 */
	enum class AnalogueChannel : uint8_t {
		CH0 = 0x0u,
		CH2 = 0x2u,
		CH4 = 0x4u,
		CH6 = 0x6u,
	};

	/**
	 * @brief Generic enumerable for core electrical properties.
	 */
	enum class ElectricalProperty : uint8_t {
		CURRENT_IN	= 0u,
		VOLTAGE_IN	= 1u,
		CURRENT_OUT = 2u,
		VOLTAGE_OUT = 3u,
	};

protected:
	I2C *const	  i2c;
	DeviceAddress deviceAddress;

	const float vDDA;

	const float vInGain;
	const float vOutGain;
	const float iInGain;
	const float iOutGain;

public:
	SM72445(
		I2C			 *i2c,
		DeviceAddress deviceAddress,
		float		  vInGain,	  // Input Voltage Gain = vInAdc : vInReal
		float		  vOutGain,	  // Output Voltage Gain = vOutAdc : vOutReal
		float		  iInGain,	  // Input Current Gain = iInAdc : iInReal
		float		  iOutGain,	  // Output Current Gain = iOutAdc : iOutReal
		float		  vDDA = 5.0f // Analog Supply Voltage
	);

	SM72445(const SM72445 &) = delete;

	/**
	 * @brief Get the Input Current measured by the SM72445.
	 *
	 * @return optional<float> The input current, if successful.
	 */
	optional<float> getInputCurrent(void) const;

	/**
	 * @brief Get the Input Voltage measured by the SM72445.
	 *
	 * @return optional<float> The input voltage, if successful.
	 */
	optional<float> getInputVoltage(void) const;

	/**
	 * @brief Get the Output Current measured by the SM72445.
	 *
	 * @return optional<float> The output current, if successful.
	 */
	optional<float> getOutputCurrent(void) const;

	/**
	 * @brief Get the Output Voltage measured by the SM72445.
	 *
	 * @return optional<float> The output voltage, if successful.
	 */
	optional<float> getOutputVoltage(void) const;

	/**
	 * @brief Get an Analogue Configuration Channel Pin Voltage.
	 *
	 * @param channel The channel to read. @ref SM72445 Datasheet, Page 12.
	 * @return optional<float> The pin voltage, if successful.
	 */
	optional<float> getAnalogueChannelVoltage(AnalogueChannel channel) const;

private:
	optional<float> getElectricalMeasurement(ElectricalProperty property) const;

	/**
	 * @brief Get an Analogue Configuration Channel Pin Voltage.
	 *
	 * @param channel The channel to read. @ref SM72445 Datasheet, Page 12.
	 * @return optional<float> The pin voltage, if successful.
	 */
	optional<uint16_t> getAnalogueChannelAdcResult(AnalogueChannel channel) const;

	/**
	 * @brief Convert an SM72445 binary ADC result to the pin voltage, given the assumed supply voltage reference vDDA.
	 *
	 * @param adcResult The ADC Result to convert.
	 * @param resolution The resolution (in bits) of the ADC measurement.
	 * @return float The apparent pin voltage.
	 */
	float convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const;

#ifdef SM72445_GTEST_TESTING
	friend class SM72445_Test;

	FRIEND_TEST(SM72445_Test, constructorAssignsArguments);

	FRIEND_TEST(SM72445_Test, getElectricalMeasurementReturnsNulloptIfI2CReadFails);
	FRIEND_TEST(SM72445_Test, getElectricalMeasurementReturnsNulloptIfPropertyIsInvalid);

	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultReturnsNulloptIfI2CReadFails);

	FRIEND_TEST(SM72445_Test, convertAdcResultToPinVoltageNormallyConvertsValue);
#endif
};
