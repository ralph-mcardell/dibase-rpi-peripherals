// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file interactivetests_config.h 
/// @brief Shared configuration for interactive tests
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_TEST_INTERACTIVETEST_CONFIG_H
# define DIBASE_RPI_PERIPHERALS_TEST_INTERACTIVETEST_CONFIG_H
# include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { 
    namespace test 
    {
    // Change if P1 GPIO_GCLK/GPIO_GEN0 in use on your system...
      pin_id const out_pin_id(gpio_gen3); // Gertboard GP22 (GPIO22)
      pin_id const in_pin_id(gpio_gen2);  // Gertboard GP21 (GPIO21 or 27)
    }
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_TEST_INTERACTIVETEST_CONFIG_H
