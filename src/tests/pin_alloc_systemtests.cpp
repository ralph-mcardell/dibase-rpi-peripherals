// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc_systemtests.cpp 
/// @brief System tests for pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_alloc.h"

using namespace dibase::rpi::peripherals;

// Change if P1 GPIO_GEN0 is in use on your system...
pin_id const available_pin_id{17}; // P1 pin GPIO_GEN0

TEST_CASE( "Unit_tests/pin_export_allocator/0is_in_use_initially_reports_free"
         , "The available_pin_id is initially not exported and assumed to be free"
         )
{
  pin_export_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Unit_tests/pin_export_allocator/1alloc_pin_is_in_use_unalloc_is_free"
         , "Allocate available_pin_id is in use, deallocate it is free"
         )
{
  pin_export_allocator a;
  a.allocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==true);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Unit_tests/pin_export_allocator/2alloc_in_use_pin_throws"
         , "Allocate available_pin_id twice should throw on 2nd allocation."
         )
{
  pin_export_allocator a;
  a.allocate(available_pin_id);
  REQUIRE(a.is_in_use(available_pin_id)==true);
  REQUIRE_THROWS_AS(a.allocate(available_pin_id), bad_pin_alloc);
  a.deallocate(available_pin_id);
  CHECK(a.is_in_use(available_pin_id)==false);
}

TEST_CASE( "Unit_tests/pin_export_allocator/3dealloc_free_pin_throws"
         , "Deallocate available_pin_id when free should throw."
         )
{
  pin_export_allocator a;
  REQUIRE(a.is_in_use(available_pin_id)==false);
  REQUIRE_THROWS_AS(a.deallocate(available_pin_id), std::runtime_error);
}
