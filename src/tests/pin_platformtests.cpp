// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_platformtests.cpp 
/// @brief System tests for GPIO pin IO types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;

// Change if P1 GPIO_GCLK/GPIO_GEN0 in use on your system...
pin_id available_out_pin_id{4}; // P1 pin GPIO_GCLK
pin_id available_in_pin_id{17}; // P1 pin GPIO_GEN0

TEST_CASE( "Platform_tests/000/opin/RAII construct allocates, destruct frees"
         , "An opin is allocated on construction and is freed on destruction"
         )
{
  {
    opin o{available_out_pin_id};
    REQUIRE_THROWS_AS((opin(available_out_pin_id)), bad_peripheral_alloc);
  }
  opin o{available_out_pin_id}; // should throw if pin still open
}

TEST_CASE( "Platform_tests/010/ipin/RAII construct allocates, destruct frees"
         , "An ipin is allocated on construction and is freed on destruction"
         )
{
  {
    ipin i{available_in_pin_id};
    REQUIRE_THROWS_AS((ipin(available_in_pin_id)), bad_peripheral_alloc);
  }
  ipin i{available_in_pin_id}; // should throw if pin still open
}
