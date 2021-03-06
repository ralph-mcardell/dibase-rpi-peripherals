// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin.cpp 
/// @brief Single GPIO pin I/O class implementations.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin.h"
#include "gpio_ctrl.h"
#include <chrono>
#include <thread>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
      auto pud_wait_us(std::chrono::microseconds(10U));

      static void apply_pull(pin_id pin, unsigned mode)
      {
        if ( mode&ipin::pull_up && mode&ipin::pull_down )
          {
            throw std::invalid_argument
                  ("Cannot open ipin with both pull up and down enabled!");
          }

        gpio_ctrl::instance().regs->set_pull_up_down_mode
                                    ( mode&ipin::pull_up 
                                        ? gpio_pud_mode::enable_pull_up_control
                                    : mode&ipin::pull_down 
                                        ? gpio_pud_mode::enable_pull_down_control
                                        : gpio_pud_mode::off
                                    );
        std::this_thread::sleep_for(pud_wait_us);
        gpio_ctrl::instance().regs->assert_pin_pull_up_down_clock( pin );
        std::this_thread::sleep_for(pud_wait_us);
        gpio_ctrl::instance().regs->set_pull_up_down_mode(gpio_pud_mode::off);
        gpio_ctrl::instance().regs->remove_all_pin_pull_up_down_clocks();
      }
    }

    pin_base::pin_base(pin_id pin, direction_mode dir)
    : pin(pin)
    {
      using internal::gpio_pin_fn;
      internal::gpio_ctrl::instance().alloc.allocate(pin);
      internal::gpio_ctrl::instance().regs->set_pin_function
                                            ( pin
                                            , (dir==out) ? gpio_pin_fn::output
                                                         : gpio_pin_fn::input
                                            
                                            );
    }

    pin_base::~pin_base()
    {
      internal::gpio_ctrl::instance().alloc.deallocate(pin);
    }

    void opin::put( bool v )
    {
      if ( v )
        {
          internal::gpio_ctrl::instance().regs->set_pin(get_pin());
        }
      else
        {
          internal::gpio_ctrl::instance().regs->clear_pin(get_pin());
        }
    }

    ipin::~ipin()
    {
      internal::apply_pull(get_pin(), pull_disable);
    }

    ipin::ipin(pin_id pin, unsigned mode)
    : pin_base(pin, in)
    {
      internal::apply_pull(pin, mode);
    }

    bool ipin::get()
    {
        return internal::gpio_ctrl::instance().regs->pin_level(get_pin());
    }
  }
}}
