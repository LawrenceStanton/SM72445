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

protected:
	I2C			 *i2c;
	DeviceAddress deviceAddress;

public:
	SM72445(I2C *i2c, DeviceAddress deviceAddress);
};
