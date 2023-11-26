/**
 ******************************************************************************
 * @file			: SM72445_Config.test.cpp
 * @brief			: Tests for SM72445::Config_ related structures.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445_X.test.hpp"

#include <bitset>

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;

using Register = SM72445::Register;

using Config = SM72445_X::Config;
using Reg3	 = SM72445::Reg3;

using PanelMode		= Config::PanelMode;
using FrequencyMode = Config::FrequencyMode;
using DeadTime		= Config::DeadTime;

using std::nullopt;

class SM72445_Config : public SM72445_X_Test {
public:
	// Don't care but randomised to reveal any arithmetic errors.
	const float vInGainTest	 = .1f;
	const float vOutGainTest = .2f;
	const float iInGainTest	 = .3f;
	const float iOutGainTest = .4f;
	const float vDdaTest	 = 5.0f;

	const SM72445_X sm72445{
		i2c,
		SM72445::I2C::DeviceAddress::ADDR001,
		vInGainTest,
		vOutGainTest,
		iInGainTest,
		iOutGainTest,
		vDdaTest,
	};
};

TEST_F(SM72445_Config, getConfigReturnsExpectedConfig) {
	// Expected values hardcoded to these test parameters.
	EXPECT_EQ(this->vDdaTest, 5.0f);
	EXPECT_EQ(this->iOutGainTest, 0.4f);
	EXPECT_EQ(this->vOutGainTest, 0.2f);

	// Mirror aggregate type for Config values.
	struct ConfigValues {
		bool		  overrideAdcProgramming;
		FrequencyMode frequencyMode;
		PanelMode	  panelMode;
		float		  iOutMax;
		float		  vOutMax;
		DeadTime	  tdOff;
		DeadTime	  tdOn;
		bool		  panelModeOverrideEnable;
		bool		  panelModeOverride;
		bool		  clockOutputManualEnable;
		bool		  openLoopOperation;

		bool operator==(const Config &realConfig) const {
			EXPECT_EQ(overrideAdcProgramming, realConfig.overrideAdcProgramming);
			EXPECT_EQ(frequencyMode, realConfig.frequencyMode);
			EXPECT_EQ(panelMode, realConfig.panelMode);
			EXPECT_FLOAT_EQ(iOutMax, realConfig.iOutMax);
			EXPECT_FLOAT_EQ(vOutMax, realConfig.vOutMax);
			EXPECT_EQ(tdOff, realConfig.tdOff);
			EXPECT_EQ(tdOn, realConfig.tdOn);
			EXPECT_EQ(panelModeOverrideEnable, realConfig.panelModeOverrideEnable);
			EXPECT_EQ(panelModeOverride, realConfig.panelModeOverride);
			EXPECT_EQ(clockOutputManualEnable, realConfig.clockOutputManualEnable);
			EXPECT_EQ(openLoopOperation, realConfig.openLoopOperation);

			return overrideAdcProgramming == realConfig.overrideAdcProgramming	 //
				&& frequencyMode == realConfig.frequencyMode					 //
				&& panelMode == realConfig.panelMode							 //
				&& iOutMax == realConfig.iOutMax								 //
				&& vOutMax == realConfig.vOutMax								 //
				&& tdOff == realConfig.tdOff									 //
				&& tdOn == realConfig.tdOn										 //
				&& panelModeOverrideEnable == realConfig.panelModeOverrideEnable //
				&& panelModeOverride == realConfig.panelModeOverride			 //
				&& clockOutputManualEnable == realConfig.clockOutputManualEnable //
				&& openLoopOperation == realConfig.openLoopOperation;
		}
	};

	std::map<Register, ConfigValues> testValues = {
		{
			// Used fields all 1s. Others 0s.
			0b1'0'01'111'1111111111'1111111111'111'111'000000000'1'1'0'1'1ull,
			{
				true,
				// Rsvd bits.
				FrequencyMode::HIGH,
				PanelMode::USE_SWITCH,
				12.5f,
				25.0f,
				DeadTime::MAX,
				DeadTime::MAX,
				// Unused DC Open
				true,
				true,
				// Unused bbReset
				true,
				true,
			},
		},
		{
			// Used fields all 0s. Others 1s.
			0b0'1'01'000'0000000000'0000000000'000'000'111111111'0'0'1'0'0ull,
			{
				false,
				// Rsvd bits
				FrequencyMode::HIGH,
				PanelMode::USE_SWITCH,
				0.0f,
				0.0f,
				DeadTime::ZERO,
				DeadTime::ZERO,
				// Unused DC OPen
				false,
				false,
				// Unused bbReset
				false,
				false,
			},
		},
		{
			// Alternating pattern 1
			0b1'0'01'101'0101010101'0101010101'010'010'101010101'1'0'1'0'1ull,
			{
				true,
				// Rsvd bits
				FrequencyMode::LOW,
				PanelMode::USE_H_BRIDGE,
				4.1666667f,
				8.3333333f,
				DeadTime::TWO,
				DeadTime::TWO,
				// Unused DC Open
				true,
				false,
				// Unused bbReset
				false,
				true,
			},
		},
		{
			// Alternating pattern 2
			0b0'1'01'010'1010101010'1010101010'101'101'010101010'0'1'0'1'0ull,
			{
				false,
				// Rsvd bits
				FrequencyMode::LOW,
				PanelMode::USE_SWITCH,
				8.3333333f,
				16.666666f,
				DeadTime::FIVE,
				DeadTime::FIVE,
				// Unused DC Open
				false,
				true,
				// Unused bbReset
				true,
				false,
			},
		},
		{0b1'0'01'000'0101010101'1010101010'001'110'000000000'0'0'0'0'0ull,
		 {
			 true,					// Don't care
									// Rsvd bits
			 FrequencyMode::HIGH,	// Don't care
			 PanelMode::USE_SWITCH, // Don't care
			 4.1666667f,
			 16.666666f,
			 DeadTime::ONE,
			 DeadTime::SIX,
			 // Unused DC Open
			 false, // Don't care
			 false, // Don't care
					// Unused bbReset
			 false, // Don't care
			 false, // Don't care
		 }}
	};

	for (const auto &[testReg3Value, expected] : testValues) {
		EXPECT_CALL(i2c, read).WillOnce(Return(testReg3Value));
		auto config = sm72445.getConfig();
		EXPECT_TRUE(config.has_value());
		EXPECT_EQ(expected, *config)
			<< "Failed on value " << std::bitset<46>(testReg3Value);
	}
}

TEST_F(SM72445_Config, setConfigNormallyWritesValuesToReg3) {
	const Register testReg3Value = 0x1ull;
	EXPECT_CALL(i2c, write(_, Eq(SM72445::MemoryAddress::REG3), _))
		.WillOnce(Return(testReg3Value));
	auto result = sm72445.setConfig(testReg3Value);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(*result, testReg3Value);
}

TEST_F(SM72445_Config, setConfigReturnsNulloptIfWriteFails) {
	EXPECT_CALL(i2c, write).WillRepeatedly(Return(nullopt));
	auto result = sm72445.setConfig(0x1ull);
	EXPECT_FALSE(result.has_value());
}
