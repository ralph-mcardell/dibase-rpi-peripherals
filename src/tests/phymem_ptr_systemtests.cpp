// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file phymem_ptr_systemtests.cpp 
/// @brief Tests for the physical memory samrt pointer types.
//
/// These are system tests because the rely on the system to be a Raspberry Pi
/// Linux system or something very similar - i.e. we require a system with a 
/// /dev/mem device and a similar safe area of physical memory to be mappable
/// into a process' virtual memory space.
///
/// The Raspberry Pi's BCM2835 peripheral IO physical memory area is used for
/// these system tests (should be similar to BCM2708's) as it is known to be
/// safe on a Raspberry Pi. Refer to the Broadcom BCM2835 peripheral datasheet
/// for details on the 'magic' numbers and operations (specifically the
/// introduction and GPIO chanpters)!
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///(some errata are listed at: http://elinux.org/BCM2835_datasheet_errata ).
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#define CATCH_CONFIG_MAIN // want to use catch's built-in main
#include "catch.hpp"

#include "phymem_ptr.h"
#include <sys/mman.h>

size_t const PeripheralsBlockSize(4096);
off_t  const PeripheralsBaseAddress(0x20000000);
off_t  const GpioBaseAddress( PeripheralsBaseAddress+0x200000);

typedef unsigned int volatile PeripheralAccessType;

using namespace dibase::rpi::peripherals;

TEST_CASE( "phymem_ptr", "mapping a valid block returns an accessible pointer to R/W memory only until deleted" )
{ 
  PeripheralAccessType * raw_peripheral_ptr(nullptr);
  PeripheralAccessType * null_peripheral_ptr(nullptr);
  {
    phymem_ptr<PeripheralAccessType>
      smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);
    raw_peripheral_ptr = smart_peripheral_ptr.get();
    
  // Check non-null pointer returned
    REQUIRE( raw_peripheral_ptr != null_peripheral_ptr );
    
  // Check read and write OK at this location (valid GPIO pin setup operations):
    PeripheralAccessType original_value( *raw_peripheral_ptr );
    // Clear bottom 3 bits => GPIO0 in input mode
    PeripheralAccessType first_updated_value( original_value&(~7) );
    *raw_peripheral_ptr = first_updated_value;
    CHECK( *raw_peripheral_ptr == first_updated_value );
    // Set bottom 3 bits to 001 => GPIO0 in output mode
    PeripheralAccessType second_updated_value( first_updated_value|1 );
    *raw_peripheral_ptr = second_updated_value;
    CHECK( *raw_peripheral_ptr == second_updated_value );
    // Restore original value
    *raw_peripheral_ptr = original_value;
    CHECK( *raw_peripheral_ptr == original_value );
  }
// Check to ensure mapped region has been unmapped on destruction:
//    Attempting to mlock non-mapped region should fail: 
//        mlock returns -1 and sets errno to ENOMEM.
  REQUIRE( mlock((const void*)raw_peripheral_ptr, PeripheralsBlockSize) == -1 );
  REQUIRE( errno == ENOMEM );
}
