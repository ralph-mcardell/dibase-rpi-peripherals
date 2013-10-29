// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_ctrl_platformtests.cpp 
/// @brief Platform tests for internal I2C/BSC master control area type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 3 BSC
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "i2c_ctrl.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals::internal;

TEST_CASE( "Platform_tests/i2c_ctrl/0000/i2c_registers pointer getter"
         , "i2c_ctrl::regs(index) should return valid and constant "
           "smart pointers to i2c_registers mapped to the BSC control area "
           "indicated by the index parameter"
         )
{
  i2c_ctrl & ctrl(i2c_ctrl::instance());

  volatile i2c_registers * reg_addrs_bsc0(ctrl.regs(0).get());
  CHECK(reg_addrs_bsc0!=0); 
  CHECK(ctrl.regs(0).get()==reg_addrs_bsc0);

  volatile i2c_registers * reg_addrs_bsc1(ctrl.regs(1).get());
  CHECK(reg_addrs_bsc1!=0); 
  CHECK(ctrl.regs(1).get()==reg_addrs_bsc1);

  volatile i2c_registers * reg_addrs_bsc2(ctrl.regs(2).get());
  CHECK(reg_addrs_bsc2!=0); 
  CHECK(ctrl.regs(2).get()==reg_addrs_bsc2);

  CHECK(reg_addrs_bsc0!=reg_addrs_bsc1);
  CHECK(reg_addrs_bsc0!=reg_addrs_bsc2);
  CHECK(reg_addrs_bsc1!=reg_addrs_bsc2);
}
