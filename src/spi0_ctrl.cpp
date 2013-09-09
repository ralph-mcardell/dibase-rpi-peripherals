// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_ctrl.cpp
/// @brief Internal SPI0 control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "spi0_ctrl.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      spi0_ctrl::spi0_ctrl()
      : regs(spi0_registers::physical_address, register_block_size)
      , allocated(false)
      {}

      spi0_ctrl & spi0_ctrl::instance()
      {
        static spi0_ctrl spi0_control_area;
        return spi0_control_area;
      }
    }
  }
}}
