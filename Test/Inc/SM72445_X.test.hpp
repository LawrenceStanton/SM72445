/**
 ******************************************************************************
 * @file			: SM72445_X.test.hpp
 * @brief			: BRIEF
 * @author			: Lawrence Stanton
 ******************************************************************************
 */

#pragma once

#include "SM72445.test.hpp"

class SM72445_X_Test : public SM72445_Test {
public:
	SM72445_X sm72445{i2c, SM72445::DeviceAddress::ADDR001, .5f, .5f, .5f, .5f};
};