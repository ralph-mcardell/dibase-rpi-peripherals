// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_info.cpp 
/// @brief Implementation and initialisation for rpi_info calss functions.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "rpi_info.h"
#include "rpi_init.h"

 namespace dibase {
  namespace rpi
  {
    namespace internal
    {
    // A base rpi_init object is used to initialise the data for rpi_info
    // objects for production use. Tests may subclass rpi_init, override
    // virtual initialisation methods as appropriate, and replace the
    // rpi_initialiser pointer with a pointer to an instance of their test
    // initialiser.
      rpi_init production_code_initialiser;
      rpi_init * rpi_initialiser{&production_code_initialiser};

      std::size_t rpi_init::init_major_version()
      {
      // ### TO BE DONE ###
        return 0xBADFEED; // place holder
      }
    }
    
    rpi_info::rpi_info()
    : major_version_value(internal::rpi_initialiser->init_major_version())
    {
    }
  }
 }


