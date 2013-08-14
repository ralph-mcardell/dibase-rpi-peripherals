// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_ctrl.cpp 
/// @brief Internal GPIO clock control type implentation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_ctrl.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      clock_ctrl::clock_ctrl()
      : regs(clock_registers::physical_address, register_block_size)
      {}

      clock_ctrl & clock_ctrl::instance()
      {
        static clock_ctrl clock_control_area;
        return clock_control_area;
      }
    }
  }
}}

