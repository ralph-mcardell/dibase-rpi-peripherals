// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_ctrl.cpp 
/// @brief Internal PWM control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pwm_ctrl.h"
#include "clock_ctrl.h"
#include "periexcept.h"
#include <thread>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      pwm_ctrl::pwm_ctrl()
      : regs(pwm_registers::physical_address, register_block_size)
      {}

      pwm_ctrl & pwm_ctrl::instance()
      {
        static pwm_ctrl pwm_control_area;
        return pwm_control_area;
      }

      void pwm_ctrl::set_clock(clock_parameters const & cp)
      {
        if (alloc.any_in_use())
          {
            throw peripheral_in_use
                  { "pwm_ctrl::set_pwm_clock : Unable to modify PWM clock "
                    "while one or more PWM channels is in use."
                  };
          }
        clock_ctrl & clk_ctrl(clock_ctrl::instance());
        if (clk_ctrl.alloc.is_in_use(pwmclk))
          { // If clock in use (stop) and deallocate it
             if (clk_ctrl.regs->is_busy(index_to_clock_id(pwmclk)))
              { // if running  stop it
                clk_ctrl.regs->set_enable(index_to_clock_id(pwmclk),false);
                std::this_thread::sleep_for(std::chrono::microseconds{10});
              }
            clk_ctrl.alloc.deallocate(pwmclk);
          }
        clk_ctrl.allocate_and_initialise_clock(pwmclk, cp); // should not throw
        clk_ctrl.regs->set_enable(index_to_clock_id(pwmclk),true);
      }
    }
  }
}}
