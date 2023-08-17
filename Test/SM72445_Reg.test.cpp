/**
 ******************************************************************************
 * @file			: SM72445_Reg.test.cpp
 * @brief			: Tests for SM72445::RegX Structures
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.test.hpp"

TEST(SM72445_Reg0, constructsWithRegisterValue) {
	SM72445::Reg0 reg0{Register(0x0ul)};
	EXPECT_EQ(reg0.ADC0, 0x0000u);
	EXPECT_EQ(reg0.ADC2, 0x0000u);
	EXPECT_EQ(reg0.ADC4, 0x0000u);
	EXPECT_EQ(reg0.ADC6, 0x0000u);

	SM72445::Reg0 reg0Max{Register(~0x0ul)};
	EXPECT_EQ(reg0Max.ADC0, 0x03FFu);
	EXPECT_EQ(reg0Max.ADC2, 0x03FFu);
	EXPECT_EQ(reg0Max.ADC4, 0x03FFu);
	EXPECT_EQ(reg0Max.ADC6, 0x03FFu);

	SM72445::Reg0 reg0Zeros(Register(0x0ul));
	EXPECT_EQ(reg0Zeros.ADC0, 0x0000u);
	EXPECT_EQ(reg0Zeros.ADC2, 0x0000u);
	EXPECT_EQ(reg0Zeros.ADC4, 0x0000u);
	EXPECT_EQ(reg0Zeros.ADC6, 0x0000u);

	SM72445::Reg0 reg0Alt1{Register(0xAA'AAAA'AAAA'AAAAul)};
	EXPECT_EQ(reg0Alt1.ADC0, 0x02AAu);
	EXPECT_EQ(reg0Alt1.ADC2, 0x02AAu);
	EXPECT_EQ(reg0Alt1.ADC4, 0x02AAu);
	EXPECT_EQ(reg0Alt1.ADC6, 0x02AAu);

	SM72445::Reg0 reg0Alt2{Register(0x55'5555'5555'5555ul)};
	EXPECT_EQ(reg0Alt2.ADC0, 0x0155u);
	EXPECT_EQ(reg0Alt2.ADC2, 0x0155u);
	EXPECT_EQ(reg0Alt2.ADC4, 0x0155u);
	EXPECT_EQ(reg0Alt2.ADC6, 0x0155u);
}

TEST(SM72445_Reg0, registerConstructsBinaryRepresentation) {
	for (uint64_t value : {
			 0x0ul,
			 0x1ul,
			 ~0x0ul,
			 0xAA'AAAA'AAAA'AAAAul,
			 0x55'5555'5555'5555ul,
			 0xFF'FFFF'FFFF'FFFFul,
			 0xA6'1239'FE42'FEDCul,
		 }) {
		SM72445::Reg0 reg0{Register(value)};
		Register	  regOnlyUsedBits = Register(value & 0x0000'00FF'FFFF'FFFFul);
		EXPECT_EQ(Register(reg0), regOnlyUsedBits);
	}
}

TEST(SM72445_Reg0, indexOperatorReturnsCorrespondingChannelValue) {
	SM72445::Reg0 reg0{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg0[AnalogueChannel::CH0], 0x0u);
	EXPECT_EQ(reg0[AnalogueChannel::CH2], 0x1u);
	EXPECT_EQ(reg0[AnalogueChannel::CH4], 0x2u);
	EXPECT_EQ(reg0[AnalogueChannel::CH6], 0x3u);
}

TEST(SM72445_Reg0, indexOperatorReturnsZeroIfGivenInvalidChannel) {
	SM72445::Reg0 reg0{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg0[static_cast<AnalogueChannel>(0xFFu)], 0x0u);
}

TEST(SM72445_Reg1, constructsWithRegisterValue) {
	SM72445::Reg1 reg1{Register(0x100300400ul)};
	EXPECT_EQ(reg1.iIn, 0x0000u);
	EXPECT_EQ(reg1.vIn, 0x0001u);
	EXPECT_EQ(reg1.iOut, 0x0003u);
	EXPECT_EQ(reg1.vOut, 0x0004u);

	SM72445::Reg1 reg1Max{Register(~0x0ul)};
	EXPECT_EQ(reg1Max.iIn, 0x03FFu);
	EXPECT_EQ(reg1Max.vIn, 0x03FFu);
	EXPECT_EQ(reg1Max.iOut, 0x03FFu);
	EXPECT_EQ(reg1Max.vOut, 0x03FFu);

	SM72445::Reg1 reg1Zeros{Register(0x0ul)};
	EXPECT_EQ(reg1Zeros.iIn, 0x0000u);
	EXPECT_EQ(reg1Zeros.vIn, 0x0000u);
	EXPECT_EQ(reg1Zeros.iOut, 0x0000u);
	EXPECT_EQ(reg1Zeros.vOut, 0x0000u);

	SM72445::Reg1 reg1Alt1{Register(0xAA'AAAA'AAAA'AAAAul)};
	EXPECT_EQ(reg1Alt1.iIn, 0x02AAu);
	EXPECT_EQ(reg1Alt1.vIn, 0x02AAu);
	EXPECT_EQ(reg1Alt1.iOut, 0x02AAu);
	EXPECT_EQ(reg1Alt1.vOut, 0x02AAu);

	SM72445::Reg1 reg1Alt2{Register(0x55'5555'5555'5555ul)};
	EXPECT_EQ(reg1Alt2.iIn, 0x0155u);
	EXPECT_EQ(reg1Alt2.vIn, 0x0155u);
	EXPECT_EQ(reg1Alt2.iOut, 0x0155u);
	EXPECT_EQ(reg1Alt2.vOut, 0x0155u);
}

TEST(SM72445_Reg1, registerConstructsBinaryRepresentation) {
	for (uint64_t value : {
			 0x0ul,
			 0x1ul,
			 ~0x0ul,
			 0xAA'AAAA'AAAA'AAAAul,
			 0x55'5555'5555'5555ul,
			 0xFF'FFFF'FFFF'FFFFul,
			 0xA6'1239'FE42'FEDCul,
		 }) {
		SM72445::Reg1 reg1{Register(value)};
		Register	  regOnlyUsedBits = Register(value & 0x0000'00FF'FFFF'FFFFul);
		EXPECT_EQ(Register(reg1), regOnlyUsedBits);
	}
}

TEST(SM72445_Reg1, indexOperatorReturnsCorrespondingChannelValue) {
	SM72445::Reg1 reg0{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg0[ElectricalProperty::CURRENT_IN], 0x0u);
	EXPECT_EQ(reg0[ElectricalProperty::VOLTAGE_IN], 0x1u);
	EXPECT_EQ(reg0[ElectricalProperty::CURRENT_OUT], 0x2u);
	EXPECT_EQ(reg0[ElectricalProperty::VOLTAGE_OUT], 0x3u);
}

TEST(SM72445_Reg1, indexOperatorReturnsZeroIfGivenInvalidChannel) {
	SM72445::Reg1 reg1{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg1[static_cast<ElectricalProperty>(0xFFu)], 0x0u);
}

TEST(SM72445_Reg3, constructsWithRegisterValue) {
	SM72445::Reg3 reg3Zero{Register(0x0ul)};
	EXPECT_EQ(reg3Zero.overrideAdcProgramming, false);
	EXPECT_EQ(reg3Zero.a2Override, 0x0u);
	EXPECT_EQ(reg3Zero.iOutMax, 0x0u);
	EXPECT_EQ(reg3Zero.vOutMax, 0x0u);
	EXPECT_EQ(reg3Zero.tdOff, 0x0u);
	EXPECT_EQ(reg3Zero.tdOn, 0x0u);
	EXPECT_EQ(reg3Zero.dcOpen, 0x0u);
	EXPECT_EQ(reg3Zero.passThroughSelect, false);
	EXPECT_EQ(reg3Zero.passThroughManual, false);
	EXPECT_EQ(reg3Zero.bbReset, false);
	EXPECT_EQ(reg3Zero.clkOeManual, false);
	EXPECT_EQ(reg3Zero.openLoopOperation, false);

	SM72445::Reg3 reg3Max{Register(~0x0ul)};
	EXPECT_EQ(reg3Max.overrideAdcProgramming, true);
	EXPECT_EQ(reg3Max.a2Override, 0x7u);
	EXPECT_EQ(reg3Max.iOutMax, 0x3FFu);
	EXPECT_EQ(reg3Max.vOutMax, 0x3FFu);
	EXPECT_EQ(reg3Max.tdOff, 0x7u);
	EXPECT_EQ(reg3Max.tdOn, 0x7u);
	EXPECT_EQ(reg3Max.dcOpen, 0x1FFu);
	EXPECT_EQ(reg3Max.passThroughSelect, true);
	EXPECT_EQ(reg3Max.passThroughManual, true);
	EXPECT_EQ(reg3Max.bbReset, true);
	EXPECT_EQ(reg3Max.clkOeManual, true);
	EXPECT_EQ(reg3Max.openLoopOperation, true);

	SM72445::Reg3 reg3Alt1{Register(0x5555'5555'5555'5555ul)};
	EXPECT_EQ(reg3Alt1.overrideAdcProgramming, true);
	EXPECT_EQ(reg3Alt1.a2Override, 0x5u);
	EXPECT_EQ(reg3Alt1.iOutMax, 0x155u);
	EXPECT_EQ(reg3Alt1.vOutMax, 0x155u);
	EXPECT_EQ(reg3Alt1.tdOff, 0x2u);
	EXPECT_EQ(reg3Alt1.tdOn, 0x5u);
	EXPECT_EQ(reg3Alt1.dcOpen, 0x0AAu);
	EXPECT_EQ(reg3Alt1.passThroughSelect, true);
	EXPECT_EQ(reg3Alt1.passThroughManual, false);
	EXPECT_EQ(reg3Alt1.bbReset, true);
	EXPECT_EQ(reg3Alt1.clkOeManual, false);
	EXPECT_EQ(reg3Alt1.openLoopOperation, true);

	SM72445::Reg3 reg3Alt2{Register(0xAAAA'AAAA'AAAA'AAAAul)};
	EXPECT_EQ(reg3Alt2.overrideAdcProgramming, false);
	EXPECT_EQ(reg3Alt2.a2Override, 0x2u);
	EXPECT_EQ(reg3Alt2.iOutMax, 0x2AAu);
	EXPECT_EQ(reg3Alt2.vOutMax, 0x2AAu);
	EXPECT_EQ(reg3Alt2.tdOff, 0x5u);
	EXPECT_EQ(reg3Alt2.tdOn, 0x2u);
	EXPECT_EQ(reg3Alt2.dcOpen, 0x155u);
	EXPECT_EQ(reg3Alt2.passThroughSelect, false);
	EXPECT_EQ(reg3Alt2.passThroughManual, true);
	EXPECT_EQ(reg3Alt2.bbReset, false);
	EXPECT_EQ(reg3Alt2.clkOeManual, true);
	EXPECT_EQ(reg3Alt2.openLoopOperation, false);
}

TEST(SM72445_Reg3, registerConstructsBinaryRepresentation) {
	const Register reg3UsedBitsMask = 0x1'CFFF'FFFF'FFFFul;
	for (Register value : {
			 0x0ul,
			 0x1ul,
			 ~0x0ul,
			 0x5555'5555'5555'5555ul,
			 0xAAAA'AAAA'AAAA'AAAAul,
			 0xA6'1239'FE42'FEDCul,
		 }) {
		SM72445::Reg3  reg3{value};
		const Register regOnlyUsedBits = Register(reg3) & reg3UsedBitsMask;
		EXPECT_EQ(Register(reg3), regOnlyUsedBits);
	}
}

TEST(SM72445_Reg4, constructsWithRegisterValue) {
	SM72445::Reg4 reg4{Register(0x03020100ul)};
	EXPECT_EQ(reg4.iInOffset, 0x000u);
	EXPECT_EQ(reg4.vInOffset, 0x001u);
	EXPECT_EQ(reg4.iOutOffset, 0x002u);
	EXPECT_EQ(reg4.vOutOffset, 0x003u);

	SM72445::Reg4 reg4Max{Register(~0x0ul)};
	EXPECT_EQ(reg4Max.iInOffset, 0xFFu);
	EXPECT_EQ(reg4Max.vInOffset, 0xFFu);
	EXPECT_EQ(reg4Max.iOutOffset, 0xFFu);
	EXPECT_EQ(reg4Max.vOutOffset, 0xFFu);

	SM72445::Reg4 reg4Zeros{Register(0x0ul)};
	EXPECT_EQ(reg4Zeros.iInOffset, 0x000u);
	EXPECT_EQ(reg4Zeros.vInOffset, 0x000u);
	EXPECT_EQ(reg4Zeros.iOutOffset, 0x000u);
	EXPECT_EQ(reg4Zeros.vOutOffset, 0x000u);

	SM72445::Reg4 reg4Alt1{Register(0xAA'AAAA'AAAA'AAAAul)};
	EXPECT_EQ(reg4Alt1.iInOffset, 0xAAu);
	EXPECT_EQ(reg4Alt1.vInOffset, 0xAAu);
	EXPECT_EQ(reg4Alt1.iOutOffset, 0xAAu);
	EXPECT_EQ(reg4Alt1.vOutOffset, 0xAAu);

	SM72445::Reg4 reg4Alt2{Register(0x55'5555'5555'5555ul)};
	EXPECT_EQ(reg4Alt2.iInOffset, 0x55u);
	EXPECT_EQ(reg4Alt2.vInOffset, 0x55u);
	EXPECT_EQ(reg4Alt2.iOutOffset, 0x55u);
	EXPECT_EQ(reg4Alt2.vOutOffset, 0x55u);
}

TEST(SM72445_Reg4, registerConstructsBinaryRepresentation) {
	for (uint64_t value : {
			 0x0ul,
			 0x1ul,
			 ~0x0ul,
			 0xAA'AAAA'AAAA'AAAAul,
			 0x55'5555'5555'5555ul,
			 0xFF'FFFF'FFFF'FFFFul,
			 0xA6'1239'FE42'FEDCul,
		 }) {
		SM72445::Reg4 reg4{Register(value)};
		Register	  regOnlyUsedBits = Register(value & 0x0000'0000'FFFF'FFFFul);
		EXPECT_EQ(Register(reg4), regOnlyUsedBits);
	}
}

TEST(SM72445_Reg4, indexOperatorReturnsCorrespondingChannelValue) {
	SM72445::Reg4 reg4{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg4[ElectricalProperty::CURRENT_IN], 0x0u);
	EXPECT_EQ(reg4[ElectricalProperty::VOLTAGE_IN], 0x1u);
	EXPECT_EQ(reg4[ElectricalProperty::CURRENT_OUT], 0x2u);
	EXPECT_EQ(reg4[ElectricalProperty::VOLTAGE_OUT], 0x3u);
}

TEST(SM72445_Reg4, indexOperatorReturnsZeroIfGivenInvalidChannel) {
	SM72445::Reg4 reg4{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg4[static_cast<ElectricalProperty>(0xFFu)], 0x0u);
}

TEST(SM72445_Reg5, constructsWithRegisterValue) {
	SM72445::Reg5 reg5{Register(0x0C0200400ul)};
	EXPECT_EQ(reg5.iOutLow, 0x000u);
	EXPECT_EQ(reg5.iOutHigh, 0x001u);
	EXPECT_EQ(reg5.iInLow, 0x002u);
	EXPECT_EQ(reg5.iInHigh, 0x003u);

	SM72445::Reg5 reg5Max{Register(~0x0ul)};
	EXPECT_EQ(reg5Max.iOutLow, 0x3FFu);
	EXPECT_EQ(reg5Max.iOutHigh, 0x3FFu);
	EXPECT_EQ(reg5Max.iInLow, 0x3FFu);
	EXPECT_EQ(reg5Max.iInHigh, 0x3FFu);

	SM72445::Reg5 reg5Zeros{Register(0x0ul)};
	EXPECT_EQ(reg5Zeros.iOutLow, 0x000u);
	EXPECT_EQ(reg5Zeros.iOutHigh, 0x000u);
	EXPECT_EQ(reg5Zeros.iInLow, 0x000u);
	EXPECT_EQ(reg5Zeros.iInHigh, 0x000u);

	SM72445::Reg5 reg5Alt1{Register(0xAA'AAAA'AAAA'AAAAul)};
	EXPECT_EQ(reg5Alt1.iOutLow, 0x2AAu);
	EXPECT_EQ(reg5Alt1.iOutHigh, 0x2AAu);
	EXPECT_EQ(reg5Alt1.iInLow, 0x2AAu);
	EXPECT_EQ(reg5Alt1.iInHigh, 0x2AAu);

	SM72445::Reg5 reg5Alt2{Register(0x55'5555'5555'5555ul)};
	EXPECT_EQ(reg5Alt2.iOutLow, 0x155u);
	EXPECT_EQ(reg5Alt2.iOutHigh, 0x155u);
	EXPECT_EQ(reg5Alt2.iInLow, 0x155u);
	EXPECT_EQ(reg5Alt2.iInHigh, 0x155u);
}

TEST(SM72445_Reg5, registerConstructsBinaryRepresentation) {
	for (uint64_t value : {
			 0x0ul,
			 0x1ul,
			 ~0x0ul,
			 0xAA'AAAA'AAAA'AAAAul,
			 0x55'5555'5555'5555ul,
			 0xFF'FFFF'FFFF'FFFFul,
			 0xA6'1239'FE42'FEDCul,
		 }) {
		SM72445::Reg5 reg5{Register(value)};
		Register	  regOnlyUsedBits = Register(value & 0x0000'00FF'FFFF'FFFFul);
		EXPECT_EQ(Register(reg5), regOnlyUsedBits);
	}
}

TEST(SM72445_Reg5, indexOperatorReturnsCorrespondingChannelValue) {
	SM72445::Reg5 reg5{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg5[CurrentThreshold::CURRENT_OUT_LOW], 0x0u);
	EXPECT_EQ(reg5[CurrentThreshold::CURRENT_OUT_HIGH], 0x1u);
	EXPECT_EQ(reg5[CurrentThreshold::CURRENT_IN_LOW], 0x2u);
	EXPECT_EQ(reg5[CurrentThreshold::CURRENT_IN_HIGH], 0x3u);
}

TEST(SM72445_Reg5, indexOperatorReturnsZeroIfGivenInvalidChannel) {
	SM72445::Reg5 reg5{0x0u, 0x1u, 0x2u, 0x3u};
	EXPECT_EQ(reg5[static_cast<CurrentThreshold>(0xFFu)], 0x0u);
}