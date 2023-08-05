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
	 * @note
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
	I2C *const	  i2c;
	DeviceAddress deviceAddress;

	const float vDDA;

	const float vInGain;
	const float vOutGain;
	const float iInGain;
	const float iOutGain;

	struct Reg0 {
		uint16_t ADC0 : 10;
		uint16_t ADC2 : 10;
		uint16_t ADC4 : 10;
		uint16_t ADC6 : 10;

		Reg0(Register reg);
		Reg0(uint16_t ADC0, uint16_t ADC2, uint16_t ADC4, uint16_t ADC6)
			: ADC0{ADC0}, //
			  ADC2{ADC2}, //
			  ADC4{ADC4}, //
			  ADC6{ADC6} {};

		operator Register() const;
		uint16_t operator[](AnalogueChannel channel) const;
	};

	struct Reg1 {
		const uint16_t vOut : 10;
		const uint16_t iOut : 10;
		const uint16_t vIn	: 10;
		const uint16_t iIn	: 10;

		Reg1(Register reg);
		Reg1(uint16_t iIn, uint16_t vIn, uint16_t iOut, uint16_t vOut)
			: iIn{iIn},	  //
			  vIn{vIn},	  //
			  iOut{iOut}, //
			  vOut{vOut} {};

		operator Register() const;
		uint16_t operator[](ElectricalProperty property) const;
	};

	struct Reg3 {
		enum class Override : bool {
			OFF = false,
			ON	= true,
		};

		enum class PassThrough : bool {
			OFF = false,
			ON	= true,
		};

		Override overrideAdcProgramming : 1; // {1'b0}
	private:
		Register rsvd3 : 2; // {2'd1} Reserved bits that must be set to default.
	public:
		Register a2Override : 3;  // {3'd0} Override enable for ADC2.
		Register iOutMax	: 10; // {10'd1023} Override maximum output current.
		Register vOutMax	: 10; // {10'1023} Override maximum output voltage.
		Register tdOff		: 3;  // {3'h3} Dead time off.
		Register tdOn		: 3;  // {3'h3} Dead time on.
	private:
		Register dcOpen : 9; // {9'FF} Open loop duty cycle. TESTING ONLY.
	public:
		PassThrough passThroughSelect : 1; // {1'b0} Override enable I2C control of Panel Mode
		PassThrough passThroughManual : 1; // {1'b0} Panel Mode Override Control
		bool		bbReset			  : 1; // {1'b0} Soft Reset
		bool		clkOeManual		  : 1; // {1'b0} Enable PPL Clock on SM72445 Pin 5
		Override	openLoopOperation : 1; // {1'b0} Enable Open Loop Operation. Note complex enable sequence required.

		/**
		 * @brief Construct a default representation of the SM72445 Register 3.
		 * @note The default values are the reset values for the SM72445.
		 */
		Reg3() : rsvd3{1u}, iOutMax{1023u}, vOutMax{1023u}, tdOff{0x3u}, tdOn{0x3u}, dcOpen{0x0FFu} {}
		Reg3(Register reg);

		operator Register() const;

#ifdef SM72445_GTEST_TESTING
		FRIEND_TEST(SM72445_Reg3, constructsWithRegisterValue);
#endif
	};

	struct Reg4 {
		uint8_t iInOffset;
		uint8_t vInOffset;
		uint8_t iOutOffset;
		uint8_t vOutOffset;

		Reg4(Register reg);
		Reg4(uint8_t iInOffset, uint8_t vInOffset, uint8_t iOutOffset, uint8_t vOutOffset)
			: iInOffset{iInOffset},	  //
			  vInOffset{vInOffset},	  //
			  iOutOffset{iOutOffset}, //
			  vOutOffset{vOutOffset} {};

		operator Register() const;
		uint8_t operator[](ElectricalProperty property) const;
	};

	struct Reg5 {
		uint16_t iOutLow  : 10;
		uint16_t iOutHigh : 10;
		uint16_t iInLow	  : 10;
		uint16_t iInHigh  : 10;

		Reg5(Register reg);
		Reg5(uint16_t iOutLow, uint16_t iOutHigh, uint16_t iInLow, uint16_t iInHigh)
			: iOutLow(iOutLow),	  //
			  iOutHigh(iOutHigh), //
			  iInLow(iInLow),	  //
			  iInHigh(iInHigh) {}

		operator Register() const;
		uint16_t operator[](CurrentThreshold threshold) const;
	};

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
	 * @brief Get all electrical measurements from the SM72445.
	 *
	 * @return optional<float> The measurements, indexed by ElectricalProperty, if successful.
	 * @note Voltage measurements are returned in Volts.
	 * @note Current measurements are returned in Amps.
	 */
	optional<array<float, 4>> getElectricalMeasurements(void) const;

	/**
	 * @brief Get the Analogue Configuration Channel Pin Voltages.
	 *
	 * @return optional<float> The pin voltages, indexed by AnalogueChannel, if successful.
	 */
	optional<array<float, 4>> getAnalogueChannelVoltages(void) const;

	/**
	 * @brief Get the ADC measurement offsets for all electrical properties.
	 *
	 * @return optional<array<float, 4>> The offsets indexed by ElectricalProperty, if successful.
	 * @note Voltage measurements are returned in Volts.
	 * @note Current measurements are returned in Amps.
	 * @ref SM72445 Datasheet, Page 12, reg4.
	 */
	optional<array<float, 4>> getOffsets(void) const;

	/**
	 * @brief Get all set thresholds for MPPT power conversion.
	 *
	 * @return optional<array<float, 4>> The thresholds, indexed by CurrentThreshold, if successful.
	 */
	optional<array<float, 4>> getCurrentThresholds(void) const;

