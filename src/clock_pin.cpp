// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin.cpp
/// @brief GPIO clock classes' implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_pin.h"
#include "clock_parameters.h"
#include "clock_ctrl.h"
#include "gpio_ctrl.h"
#include "gpio_alt_fn.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    using internal::clock_ctrl;
    using internal::gpclk0;
    using internal::gpclk1;
    using internal::gpclk2;
    using internal::index_to_clock_id;

    bool clock_pin::is_running() const
    {
      return clock_ctrl::instance().regs->is_busy(index_to_clock_id(clk));
    }

    void clock_pin::start() const
    {
      if (!is_running())
        {
          clock_ctrl::instance().regs->set_enable(index_to_clock_id(clk), true);
        }
    }

    void clock_pin::stop() const
    {
      if (is_running())
        {
          clock_ctrl::instance().regs->set_enable(index_to_clock_id(clk),false);
        }
    }

    using internal::gpio_ctrl;

    clock_pin::~clock_pin()
    {
      stop();
      gpio_ctrl::instance().alloc.deallocate(pin);
      clock_ctrl::instance().alloc.deallocate(clk);
    }

    unsigned clock_pin::construct
    ( pin_id pin
    , hertz src_freq
    , clock_source src_type
    , clock_frequency const & freq
    )
    { // Select alt function descriptors for pin for GPCLKn special functions
      using internal::pin_alt_fn::result_set;
      using internal::pin_alt_fn::select;
      using internal::pin_alt_fn::gpio_special_fn;
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
          throw std::range_error // NO pin has >1 clock function
                  {"clock_pin::clock_pin: Internal data error: more than one "
                   "pin alt function selected that supports a clock function."
                  };
        }
      auto clk_fn = clk_fn_info[0].special_fn();
      if ( clk_fn!=gpio_special_fn::gpclk0 
        && clk_fn!=gpio_special_fn::gpclk1 && clk_fn!=gpio_special_fn::gpclk2 )
        {
          throw std::range_error // Expect one of special fns passed to select!
                  {"clock_pin::clock_pin: Internal data error: Unexpected "
                   "pin alt-fn special function - does not represent a "
                   "GPCLKn special function."
                  };
        }
      gpio_ctrl::instance().alloc.allocate(pin);
      auto clk_idx( clk_fn==gpio_special_fn::gpclk0 ? gpclk0 :
                    clk_fn==gpio_special_fn::gpclk1 ? gpclk1 :
                                                      gpclk2 
                  );
      try
      {
        using internal::clock_parameters;
        clock_parameters cp(src_type, src_freq, freq); // CAN THROW!!!
        clock_ctrl::instance().allocate_and_initialise_clock(clk_idx, cp); // CAN THROW!!!
        freq_min = cp.frequency_min();
        freq_avg = cp.frequency_avg();
        freq_max = cp.frequency_max();
      }
      catch (...)
      {
        gpio_ctrl::instance().alloc.deallocate(pin);
        throw; 
      }
      gpio_ctrl::instance().regs->set_pin_function(pin,clk_fn_info[0].alt_fn());
      return clk_idx;
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
