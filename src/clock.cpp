// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock.cpp
/// @brief GPIO clock classes' implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_parameters.h"
#include "clock_ctrl.h"
#include "gpio_ctrl.h"
#include "gpio_alt_fn.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    using internal::clock_ctrl;

    static clock_id translate_id(clock::id id_)
    {
      return id_==clock::id::pwm ? pwm_clk_id :
             id_==clock::id::gp0 ? gp0_clk_id :
             id_==clock::id::gp1 ? gp1_clk_id :
                                   gp2_clk_id;
    }

    void clock::initialise
    ( hertz src_freq
    , clock_source src_type
    , clock_frequency const & freq
    )
    {
      using internal::clock_parameters;
      clock_parameters cp(src_type, src_freq, freq);
      clock_id clk{translate_id(clk_id)};
      clock_ctrl::instance().regs->set_source(clk,cp.clk_source());
      clock_ctrl::instance().regs->set_mash(clk,cp.clk_mash());
      clock_ctrl::instance().regs->set_divi(clk,cp.clk_divi());
      clock_ctrl::instance().regs->set_divf(clk,cp.clk_divf());
      freq_min = cp.frequency_min();
      freq_avg = cp.frequency_avg();
      freq_max = cp.frequency_max();      
    }

    void clock::start() const
    {
      clock_id clk{translate_id(clk_id)};
      if (!clock_ctrl::instance().regs->is_busy(clk))
        {
          clock_ctrl::instance().regs->set_enable(clk, true);
        }
    }

    void clock::stop() const
    {
      clock_id clk{translate_id(clk_id)};
      if (clock_ctrl::instance().regs->is_busy(clk))
        {
          clock_ctrl::instance().regs->set_enable(clk, false);
        }
    }

    bool clock::is_running() const
    {
      clock_id clk{translate_id(clk_id)};
      return clock_ctrl::instance().regs->is_busy(clk);
    }

    using internal::gpio_ctrl;

    clock::id clock_pin::pin_id_to_clock_id(pin_id pin)
    { // Select alt function decsriptors for pin for GPCLKn special functions
      using pin_alt_fn::result_set;
      using pin_alt_fn::select;
      using pin_alt_fn::gpio_special_fn;
      auto clk_fn_info=result_set(select( pin
                                        , { gpio_special_fn::gpclk0
                                          , gpio_special_fn::gpclk1 
                                          , gpio_special_fn::gpclk2 
                                          }
                                        )
                                  );
      if (clk_fn_info.empty())
        {
          throw std::invalid_argument
                  {"clock_pin::clock_pin: Pin has no supported clock function"};
        }
      if (clk_fn_info.size()!=1)
        {
          throw std::invalid_argument // NO pin has >1 clock function
                  {"clock_pin::clock_pin: Internal data error: more than one "
                   "pin alt function selected that supports a clock function."
                  };
        }
      auto clk_fn = clk_fn_info[0].special_fn();
      if ( clk_fn!=gpio_special_fn::gpclk0 
        && clk_fn!=gpio_special_fn::gpclk1 && clk_fn!=gpio_special_fn::gpclk2 )
        {
          throw std::invalid_argument // Expect one of selection special fns!
                  {"clock_pin::clock_pin: Internal data error: Unexpected "
                   "pin alt-fn special function - does not represent a "
                   "GPCLKn special function."
                  };
        }
      gpio_ctrl::instance().alloc.allocate(pin);
      gpio_ctrl::instance().regs->set_pin_function(pin,clk_fn_info[0].alt_fn());
      return  clk_fn==gpio_special_fn::gpclk0 ? clock::id::gp0 :
              clk_fn==gpio_special_fn::gpclk1 ? clock::id::gp1 :
                                                clock::id::gp2 ;
    }

    clock_pin::~clock_pin()
    {
      gpio_ctrl::instance().alloc.deallocate(pin);
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