protected:
	/**
	 * @brief Get the Analogue Channel Adc Results from the SM72445.
	 *
	 * @return optional<Reg0> Structural representation of the register values, if successful.
	 */
	optional<Reg0> getAnalogueChannelAdcResults(void) const;

	/**
	 * @brief Get the Electrical Measurements ADC Results from the SM72445.
	 *
	 * @return optional<Reg1> Structural representation of the register values, if successful.
	 */
	optional<Reg1> getElectricalMeasurementsAdcResults(void) const;

	/**
	 * @brief Get the Offset Register Values from the SM72445.
	 *
	 * @return optional<Reg4> Structural representation of the register values, if successful.
	 */
	optional<Reg4> getOffsetRegisterValues(void) const;

	/**
	 * @brief Get the register binary values for the current MPPT thresholds.
	 *
	 * @return optional<array<uint16_t, 4>> The register values, indexed by CurrentThreshold, if successful.
	 */
	optional<Reg5> getThresholdRegisterValues(void) const;

	/**
	 * @brief Convert an SM72445 binary ADC result to the pin voltage, given the assumed supply voltage reference vDDA.
	 *
	 * @param adcResult The ADC Result to convert.
	 * @param resolution The resolution (in bits) of the ADC measurement.
	 * @return float The apparent pin voltage.
	 */
	float convertAdcResultToPinVoltage(uint16_t adcResult, uint8_t resolution) const;

	float getGain(SM72445::ElectricalProperty property) const;
	float getGain(SM72445::CurrentThreshold threshold) const;

private:
#ifdef SM72445_GTEST_TESTING
	friend class SM72445_Test;

	FRIEND_TEST(SM72445_Test, constructorAssignsArguments);

	FRIEND_TEST(SM72445_GainTest, getGainNormallyReturnsCorrespondingGainValue);
	FRIEND_TEST(SM72445_GainTest, getGainReturnsZeroIfGivenPropertyInvalid);

	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultsNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getAnalogueChannelAdcResultsReturnsNulloptIfI2CReadFails);

	FRIEND_TEST(SM72445_Test, getElectricalMeasurementAdcResultsNormallyReturnsValue);
	FRIEND_TEST(SM72445_Test, getElectricalMeasurementAdcResultsReturnsNulloptIfI2CReadFails);

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

/**
 * @brief Extended interface for the SM72445 including convenient (albeit inefficient) methods for single operations.
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
