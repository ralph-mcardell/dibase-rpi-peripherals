// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_ctrl.cpp 
/// @brief Internal GPIO control type implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "gpio_ctrl.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      gpio_ctrl::gpio_ctrl()
      : regs(gpio_registers::physical_address, register_block_size)
      {}

      gpio_ctrl & gpio_ctrl::instance()
      {
        static gpio_ctrl gpio_control_area;
        return gpio_control_area;
      }
    }
  }
}}
