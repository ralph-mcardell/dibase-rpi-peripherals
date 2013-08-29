// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc_platformtests.cpp 
/// @brief System tests for pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_alloc.h"

using namespace dibase::rpi::peripherals::internal;
using namespace dibase::rpi::peripherals;

// Change if P1 GPIO_GEN0 is in use on your system...
pin_id const available_pin_id{17}; // P1 pin GPIO_GEN0

TEST_CASE( "Platform_tests/000/pin_export_allocator/is_in_use_initially_reports_free"
         , "The available_pin_id is initially not exported and assumed to be free"
         )
{
  pin_export_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Platform_tests/001/pin_export_allocator/alloc_pin_is_in_use_unalloc_is_free"
         , "Allocate available_pin_id is in use, deallocate it is free"
         )
{
  pin_export_allocator a;
  a.allocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==true);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Platform_tests/002/pin_export_allocator/alloc_in_use_pin_throws"
         , "Allocate available_pin_id twice should throw on 2nd allocation."
         )
{
  pin_export_allocator a;
  a.allocate(available_pin_id);
  REQUIRE(a.is_in_use(available_pin_id)==true);
  REQUIRE_THROWS_AS(a.allocate(available_pin_id), bad_peripheral_alloc);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Platform_tests/003/pin_export_allocator/dealloc_free_pin_throws"
         , "Deallocate available_pin_id when free should throw."
         )
{
  pin_export_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
  REQUIRE_THROWS_AS(a.deallocate(available_pin_id), std::runtime_error);
}


TEST_CASE( "Platform_tests/100/pin_allocator/is_in_use_initially_reports_free"
         , "The available_pin_id is initially not exported and assumed to be free"
         )
{
  pin_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Platform_tests/101/pin_allocator/alloc_pin_is_in_use_unalloc_is_free"
         , "Allocate available_pin_id is in use, deallocate it is free"
         )
{
  pin_allocator a;
  a.allocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==true);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Platform_tests/102/pin_allocator/alloc_in_use_pin_throws"
         , "Allocate available_pin_id twice should throw on 2nd allocation."
         )
{
  pin_allocator a;
  a.allocate(available_pin_id);
  REQUIRE(a.is_in_use(available_pin_id)==true);
  REQUIRE_THROWS_AS(a.allocate(available_pin_id), bad_peripheral_alloc);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Unit_tests/103/pin_allocator/dealloc_free_pin_throws"
         , "Deallocate available_pin_id when free should throw."
         )
{
  pin_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
  REQUIRE_THROWS_AS(a.deallocate(available_pin_id), std::logic_error);
}
