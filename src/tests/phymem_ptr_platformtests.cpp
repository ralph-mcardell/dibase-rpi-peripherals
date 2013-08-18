// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file phymem_ptr_platformtests.cpp 
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

#include "catch.hpp"

#include "phymem_ptr.h"
#include <sys/mman.h>

size_t const PeripheralsBlockSize(4096);
off_t  const PeripheralsBaseAddress(0x20000000);
off_t  const GpioBaseAddress( PeripheralsBaseAddress+0x200000);

typedef unsigned int volatile PeripheralAccessType;

using namespace dibase::rpi::peripherals;

TEST_CASE( "Platform_tests/phymem_ptr/lifetime-and-access"
         , "Successful creation should allow R/W access only until deleted" 
         )
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

TEST_CASE( "Platform_tests/phymem_ptr/dereference"
         , "Access to block via operator*() same as via *p.get()"
         )
{
  PeripheralAccessType * null_peripheral_ptr(nullptr);

  phymem_ptr<PeripheralAccessType>
    smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_peripheral_ptr.get() != null_peripheral_ptr );
  REQUIRE( *smart_peripheral_ptr == *(smart_peripheral_ptr.get()) );
}

// Wrap some of the initial GPIO control registers in a type
// that has members so we can test member access:
struct gpio_function_select
{
  unsigned int gpfsel0;
  unsigned int gpfsel1;
  unsigned int gpfsel2;
  unsigned int gpfsel3;
  unsigned int gpfsel4;
  unsigned int gpfsel5;
};

typedef gpio_function_select volatile GPIOFunctionSelectType;

TEST_CASE( "Platform_tests/phymem_ptr/first_member_access"
         , "Access to 1st member in block via operator->() same as via get()" 
         )
{
  GPIOFunctionSelectType * null_gpiofnsel_ptr(nullptr);

  phymem_ptr<GPIOFunctionSelectType>
    smart_gpiofsel_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_gpiofsel_ptr.get() != null_gpiofnsel_ptr );
  REQUIRE( &smart_gpiofsel_ptr->gpfsel0 == (unsigned*)smart_gpiofsel_ptr.get() );
}

TEST_CASE( "Platform_tests/phymem_ptr/indexed-get-zero-index"
         , "Access to block via p.get(0) same as via p.get()"
         )
{
  PeripheralAccessType * null_peripheral_ptr(nullptr);

  phymem_ptr<PeripheralAccessType>
    smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_peripheral_ptr.get() != null_peripheral_ptr );
  REQUIRE( smart_peripheral_ptr.get(0) == smart_peripheral_ptr.get() );
}

TEST_CASE( "Platform_tests/phymem_ptr/subscript-zero"
         , "Access to block via operator[](0) same as via *p.get()"
         )
{
  PeripheralAccessType * null_peripheral_ptr(nullptr);

  phymem_ptr<PeripheralAccessType>
    smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_peripheral_ptr.get() != null_peripheral_ptr );
  REQUIRE( smart_peripheral_ptr[0] == *(smart_peripheral_ptr.get()) );
}

TEST_CASE( "Platform_tests/phymem_ptr/sixth_member_access"
         , "Using operator->() to access to non-1st member same as get()+offset"
         )
{
  GPIOFunctionSelectType * null_gpiofnsel_ptr(nullptr);

  phymem_ptr<GPIOFunctionSelectType>
    smart_gpiofsel_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_gpiofsel_ptr.get() != null_gpiofnsel_ptr );
  REQUIRE(&smart_gpiofsel_ptr->gpfsel5==((unsigned*)smart_gpiofsel_ptr.get())+5);
}

TEST_CASE( "Platform_tests/phymem_ptr/indexed-get-index-1"
         , "Access to block via p.get(1) same as via p.get()+1"
         )
{
  PeripheralAccessType * null_peripheral_ptr(nullptr);

  phymem_ptr<PeripheralAccessType>
    smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_peripheral_ptr.get() != null_peripheral_ptr );
  REQUIRE( smart_peripheral_ptr.get(1) == (smart_peripheral_ptr.get()+1) );
}

TEST_CASE( "Platform_tests/phymem_ptr/subscript-1"
         , "Access to block via operator[](1) same as via *(p.get()+1)"
         )
{
  PeripheralAccessType * null_peripheral_ptr(nullptr);

  phymem_ptr<PeripheralAccessType>
    smart_peripheral_ptr(GpioBaseAddress, PeripheralsBlockSize);

  REQUIRE( smart_peripheral_ptr.get() != null_peripheral_ptr );
  REQUIRE( smart_peripheral_ptr[1] == *(smart_peripheral_ptr.get()+1) );
}

