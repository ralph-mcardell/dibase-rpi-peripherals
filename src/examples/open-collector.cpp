// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

// ##### Ongoing version. Will be updated when more facilities avaialble #####

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <unistd.h>     // for sleep

using namespace dibase::rpi::peripherals;

int main()
{
  try
    {
      opin gpio_clk_out(gpio_gclk);

      for (unsigned p = 0; p < 10; ++p) 
        {
          gpio_clk_out.put(true);
          sleep(1);
          gpio_clk_out.put(false);
          sleep(1);
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}
