// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins.cpp
/// @brief Use a set of GPIO pins for use with SPI0: implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "spi0_pins.h"
#include "gpio_alt_fn.h"
#include "gpio_ctrl.h"
#include "spi0_ctrl.h"
#include "periexcept.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    using internal::spi0_ctrl;
    using internal::gpio_ctrl;
    using internal::pin_alt_fn::gpio_special_fn;
    using internal::gpio_pin_fn;

    namespace
    {
      gpio_pin_fn get_alt_fn(pin_id pin, gpio_special_fn special_fn)
      {
        using internal::pin_alt_fn::result_set;
        using internal::pin_alt_fn::select;
        auto pin_fn_info( select(pin,special_fn) );
        if (pin_fn_info.empty())
          {
            throw std::invalid_argument
                  { "spi0_pins::spi0_pins: Pin does not support "
                    "requested SPI0 special function"
                  };
          } 
        if (pin_fn_info.size()!=1)
          {
            throw std::range_error // NO pin has >1 SPI0 function
                  {"spi0_pins::spi0_pins: Internal data error: more than one "
                   "pin alt function selected that supports the requested "
                   "SPI0 special function."
                  };
          }
        return pin_fn_info[0].alt_fn();
      }

    }

    constexpr auto ce0_idx(0U);
    constexpr auto ce1_idx(1U);
    constexpr auto sclk_idx(2U);
    constexpr auto mosi_idx(3U);
    constexpr auto miso_idx(4U);
 
    bool spi0_pins::has_conversation() const
    {
      return open_conversation!=nullptr;
    }

    bool spi0_pins::has_std_mode_support() const
    {
      return pins[miso_idx]!=spi0_pin_not_used;
    }

    spi0_pins::~spi0_pins()
    {
      unsigned idx{0U};
      while (idx!=number_of_pins && pins[idx]!=spi0_pin_not_used)
        {
          gpio_ctrl::instance().alloc.deallocate(pin_id(pins[idx]));
          ++idx;
        }
      spi0_ctrl::instance().allocated = false;
    }

    void spi0_pins::construct
    ( pin_id ce0
    , pin_id ce1
    , pin_id sclk
    , pin_id mosi
    , pin_id miso
    )
    {
      pins.fill(pin_id(spi0_pin_not_used)); 
      bool all_protocols(miso!=spi0_pin_not_used);
    // Get each pin's alt function for its SPI0 special function.
    // Note: any of these can throw - but nothing allocated yet so OK
      gpio_pin_fn alt_fn[number_of_pins];
      alt_fn[ce0_idx] = get_alt_fn(ce0, gpio_special_fn::spi0_ce0_n);
      alt_fn[ce1_idx] = get_alt_fn(ce1, gpio_special_fn::spi0_ce1_n);
      alt_fn[sclk_idx] = get_alt_fn(sclk, gpio_special_fn::spi0_sclk);
      alt_fn[mosi_idx] = get_alt_fn(mosi, gpio_special_fn::spi0_mosi);
      if (all_protocols)
        {
          alt_fn[miso_idx] = get_alt_fn(miso, gpio_special_fn::spi0_miso);
        }
    // Only one SPI0 peripheral so can check whether it is in use before
    // starting on pin allocations
      if ( spi0_ctrl::instance().allocated )
        {
          throw bad_peripheral_alloc( "spi0_pins::spi0_pins: SPI0 is "
                                      "already being used locally."
                                    );
        }

    // Speculatively allocate SPI0 peripheral
      spi0_ctrl::instance().allocated = true;

      try
      {
        gpio_ctrl::instance().alloc.allocate(ce0); // CAN THROW
        pins[ce0_idx] = ce0;
        gpio_ctrl::instance().alloc.allocate(ce1); // CAN THROW
        pins[ce1_idx] = ce1;
        gpio_ctrl::instance().alloc.allocate(sclk); // CAN THROW
        pins[sclk_idx] = sclk;
        gpio_ctrl::instance().alloc.allocate(mosi); // CAN THROW
        pins[mosi_idx] = mosi;
        if (all_protocols)
          {
            gpio_ctrl::instance().alloc.allocate(miso); // CAN THROW
            pins[miso_idx] = miso;
          }
      }
      catch (...)
      { // Oops - failed to complete resource acquisition and initialisation;
      // Release resources allocated so far and re-throw
        unsigned idx{0U};
        while (idx!=number_of_pins && pins[idx]!=spi0_pin_not_used)
          {
            gpio_ctrl::instance().alloc.deallocate(pin_id(pins[idx]));
            ++idx;
          }
        spi0_ctrl::instance().allocated = false;
        throw;
      }
      unsigned idx{0U};
      while (idx!=number_of_pins && pins[idx]!=spi0_pin_not_used)
        {
          gpio_ctrl::instance().regs->set_pin_function( pin_id(pins[idx])
                                                      , alt_fn[idx]
                                                      );
          ++idx;
        }
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
