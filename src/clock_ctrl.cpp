// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_ctrl.cpp 
/// @brief Internal GPIO clock control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_ctrl.h"
#include "clock_parameters.h"

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
 
      void clock_ctrl::initialise_clock
      ( clock_id clk
      , clock_parameters const & cp
      )
      {
        instance().regs->set_source(clk,cp.clk_source());
        instance().regs->set_mash(clk,cp.clk_mash());
        instance().regs->set_divi(clk,cp.clk_divi());
        instance().regs->set_divf(clk,cp.clk_divf());      
      }

      clock_id index_to_clock_id(unsigned i)
      {
        static clock_id clocks[] = { gp0_clk_id, gp1_clk_id
                                   , gp2_clk_id, pwm_clk_id
                                   };
        return clocks[i];
      }     
    }
  }
}}

