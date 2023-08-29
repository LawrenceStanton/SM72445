/**
 ******************************************************************************
 * @file			: SM72445.cpp
 * @brief			: Source for SM72445.hpp
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#include "SM72445.hpp"

using DeviceAddress = SM72445::DeviceAddress;

using std::nullopt;

SM72445::SM72445(I2C &i2c, DeviceAddress deviceAddress)
	: i2c{i2c}, deviceAddress{deviceAddress} {}

template <typename Reg>
optional<Reg> SM72445::getRegister(SM72445::MemoryAddress memoryAddress) const {
	auto transmission = this->i2c.read(this->deviceAddress, memoryAddress);

	if (!transmission) return nullopt;

	Reg reg{*transmission};
	return reg;
}

optional<SM72445::Reg0> SM72445::getAnalogueChannelRegister(void) const {
	return getRegister<Reg0>(MemoryAddress::REG0);
}

optional<SM72445::Reg1> SM72445::getElectricalMeasurementsRegister(void) const {
	return getRegister<Reg1>(MemoryAddress::REG1);
}

optional<SM72445::Reg3> SM72445::getConfigRegister(void) const {
	return getRegister<Reg3>(MemoryAddress::REG3);
}

optional<SM72445::Reg4> SM72445::getOffsetRegister(void) const {
	return getRegister<Reg4>(MemoryAddress::REG4);
}

optional<SM72445 ::Reg5> SM72445::getThresholdRegister(void) const {
	return getRegister<Reg5>(MemoryAddress::REG5);
}

DeviceAddress SM72445::getDeviceAddress(void) const {
	return this->deviceAddress;
}
