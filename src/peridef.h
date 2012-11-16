// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file peridef.h 
/// @brief Common BCM2835 peripheral declarations and definitions.
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PERIDEF_H
 #define DIBASE_RPI_PERIPHERALS_PERIDEF_H

 #include <sys/types.h>
 #include <cstdint>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    typedef off_t    physical_address_t;
    typedef std::uint32_t register_t;

    physical_address_t const peripheral_base_address(0x20000000);
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_PERIDEF_H
