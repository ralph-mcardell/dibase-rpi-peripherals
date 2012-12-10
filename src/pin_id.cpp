// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_id.cpp
/// @brief GPIO pin id numbers implementation.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin_id.h"
#include "rpi_info.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace
    {
      pin_id_int_t const noio{-1}; // indicates no GPIO on connector pin
    }
  // Note: connector pins start at 1 so pin 0 noio!
    pin_id_int_t p1_gpio_pin_map[pinout_versions][p1_map_size] =
      { {noio,noio,noio,0,noio,1,noio,4,14,noio,15,17,18,21,noio,22,23,noio,24,10,noio,9,25,11,8,noio,7}
      , {noio,noio,noio,2,noio,3,noio,4,14,noio,15,17,18,27,noio,22,23,noio,24,10,noio,9,25,11,8,noio,7}
      };
    pin_id_int_t p5_gpio_pin_map[pinout_versions][p5_map_size] =
      { {noio,noio,noio,noio,noio,noio,noio,noio,noio}
      , {noio,noio,noio,  28,  29,  30,  31,noio,noio}
      };

    static pin_id_int_t do_lookup
    ( std::size_t pin
    , std::size_t version
    , pin_id_int_t const * map
    , std::size_t n_pins
    , std::size_t n_versions
    )
    {
      return version>=n_versions || pin>=n_pins
              ? -1 // return invalid pin id for bad map index values
              : map[version*n_pins+pin]
              ;
    }

    rpi_version_mapped_pin_id::rpi_version_mapped_pin_id
    ( pin_id_int_t pin
    , pin_id_int_t const * map
    , std::size_t n_pins
    , std::size_t n_versions
    )
    : pin_id(do_lookup(pin, rpi_info().index_version(), map, n_pins, n_versions))
    {}
  }
}}
