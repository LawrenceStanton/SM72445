/**
 ******************************************************************************
 * @file			: SM72445_Reg.cpp
 * @brief			: Source for RegX-related structures in SM72445.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using Reg0 = SM72445::Reg0;
using Reg1 = SM72445::Reg1;
using Reg3 = SM72445::Reg3;
using Reg4 = SM72445::Reg4;
using Reg5 = SM72445::Reg5;

Reg0::Reg0(Register reg)
	: ADC0(static_cast<uint16_t>((reg >> 00u) & 0x3FFu)), //
	  ADC2(static_cast<uint16_t>((reg >> 10u) & 0x3FFu)), //
	  ADC4(static_cast<uint16_t>((reg >> 20u) & 0x3FFu)), //
	  ADC6(static_cast<uint16_t>((reg >> 30u) & 0x3FFu)) {}

Reg0::Reg0(uint16_t ADC0, uint16_t ADC2, uint16_t ADC4, uint16_t ADC6)
	: ADC6{ADC6}, //
	  ADC4{ADC4}, //
	  ADC2{ADC2}, //
	  ADC0{ADC0} {};

Reg0::operator Register() const {
	auto reg = static_cast<Register>(this->ADC0) << 0u	//
			 | static_cast<Register>(this->ADC2) << 10u //
			 | static_cast<Register>(this->ADC4) << 20u //
			 | static_cast<Register>(this->ADC6) << 30u;
	return reg;
}

uint16_t Reg0::operator[](AnalogueChannel channel) const {
	switch (channel) {
	case AnalogueChannel::CH0:
		return this->ADC0;
	case AnalogueChannel::CH2:
		return this->ADC2;
	case AnalogueChannel::CH4:
		return this->ADC4;
	case AnalogueChannel::CH6:
		return this->ADC6;
	default:
		return 0;
	}
}

Reg1::Reg1(Register reg)
	: iIn{static_cast<uint16_t>((reg >> 00u) & 0x3FFu)},  //
	  vIn{static_cast<uint16_t>((reg >> 10u) & 0x3FFu)},  //
	  iOut{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)}, //
	  vOut{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)} {}

Reg1::Reg1(uint16_t iIn, uint16_t vIn, uint16_t iOut, uint16_t vOut)
	: vOut{vOut}, //
	  iOut{iOut}, //
	  vIn{vIn},	  //
	  iIn{iIn} {};

Reg1::operator Register() const {
	auto reg = static_cast<Register>(this->iIn) << 0u	//
			 | static_cast<Register>(this->vIn) << 10u	//
			 | static_cast<Register>(this->iOut) << 20u //
			 | static_cast<Register>(this->vOut) << 30u;
	return reg;
}

uint16_t Reg1::operator[](ElectricalProperty property) const {
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		return this->iIn;
	case ElectricalProperty::VOLTAGE_IN:
		return this->vIn;
	case ElectricalProperty::CURRENT_OUT:
		return this->iOut;
	case ElectricalProperty::VOLTAGE_OUT:
		return this->vOut;
	default:
		return 0u;
	}
}

Reg3::Reg3()
	: overrideAdcProgramming{false}, //
	  a2Override{0x0u},				 //
	  iOutMax{1023u},				 //
	  vOutMax{1023u},				 //
	  tdOff{0x3u},					 //
	  tdOn{0x3u},					 //
	  dcOpen{0x0FFu},				 //
	  passThroughSelect{false},		 //
	  passThroughManual{false},		 //
	  bbReset{false},				 //
	  clkOeManual{false},			 //
	  openLoopOperation{false} {}

Reg3::Reg3(Register reg)
	: overrideAdcProgramming{!!((reg >> 46u) & 0x1u)},		 //
	  a2Override{static_cast<uint8_t>((reg >> 40u) & 0x7u)}, //
	  iOutMax{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)}, //
	  vOutMax{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)}, //
	  tdOff{static_cast<uint8_t>((reg >> 17u) & 0x7u)},		 //
	  tdOn{static_cast<uint8_t>((reg >> 14u) & 0x7u)},		 //
	  dcOpen{static_cast<uint16_t>((reg >> 5u) & 0x1FFu)},	 //
	  passThroughSelect{!!((reg >> 4u) & 0x1u)},			 //
	  passThroughManual{!!((reg >> 3u) & 0x1u)},			 //
	  bbReset{!!((reg >> 2u) & 0x1u)},						 //
	  clkOeManual{!!((reg >> 1u) & 0x1u)},					 //
	  openLoopOperation{!!(reg & 0x1u)} {}

Reg3::operator Register() const {
	const Register reg =
		(static_cast<Register>(this->overrideAdcProgramming & 0x1u) << 46u)
		| (static_cast<Register>(this->a2Override) << 40u)				//
		| (static_cast<Register>(this->iOutMax) << 30u)					//
		| (static_cast<Register>(this->vOutMax) << 20u)					//
		| (static_cast<Register>(this->tdOff) << 17u)					//
		| (static_cast<Register>(this->tdOn) << 14u)					//
		| (static_cast<Register>(this->dcOpen) << 5u)					//
		| (static_cast<Register>(this->passThroughSelect & 0x1u) << 4u) //
		| (static_cast<Register>(this->passThroughManual & 0x1u) << 3u) //
		| (static_cast<Register>(this->bbReset & 0x1u) << 2u)			//
		| (static_cast<Register>(this->clkOeManual & 0x1u) << 1u)		//
		| (static_cast<Register>(this->openLoopOperation & 0x1u));
	return reg;
};

Reg4::Reg4(Register reg)
	: iInOffset{static_cast<uint8_t>((reg >> 0u) & 0xFFu)},	  //
	  vInOffset{static_cast<uint8_t>((reg >> 8u) & 0xFFu)},	  //
	  iOutOffset{static_cast<uint8_t>((reg >> 16u) & 0xFFu)}, //
	  vOutOffset{static_cast<uint8_t>((reg >> 24u) & 0xFFu)} {}

Reg4::Reg4(uint8_t iInOffset, uint8_t vInOffset, uint8_t iOutOffset, uint8_t vOutOffset)
	: vOutOffset{vOutOffset}, //
	  iOutOffset{iOutOffset}, //
	  vInOffset{vInOffset},	  //
	  iInOffset{iInOffset} {}

Reg4::operator Register() const {
	return static_cast<Register>(static_cast<Register>(this->iInOffset) << 0u)	 //
		 | static_cast<Register>(static_cast<Register>(this->vInOffset) << 8u)	 //
		 | static_cast<Register>(static_cast<Register>(this->iOutOffset) << 16u) //
		 | static_cast<Register>(static_cast<Register>(this->vOutOffset) << 24u);
}

uint8_t Reg4::operator[](ElectricalProperty property) const {
	switch (property) {
	case ElectricalProperty::CURRENT_IN:
		return this->iInOffset;
	case ElectricalProperty::VOLTAGE_IN:
		return this->vInOffset;
	case ElectricalProperty::CURRENT_OUT:
		return this->iOutOffset;
	case ElectricalProperty::VOLTAGE_OUT:
		return this->vOutOffset;
	default:
		return 0u;
	}
}

Reg5::Reg5() : iInHigh{40}, iInLow{24}, iOutHigh{40}, iOutLow{24} {}
Reg5::Reg5(
	uint16_t iOutLow,
	uint16_t iOutHigh,
	uint16_t iInLow,
	uint16_t iInHigh
)
	: iInHigh(iInHigh),	  //
	  iInLow(iInLow),	  //
	  iOutHigh(iOutHigh), //
	  iOutLow(iOutLow) {}

Reg5::Reg5(Register reg)
	: iOutLow{static_cast<uint16_t>((reg >> 0u) & 0x3FFu)},	  //
	  iOutHigh{static_cast<uint16_t>((reg >> 10u) & 0x3FFu)}, //
	  iInLow{static_cast<uint16_t>((reg >> 20u) & 0x3FFu)},	  //
	  iInHigh{static_cast<uint16_t>((reg >> 30u) & 0x3FFu)} {}

Reg5::operator Register() const {
	Register reg = static_cast<Register>(this->iOutLow) << 0u	//
				 | static_cast<Register>(this->iOutHigh) << 10u //
				 | static_cast<Register>(this->iInLow) << 20u	//
				 | static_cast<Register>(this->iInHigh) << 30u;
	return reg;
}

uint16_t Reg5::operator[](CurrentThreshold threshold) const {
	switch (threshold) {
	case CurrentThreshold::CURRENT_IN_LOW:
		return this->iInLow;
	case CurrentThreshold::CURRENT_IN_HIGH:
		return this->iInHigh;
	case CurrentThreshold::CURRENT_OUT_LOW:
		return this->iOutLow;
	case CurrentThreshold::CURRENT_OUT_HIGH:
		return this->iOutHigh;
	default:
		return 0u;
	}
}
