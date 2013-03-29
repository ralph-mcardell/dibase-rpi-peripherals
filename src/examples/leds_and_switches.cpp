// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/leds_and_switches.cpp
/// @brief GPIO oupput and input - assumed to be connected to LEDs and switches
//
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell
//

// ##### W.I.P. #####

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <unistd.h>     // for sleep

using namespace dibase::rpi::peripherals;

int main()
{
  try
    {
      opin led0(gpio_gen6);
      opin led1(gpio_gen5);
      opin led2(gpio_gen4);
      opin led3(gpio_gen3);

      ipin switch0(gpio_gen2);

      for (unsigned p = 0; p < 17; ++p) 
        {
          led0.put((p&1));
          led1.put((p&2));
          led2.put((p&4));
          led3.put((p&8));
          std::cout << "Switch0 state: " << switch0.get() << std::endl;
          sleep(1);
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}
