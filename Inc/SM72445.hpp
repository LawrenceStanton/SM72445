/**
 ******************************************************************************
 * @file			: SM72445.hpp
 * @brief			: SM72445 Photovoltaic MPPT Controller Driver
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include <array>
#include <cstdint>
#include <optional>

using std::array;
using std::optional;

class SM72445 {
public:
	/**
	 * @brief I2C interface for the SM72445.
	 *
	 * @details
	 * This interface is used to abstract the I2C communication from the SM72445 class.
	 * A concrete implementation is then aggregated by the SM72445 class.
	 *
	 * The concrete implementation of this interface should be provided by the user.
	 * This also allows for the SM72445 class to be tested without the need for a real I2C
	 * bus.
	 *
	 * @note
	 * The concrete implementation's lifetime must be managed by the user and must outlive
	 * the SM72445 class.
	 *
	 * It is recommended that the concrete implementation be a static object.
	 */
	class I2C {
	public:
		typedef uint64_t Register;

		/**
		 * @brief Device Address of the SM72445 on the I2C Bus.
		 * @ref SM72445 Datasheet, Page 14.
		 * @note These address values should be left shifted by 1 the LSB R/W bit should
		 * be added in the I2C call.
		 */
		enum class DeviceAddress : uint8_t;

		/**
		 * @brief Memory Address of the SM72445 registers.
		 * @note Registers begin at 0xE0 and are offset by the register number.
		 * 		 e.g. reg1 = 0xE1, reg3 = 0xE3, etc.
		 * 		 This is not clearly stated in the datasheet.
		 */
		enum class MemoryAddress : uint8_t;

		/**
		 * @brief Read a I2C register from the SM72445.
		 *
		 * @param deviceAddress Device Address of the SM72445 on the I2C Bus.
		 * @param memoryAddress Memory Address of the register to read.
		 * @return optional<Register> The value of the register if the read was
		 * successful.
		 * @note The SM72445 will return the length of the data in the LSB of the first
		 * byte. This must be discarded. The return value should be an exact copy as
		 * described in the SM72445 datasheet pages 12-13.
		 * @note Data is to be transmitted LSB first.
		 */
		virtual optional<Register> read(
			DeviceAddress deviceAddress, //
			MemoryAddress memoryAddress
		) = 0;

		/**
		 * @brief Write a I2C register to the SM72445.
		 *
		 * @param deviceAddress Device Address of the SM72445 on the I2C Bus.
		 * @param memoryAddress Memory Address of the register to write.
		 * @param data The data to write to the register.
		 * @return optional<Register> The value written to the register if the write was
		 * successful.
		 * @note The SM72445 expects the length of the data (always 7) first byte
		 * transmitted, then the data proceeds. This length byte must be inserted by this
		 * method's implementation.
		 * @note Data is to be transmitted LSB first.
		 */
		virtual optional<Register> write(
			DeviceAddress deviceAddress,
			MemoryAddress memoryAddress,
			Register	  data
		) = 0;
	};

	using DeviceAddress = I2C::DeviceAddress;
	using MemoryAddress = I2C::MemoryAddress;
	using Register		= I2C::Register;

	/**
	 * @brief Enumerable for the Analogue Configuration Channels.
	 */
	enum class AnalogueChannel : uint8_t {
		CH0 = 0x0u,
		CH2 = 0x1u,
		CH4 = 0x2u,
		CH6 = 0x3u,
	};

	/**
	 * @brief Generic enumerable for core electrical properties.
	 */
	enum class ElectricalProperty : uint8_t {
		CURRENT_IN	= 0x0u,
		VOLTAGE_IN	= 0x1u,
		CURRENT_OUT = 0x2u,
		VOLTAGE_OUT = 0x3u,
	};

	/**
	 * @brief Enumerable for MPPT Current Thresholds.
	 */
	enum class CurrentThreshold : uint8_t {
		CURRENT_OUT_LOW	 = 0x0u,
		CURRENT_OUT_HIGH = 0x1u,
		CURRENT_IN_LOW	 = 0x2u,
		CURRENT_IN_HIGH	 = 0x3u,
	};

protected:
	I2C			 &i2c;
	DeviceAddress deviceAddress;

	const float vDDA;

	const float vInGain;
	const float vOutGain;
	const float iInGain;
	const float iOutGain;

public:
	struct Reg0;
	struct Reg1;
	struct Reg3;
	struct Reg4;
	struct Reg5;

	struct Config;
	class ConfigBuilder;

