// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_ctrl.cpp 
/// @brief Internal I2C control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "i2c_ctrl.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      namespace
      {
        static register_t physical_addresses[] =
                { i2c_registers::bsc0_physical_address
                , i2c_registers::bsc1_physical_address
                , i2c_registers::bsc2_physical_address
                };
      }

      i2c_ctrl::reg_ptr & i2c_ctrl::regs(std::size_t idx)
      {
        if (register_blocks[idx].get()==nullptr)
          {
            register_blocks[idx] = reg_ptr( physical_addresses[idx]
                                          , register_block_size
                                          );
          }
        return register_blocks[idx];
      }
      
      i2c_ctrl & i2c_ctrl::instance()
      {
        static i2c_ctrl i2c_control_area;
        return i2c_control_area;
      }
    }
  }
}}
