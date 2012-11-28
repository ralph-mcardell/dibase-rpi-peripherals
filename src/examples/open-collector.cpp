// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//
// Basic GNU compiler options building executable called ocol:
// g++ -std=c++0x -Wall -Wextra -pedantic -I.. -L../../lib -o ocol open-collector.cpp -lrpi-periphals 

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
      
      pin_id gpio4{4};
      pgpio->set_pin_function(gpio4, gpio_pin_fn::output);

      for (unsigned p = 0; p < 10; ++p) 
        {
          pgpio->set_pin(gpio4);
          sleep(1);
          pgpio->clear_pin(gpio4);
          sleep(1);
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}