public:
	SM72445(
		I2C			 &i2c,
		DeviceAddress deviceAddress,
		float		  vInGain,	  // Input Voltage Gain = vInAdc : vInReal
		float		  vOutGain,	  // Output Voltage Gain = vOutAdc : vOutReal
		float		  iInGain,	  // Input Current Gain = iInAdc : iInReal
		float		  iOutGain,	  // Output Current Gain = iOutAdc : iOutReal
		float		  vDDA = 5.0f // Analog Supply Voltage
	);

	SM72445(const SM72445 &) = delete;

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
	 * @brief Get the Configuration of the SM72445.
	 *
	 * @return The configuration, if successful.
	 */
	optional<Config> getConfig(void) const;

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
	 * @brief Get a register from the SM72445, parsed into a structural representation.
	 *
	 * @tparam Reg The type of register to get.
	 * @param memoryAddress The memory address of the respective register
	 * @return
	 */
	template <typename Reg>
	optional<Reg> getRegister(SM72445::MemoryAddress memoryAddress) const;

	/**
	 * @brief Get the Analogue Channel Adc Results from the SM72445.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<Reg0> getAnalogueChannelRegister(void) const;

	/**
	 * @brief Get the Electrical Measurements ADC Results from the SM72445.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<Reg1> getElectricalMeasurementsRegister(void) const;

	/**
	 * @brief Get the Reg3 Configuration from the SM72445.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<Reg3> getConfigRegister(void) const;

	/**
	 * @brief Get the Offset Register Values from the SM72445.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<Reg4> getOffsetRegister(void) const;

	/**
	 * @brief Get the register binary values for the current MPPT thresholds.
	 *
	 * @return Structural representation of the register values, if successful.
	 */
	optional<Reg5> getThresholdRegister(void) const;

	/**
	 * @brief Convert an SM72445 binary ADC result to the pin voltage, given the assumed
	 * supply voltage reference vDDA.
	 *
	 * @param adcResult The ADC Result to convert.
	 * @param resolution The resolution (in bits) of the ADC measurement.
	 * @return float The apparent pin voltage.
	 */
	float convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const;

public:
	float			getGain(SM72445::ElectricalProperty property) const;
	float			getGain(SM72445::CurrentThreshold threshold) const;
	constexpr float getGain(SM72445::AnalogueChannel threshold) const {
		(void)threshold; // TODO : Improve this workaround
		return 1.0f;
	};

	constexpr DeviceAddress getDeviceAddress(void) const;

private:
#ifdef SM72445_GTEST_TESTING
	friend class SM72445_Test;

	FRIEND_TEST(SM72445_Test, constructorAssignsArguments);

	FRIEND_TEST(SM72445_GainTest, getGainNormallyReturnsCorrespondingGainValue);
	FRIEND_TEST(SM72445_GainTest, getGainReturnsZeroIfGivenPropertyInvalid);

	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultsNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultsReturnsNulloptIfI2CReadFails);

	FRIEND_TEST(SM72445_Test, getElectricalMeasurementAdcResultsNormallyReturnsValue);
	FRIEND_TEST(
		SM72445_Test,
		getElectricalMeasurementAdcResultsReturnsNulloptIfI2CReadFails
	);

	FRIEND_TEST(SM72445_Test, getOffsetRegisterValuesNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getOffsetRegisterValuesReturnsNulloptIfI2CReadFails);

	FRIEND_TEST(SM72445_Test, getThresholdRegisterValuesNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getThresholdRegisterValuesReturnsNulloptIfI2CReadFails);

	FRIEND_TEST(SM72445_Test, convertAdcResultToPinVoltageNormallyConvertsValue);

	FRIEND_TEST(SM72445_Reg0, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg0, registerConstructsBinaryRepresentation);
	FRIEND_TEST(SM72445_Reg0, indexOperatorReturnsCorrespondingChannelValue);
	FRIEND_TEST(SM72445_Reg0, indexOperatorReturnsZeroIfGivenInvalidChannel);

	FRIEND_TEST(SM72445_Reg1, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg1, registerConstructsBinaryRepresentation);
	FRIEND_TEST(SM72445_Reg1, indexOperatorReturnsCorrespondingChannelValue);
	FRIEND_TEST(SM72445_Reg1, indexOperatorReturnsZeroIfGivenInvalidChannel);

	FRIEND_TEST(SM72445_Reg3, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg3, registerConstructsBinaryRepresentation);

	FRIEND_TEST(SM72445_Reg4, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg4, registerConstructsBinaryRepresentation);
	FRIEND_TEST(SM72445_Reg4, indexOperatorReturnsCorrespondingChannelValue);
	FRIEND_TEST(SM72445_Reg4, indexOperatorReturnsZeroIfGivenInvalidChannel);

	FRIEND_TEST(SM72445_Reg5, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg5, registerConstructsBinaryRepresentation);
	FRIEND_TEST(SM72445_Reg5, indexOperatorReturnsCorrespondingChannelValue);
	FRIEND_TEST(SM72445_Reg5, indexOperatorReturnsZeroIfGivenInvalidChannel);

#endif
};

enum class SM72445::I2C::DeviceAddress : uint8_t {
	// ! ADDR000 not supported.
	ADDR001 = 0x1u,
	ADDR010 = 0x2u,
	ADDR011 = 0x3u,
	ADDR100 = 0x4u,
	ADDR101 = 0x5u,
	ADDR110 = 0x6u,
	ADDR111 = 0x7u,
};

enum class SM72445::I2C::MemoryAddress : uint8_t {
	REG0 = 0xE0u, // Analogue Channel Configuration. Read only.
	REG1 = 0xE1u, // Voltage and Current Input/Output Measurements, MPPT Status. Read.
	REG3 = 0xE3u, // I2C Override Configuration. Read/Write.
	REG4 = 0xE4u, // Voltage and Current Input/Output Offsets. Read/Write.
	REG5 = 0xE5u, // Current Input/Output High/Low Thresholds. Read/Write.
};

#include "Private/SM72445_Reg.hpp"

#include "Private/SM72445_Config.hpp"
