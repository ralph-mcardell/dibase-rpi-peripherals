// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

// ##### Ongoing version. Will be updated when more facilities avaialble #####

#include "phymem_ptr.h"
#include "gpio_registers.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <unistd.h>     // for sleep

using namespace dibase::rpi::peripherals;

int main()
{
  try
    {
      phymem_ptr<volatile gpio_registers> 
                pgpio(gpio_registers::physical_address, register_block_size);
      
      pgpio->set_pin_function(gpio_gclk, gpio_pin_fn::output);

      for (unsigned p = 0; p < 10; ++p) 
        {
          pgpio->set_pin(gpio_gclk);
          sleep(1);
          pgpio->clear_pin(gpio_gclk);
          sleep(1);
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}
