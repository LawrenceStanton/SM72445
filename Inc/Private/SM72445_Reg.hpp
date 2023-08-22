/**
 ******************************************************************************
 * @file			: SM72445_Reg.hpp
 * @brief			: Definitions of the SM72445 Register Objects.
 * @note			: This file is included as part of SM72445.hpp.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

struct SM72445::Reg0 {
	uint16_t ADC6 : 10;
	uint16_t ADC4 : 10;
	uint16_t ADC2 : 10;
	uint16_t ADC0 : 10;

	explicit Reg0() = default;
	explicit Reg0(Register reg);
	Reg0(uint16_t ADC0, uint16_t ADC2, uint16_t ADC4, uint16_t ADC6);

	explicit operator Register() const;
	uint16_t operator[](AnalogueChannel channel) const;
};

struct SM72445::Reg1 {
	const uint16_t vOut : 10;
	const uint16_t iOut : 10;
	const uint16_t vIn	: 10;
	const uint16_t iIn	: 10;

	explicit Reg1() = default;
	explicit Reg1(Register reg);
	Reg1(uint16_t iIn, uint16_t vIn, uint16_t iOut, uint16_t vOut);

	explicit operator Register() const;
	uint16_t operator[](ElectricalProperty property) const;
};

struct SM72445::Reg3 {

public:
	bool overrideAdcProgramming : 1; // {1'b0}
private:
	// Register rsvd3 : 2; // {2'd1} Reserved bits.
public:
	uint8_t	 a2Override : 3;  // {3'd0} Override enable for ADC2.
	uint16_t iOutMax	: 10; // {10'd1023} Override maximum output current.
	uint16_t vOutMax	: 10; // {10'1023} Override maximum output voltage.
	uint8_t	 tdOff		: 3;  // {3'h3} Dead time off.
	uint8_t	 tdOn		: 3;  // {3'h3} Dead time on.
private:
	uint16_t dcOpen : 9; // {9'0FF} Open loop duty cycle. TESTING ONLY.
public:
	bool passThroughSelect : 1; // {1'b0} Override enable I2C control of Panel Mode
	bool passThroughManual : 1; // {1'b0} Panel Mode Override Control
	bool bbReset		   : 1; // {1'b0} Soft Reset
	bool clkOeManual	   : 1; // {1'b0} Enable PPL Clock on SM72445 Pin 5
	bool openLoopOperation : 1; // {1'b0} Enable Open Loop Operation. Note complex enable
								// sequence required.

	explicit Reg3();
	explicit Reg3(Register reg);

	explicit operator Register() const;

#ifdef SM72445_GTEST_TESTING
	FRIEND_TEST(SM72445_Reg3, constructsWithRegisterValue);
	FRIEND_TEST(SM72445_Reg3, registerCastConstructsBinaryRepresentation);
	FRIEND_TEST(SM72445_Reg3, defaultConstructsToResetValue);
#endif
};

struct SM72445::Reg4 {
	uint8_t vOutOffset;
	uint8_t iOutOffset;
	uint8_t vInOffset;
	uint8_t iInOffset;

	explicit Reg4() = default;
	explicit Reg4(Register reg);
	Reg4(uint8_t iInOffset, uint8_t vInOffset, uint8_t iOutOffset, uint8_t vOutOffset);

	explicit operator Register() const;
	uint8_t operator[](ElectricalProperty property) const;
};

struct SM72445::Reg5 {
	uint16_t iInHigh  : 10;
	uint16_t iInLow	  : 10;
	uint16_t iOutHigh : 10;
	uint16_t iOutLow  : 10;

	explicit Reg5();
	explicit Reg5(Register reg);
	Reg5(uint16_t iOutLow, uint16_t iOutHigh, uint16_t iInLow, uint16_t iInHigh);

	explicit operator Register() const;
	uint16_t operator[](CurrentThreshold threshold) const;
};