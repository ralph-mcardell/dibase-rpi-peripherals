// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_pins.cpp
/// @brief Use a pair of GPIO pins for use with I2C/BSC: implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "i2c_pins.h"
#include "gpio_alt_fn.h"
#include "gpio_ctrl.h"
#include "i2c_ctrl.h"
#include "periexcept.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    using internal::i2c_ctrl;
    using internal::gpio_ctrl;
    using internal::pin_alt_fn::descriptor;
    using internal::pin_alt_fn::gpio_special_fn;
    using internal::gpio_pin_fn;

    namespace
    {
      descriptor get_alt_fn_descriptor
      ( pin_id pin
      , std::initializer_list<gpio_special_fn> special_fns
      )
      {
        using internal::pin_alt_fn::result_set;
        using internal::pin_alt_fn::select;
        auto pin_fn_info( select(pin,special_fns) );
        if (pin_fn_info.empty())
          {
            throw std::invalid_argument
                  { "i2c_pins::i2c_pins: Pin does not support "
                    "requested I2C/BSC special function."
                  };
          } 
        if (pin_fn_info.size()!=1)
          {
            throw std::range_error // this is possible if wrong ctor/pins combo
                  {"i2c_pins::i2c_pins: More than one pin alt function "
                   "selected that supports the requested I2C/BSC special "
                   "function."
                  };
          }
        return pin_fn_info[0];
      }
    }

    constexpr auto sda_idx(0U);
    constexpr auto scl_idx(1U);
    constexpr pin_id_int_t pin_not_used{53U};

    i2c_pins::i2c_pins
    ( pin_id        sda_pin
    , pin_id        scl_pin
    , hertz         f
    , std::uint16_t tout
    , std::uint16_t fedl
    , std::uint16_t redl
    , hertz         fc
    )
    {
      pins.fill(pin_id(pin_not_used)); 
    
    // Get each pin's alt function for its BSC/I2C special function.
    // Note: any of these can throw - but nothing allocated yet so OK
      descriptor alt_fn_info[number_of_pins];
      alt_fn_info[sda_idx] = get_alt_fn_descriptor( sda_pin
                                                  , { gpio_special_fn::sda0
                                                    , gpio_special_fn::sda1
                                                    }
                                                  );
      alt_fn_info[scl_idx] = get_alt_fn_descriptor( scl_pin
                                                  , { gpio_special_fn::scl0
                                                    , gpio_special_fn::scl1
                                                    }
                                                  );
      if (  (   alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda0
            &&  alt_fn_info[scl_idx].special_fn()!=gpio_special_fn::scl0
            )
         || (   alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda1
            &&  alt_fn_info[scl_idx].special_fn()!=gpio_special_fn::scl1
            )
         )
        {
          throw std::invalid_argument
                { "i2c_pins::i2c_pins: Pins do not support I2C/BSC "
                  "SDA and SCL functions for the same BSC peripheral."
                };
        }

      using internal::i2c_registers;
      i2c_registers ctx_builder;
      ctx_builder.control = 0U;
      ctx_builder.clk_div = 0U;
      ctx_builder.data_delay = 0U;
      ctx_builder.clk_stretch = 0U;

      register_t cdiv{fc.count()/f.count()};
      if (!ctx_builder.set_clock_divider(cdiv))
        {
          throw std::out_of_range( "i2c_pins::i2c_pins: f parameter "
                                   "not in the range [fc/32768,fc/2]."
                                 );
        }
      if (redl>(cdiv/2))
        {
          throw std::out_of_range( "i2c_pins::i2c_pins: redl parameter "
                                   "exceeds (fc/f)/2 (CDIV/2)."
                                 );          
        }
      if (fedl>(cdiv/2))
        {
          throw std::out_of_range( "i2c_pins::i2c_pins: fedl parameter "
                                   "exceeds (fc/f)/2 (CDIV/2)."
                                 );          
        }
      ctx_builder.set_read_delay(redl);
      ctx_builder.set_write_delay(fedl);
      ctx_builder.set_clock_stretch_timeout(tout);
      ctx_builder.set_enable(true);
      ctx_builder.clear_fifo();
      unsigned idx{alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda0?0:1};
      if (i2c_ctrl::instance().alloc.is_in_use(idx))
        {
          throw bad_peripheral_alloc( "i2c_pins::i2c_pins: BSC peripheral is "
                                      "already being used locally."
                                    );
        }
      i2c_ctrl::instance().alloc.allocate(idx);
      try
      {
        gpio_ctrl::instance().alloc.allocate(sda_pin); // CAN THROW
        pins[sda_idx] = sda_pin;
        gpio_ctrl::instance().alloc.allocate(scl_pin); // CAN THROW
        pins[scl_idx] = scl_pin;
      }
      catch (...)
      { // Oops - failed to complete resource acquisition and initialisation;
      // Release resources allocated so far and re-throw
        if (pins[sda_idx]!=pin_not_used)
          {
            gpio_ctrl::instance().alloc.deallocate(pin_id(pins[sda_idx]));
          }
        i2c_ctrl::instance().alloc.deallocate(idx);
        throw;
      }
      
      gpio_ctrl::instance().regs->set_pin_function( pin_id(pins[sda_idx])
                                                  , alt_fn_info[sda_idx].alt_fn()
                                                  );
      gpio_ctrl::instance().regs->set_pin_function( pin_id(pins[scl_idx])
                                                  , alt_fn_info[scl_idx].alt_fn()
                                                  );
      bsc_idx = idx;
      i2c_ctrl::instance().regs(idx)->clk_div = ctx_builder.clk_div;
      i2c_ctrl::instance().regs(idx)->data_delay = ctx_builder.data_delay;
      i2c_ctrl::instance().regs(idx)->clk_stretch = ctx_builder.clk_stretch;
      i2c_ctrl::instance().regs(idx)->control = ctx_builder.control;
    }

    i2c_pins::i2c_pins
    ( pin_id        sda_pin
    , pin_id        scl_pin
    , int           bsc_idx   
    , hertz         f
    , std::uint16_t tout
    , std::uint16_t fedl
    , std::uint16_t redl
    , hertz         fc
    )
    {
    }

    i2c_pins::~i2c_pins()
    {
      i2c_ctrl::instance().regs(bsc_idx)->clear_fifo(); // also aborts transfer
      i2c_ctrl::instance().regs(bsc_idx)->set_enable(false);
      i2c_ctrl::instance().alloc.deallocate(bsc_idx);
      gpio_ctrl::instance().alloc.deallocate(pin_id(pins[sda_idx]));
      gpio_ctrl::instance().alloc.deallocate(pin_id(pins[scl_idx]));
    }

  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
