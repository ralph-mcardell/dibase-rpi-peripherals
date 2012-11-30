// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_id.cpp
/// @brief GPIO pin id numbers implementation.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    pin_id_int_t const noio{-1}; // indicates no gpio on connector pin
  // Note: connector pins start at 1 so pin 0 noio!
    pin_id_int_t p1_gpio_pin_map[p1_versions][p1_map_size] =
      { {noio,noio,noio,0,noio,1,noio,4,14,noio,15,17,18,21,noio,22,23,noio,24,10,noio,9,25,11,8,noio,7}
      , {noio,noio,noio,2,noio,3,noio,4,14,noio,15,17,18,27,noio,22,23,noio,24,10,noio,9,25,11,8,noio,7}
      };
  }
}}