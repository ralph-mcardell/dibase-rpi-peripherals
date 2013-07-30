// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_registers_unittests.cpp 
/// @brief Unit tests for low-level clock manager control registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 6 General Purpose I/O (GPIO) 
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals;

typedef uint32_t RegisterType;
typedef unsigned char Byte;

TEST_CASE( "Unit-tests/clock_registers/field offsets"
         , "Clock control and divisor fields should have the expected offsets"
         )
{
  enum RegisterOffsets
  { PWM_CTRL_OFFSET=40, PWM_DIV_OFFSET=44  // NB: NOT HEX values!!
  , GP0_CTRL_OFFSET=0x70, GP0_DIV_OFFSET=0x74
  , GP1_CTRL_OFFSET=0x78, GP1_DIV_OFFSET=0x7c
  , GP2_CTRL_OFFSET=0x80, GP2_DIV_OFFSET=0x84
  };
  clock_registers clk_regs;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&clk_regs, 0xFF, sizeof(clk_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&clk_regs));
 
 // Set each 32-bit register to the value of its offset:
  clk_regs.pwm_ctrl = PWM_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_CTRL_OFFSET])==PWM_CTRL_OFFSET );
  clk_regs.pwm_div = PWM_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_DIV_OFFSET])==PWM_DIV_OFFSET );

  clk_regs.gp0_ctrl = GP0_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_CTRL_OFFSET])==GP0_CTRL_OFFSET );
  clk_regs.gp0_div = GP0_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_DIV_OFFSET])==GP0_DIV_OFFSET );

  clk_regs.gp1_ctrl = GP1_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_CTRL_OFFSET])==GP1_CTRL_OFFSET );
  clk_regs.gp1_div = GP1_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_DIV_OFFSET])==GP1_DIV_OFFSET );

  clk_regs.gp2_ctrl = GP2_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_CTRL_OFFSET])==GP2_CTRL_OFFSET );
  clk_regs.gp2_div = GP2_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_DIV_OFFSET])==GP2_DIV_OFFSET );
 }
 