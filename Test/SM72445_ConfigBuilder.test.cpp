/**
 ******************************************************************************
 * @file			: SM72445_ConfigBuilder.test.cpp
 * @brief			: Tests for SM72445_ConfigBuilder related functions.
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "SM72445_X.hpp"

using ConfigBuilder = SM72445_X::ConfigBuilder;
using Config		= SM72445_X::Config;
using Register		= SM72445::I2C::Register;

using DeadTime		= Config::DeadTime;
using FrequencyMode = Config::FrequencyMode;
using PanelMode		= Config::PanelMode;

using ::testing::Eq;
using ::testing::Return;

using std::nullopt;

class MockedI2C : public SM72445::I2C {
public:
	MOCK_METHOD(
		optional<Register>,
		read,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress),
		(final)
	);
	MOCK_METHOD(
		optional<Register>,
		write,
		(DeviceAddress deviceAddress, MemoryAddress memoryAddress, Register data),
		(final)
	);
};

class SM72445_ConfigBuilderTest : public ::testing::Test {
public:
	MockedI2C i2c{};

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

	ConfigBuilder builder = sm72445.getConfigBuilder();

	void setDeadTimeTest(uint8_t bitsOffset, std::function<void(DeadTime)> setDeadTime);
};

TEST_F(SM72445_ConfigBuilderTest, getConfigBuilderByDefaultDoesNotFetchReg3) {
	EXPECT_CALL(i2c, read).Times(0);
	sm72445.getConfigBuilder();
}

TEST_F(SM72445_ConfigBuilderTest, getConfigBuilderFetchesReg3OnceIfAskedTo) {
	const Register testNondefaultRegValue = 0x1ull;
	EXPECT_CALL(i2c, read).WillOnce(Return(testNondefaultRegValue));
	auto configBuilder = sm72445.getConfigBuilder(true);
	EXPECT_EQ(testNondefaultRegValue, configBuilder.build());
}

TEST_F(SM72445_ConfigBuilderTest, getConfigBuilderDefaultsToUnfetchedIfReg3FetchFails) {
	Register reg3Default = 0x00'FF'FF'F6'DF'E0;
	EXPECT_CALL(i2c, read).WillOnce(Return(nullopt));
	auto configBuilder = sm72445.getConfigBuilder(true);
	EXPECT_EQ(reg3Default, configBuilder.build());
}

TEST_F(SM72445_ConfigBuilderTest, resetAdcProgrammingOverrideEnableResetsBit) {
	const Register overrideBit = 0x1ull << 46u;
	builder.setPanelModeOverride(PanelMode::USE_H_BRIDGE); // Sets as side effect.
	EXPECT_EQ(builder.build() & overrideBit, overrideBit);
	builder.resetAdcProgrammingOverrideEnable();
	EXPECT_EQ(builder.build() & overrideBit, 0x0ull);
}

TEST_F(SM72445_ConfigBuilderTest, setFrequencyModeOverrideSetsA2OverrideEnable) {
	const Register overrideBit = 0x1ull << 46u;
	EXPECT_EQ(builder.build() & overrideBit, 0x0ull);
	builder.setFrequencyModeOverride(FrequencyMode::HIGH); // Don't care value
	EXPECT_EQ(builder.build() & overrideBit, overrideBit);
}

TEST_F(SM72445_ConfigBuilderTest, setPanelModeOverrideSetsA2OverrideEnable) {
	const Register overrideBit = 0x1ull << 46u;
	EXPECT_EQ(builder.build() & overrideBit, 0x0ull);
	builder.setPanelModeOverride(PanelMode::USE_H_BRIDGE); // Don't care value
	EXPECT_EQ(builder.build() & overrideBit, overrideBit);
}

TEST_F(SM72445_ConfigBuilderTest, setFrequencyAndPanelModeOverrideSetsExpectedBits) {
	const Register a2OverrideBits = 0x7ull << 40u;

	// Map of expected values. Ref SM72445 Datasheet Table 1.
	std::map<std::pair<FrequencyMode, PanelMode>, Register> expectedValues = {
		{{FrequencyMode::HIGH, PanelMode::USE_SWITCH}, 0b000ull << 40},
		{{FrequencyMode::MED, PanelMode::USE_SWITCH}, 0b001ull << 40},
		{{FrequencyMode::LOW, PanelMode::USE_SWITCH}, 0b010ull << 40},
		{{FrequencyMode::HIGH, PanelMode::USE_H_BRIDGE}, 0b011ull << 40},
		{{FrequencyMode::MED, PanelMode::USE_H_BRIDGE}, 0b100ull << 40},
		{{FrequencyMode::LOW, PanelMode::USE_H_BRIDGE}, 0b101ull << 40},
		// ? Possible higher values not implemented by driver.
	};

	for (const auto &[key, expectedValue] : expectedValues) {
		const auto &[frequencyMode, panelMode] = key;

		builder.setFrequencyModeOverride(frequencyMode);
		builder.setPanelModeOverride(panelMode);
		EXPECT_EQ(builder.build() & a2OverrideBits, expectedValue);

		// Repeat to test for idempotent behaviour.
		builder.setFrequencyModeOverride(frequencyMode);
		EXPECT_EQ(builder.build() & a2OverrideBits, expectedValue);
	}
}

TEST_F(SM72445_ConfigBuilderTest, setMaxOutputCurrentOverrideSetsExpectedBinaryValues) {
	const uint8_t iOutMaxRegOffset = 30u;

	// Below values hardcoded to match test gain values.
	EXPECT_EQ(this->iOutGainTest, 0.4f);
	EXPECT_EQ(this->vDdaTest, 5.0f);
	std::map<float, Register> expectedValues = {
		{0.0f, 0b0000000000ull},
		{0.0125f, 0b0000000001ull},
		{0.1f, 0b0000001000ull},
		{0.2f, 0b0000010000ull},
		{0.5f, 0b0000101000ull},
		{1.0f, 0b0001010001ull},
		{5.0f, 0b0110011001ull},
		{6.25f, 0b0111111111ull},
		{12.5f, 0b1111111111ull},
	};

	for (const auto &[current, expectedValue] : expectedValues) {
		builder.setMaxOutputCurrentOverride(current);
		const uint16_t iOutMaxSetBits = (builder.build() >> iOutMaxRegOffset) & 0x3FFu;
		EXPECT_EQ(iOutMaxSetBits, expectedValue)
			<< "Failed on current value " << current << "A"
			<< " with expected binary value 0x" << std::hex << expectedValue << ".\n"
			<< "Actual binary value was 0x" << std::hex
			<< ((builder.build() >> iOutMaxRegOffset) & 0x3FFu) << ".";
	}
}

TEST_F(
	SM72445_ConfigBuilderTest,
	setMaxOutputCurrentWillDefaultToZeroIfRequestedValueExceedsRange
) {
	const uint8_t  iOutMaxRegOffset = 30u;
	const uint16_t maxCurrent		= this->vDdaTest / this->iOutGainTest;
	builder.setMaxOutputCurrentOverride(maxCurrent + 1); // Should trigger out of range
	EXPECT_EQ(builder.build() >> iOutMaxRegOffset & 0x3FFu, 0x0ull);
}

TEST_F(SM72445_ConfigBuilderTest, setMaxOutputVoltageOverrideSetsExpectedBinaryValues) {
	const uint8_t vOutMaxRegOffset = 20u;

	// Below values hardcoded to match test gain values.
	EXPECT_EQ(this->vOutGainTest, 0.2f);
	EXPECT_EQ(this->vDdaTest, 5.0f);
	std::map<float, Register> expectedValues = {
		{0.0f, 0b0000000000ull},
		{0.025f, 0b0000000001ull},
		{0.1f, 0b0000000100ull},
		{0.2f, 0b0000001000ull},
		{0.5f, 0b0000010100ull},
		{1.0f, 0b0000101000ull},
		{5.0f, 0b0011001100ull},
		{6.25f, 0b0011111111ull},
		{12.5f, 0b0111111111ull},
		{25.0f, 0b1111111111ull},
	};

	for (const auto &[voltage, expectedValue] : expectedValues) {
		builder.setMaxOutputVoltageOverride(voltage);
		const uint16_t vOutMaxSetBits = (builder.build() >> vOutMaxRegOffset) & 0x3FFu;
		EXPECT_EQ(vOutMaxSetBits, expectedValue)
			<< "Failed on voltage value " << voltage << "V"
			<< " with expected binary value 0x" << std::hex << expectedValue << ".\n"
			<< "Actual binary value was 0x" << std::hex
			<< ((builder.build() >> vOutMaxRegOffset) & 0x3FFu) << ".";
	}
}

TEST_F(
	SM72445_ConfigBuilderTest,
	setMaxOutputVoltageWillDefaultToZeroIfRequestedValueExceedsRange
) {
	const uint8_t  vOutMaxRegOffset = 20u;
	const uint16_t maxVoltage		= this->vDdaTest / this->vOutGainTest;
	builder.setMaxOutputVoltageOverride(maxVoltage + 1); // Should trigger out of range
	EXPECT_EQ(builder.build() >> vOutMaxRegOffset & 0x3FFu, 0x0ull);
}

void SM72445_ConfigBuilderTest::setDeadTimeTest(
	uint8_t						  bitsOffset,
	std::function<void(DeadTime)> setDeadTime
) {
	std::map<DeadTime, Register> expectedValues = {
		{DeadTime::ZERO, 0b000ull},
		{DeadTime::ONE, 0b001ull},
		{DeadTime::TWO, 0b010ull},
		{DeadTime::THREE, 0b011ull},
		{DeadTime::FOUR, 0b100ull},
		{DeadTime::FIVE, 0b101ull},
		{DeadTime::SIX, 0b110ull},
		{DeadTime::MAX, 0b111ull},
	};

	for (const auto &[deadTime, expectedValue] : expectedValues) {
		setDeadTime(deadTime);
		const uint16_t tDeadBits = (builder.build() >> bitsOffset) & 0x7u;
		EXPECT_EQ(tDeadBits, expectedValue);
	}
};

TEST_F(SM72445_ConfigBuilderTest, setDeadTimeOffTimeOverrideSetsExpectedBinaryValues) {
	setDeadTimeTest(17u, [&](DeadTime deadTime) {
		builder.setDeadTimeOffTimeOverride(deadTime);
	});
}

TEST_F(SM72445_ConfigBuilderTest, setDeadTimeOnTimeOverrideSetsExpectedBinaryValues) {
	setDeadTimeTest(14u, [&](DeadTime deadTime) {
		builder.setDeadTimeOnTimeOverride(deadTime);
	});
}

TEST_F(SM72445_ConfigBuilderTest, resetPanelModeRegisterOverrideEnableResetsBit) {
	const Register overrideEnableBit = 0x1ull << 4u;
	builder.setPanelModeRegisterOverride(true); // Sets as side effect.
	EXPECT_EQ(builder.build() & overrideEnableBit, overrideEnableBit);
	builder.resetPanelModeRegisterOverrideEnable();
	EXPECT_EQ(builder.build() & overrideEnableBit, 0x0ull);
}

TEST_F(SM72445_ConfigBuilderTest, setPanelModeRegisterOverrideSetsExpectedBits) {
	const Register overrideBit = 0x1ull << 3u;
	EXPECT_EQ(builder.build() & overrideBit, 0x0ull);
	builder.setPanelModeRegisterOverride(true);
	EXPECT_EQ(builder.build() & overrideBit, overrideBit);
	builder.setPanelModeRegisterOverride(false);
	EXPECT_EQ(builder.build() & overrideBit, 0x0ull);
}

TEST_F(SM72445_ConfigBuilderTest, setPanelModeRegisterOverrideSetsOverrideEnableBit) {
	const Register overrideEnableBit = 0x1ull << 4u;
	EXPECT_EQ(builder.build() & overrideEnableBit, 0x0ull);
	builder.setPanelModeRegisterOverride(true);
	EXPECT_EQ(builder.build() & overrideEnableBit, overrideEnableBit);
}
