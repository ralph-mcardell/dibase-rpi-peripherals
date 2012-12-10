// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc_unittests.cpp 
/// @brief Unit tests for pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_alloc.h"

using namespace dibase::rpi::peripherals;

struct mock_allocator
{
  void allocate( pin_id /*pin*/ )
  {
    if (in_use) throw std::bad_alloc();
    in_use = true;
  }
  void deallocate( pin_id /*pin*/ )
  {
    if (!in_use) throw std::logic_error("Cannot deallocate pin that is not in use!");
    in_use = false;
  }
  bool is_in_use( pin_id /*pin*/ )
  {
    return in_use;
  }
  static bool in_use;
};
bool mock_allocator::in_use{false};

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_available_pin_marks_as_in_use"
         , "After allocating an available pin with pin_cache_allocator it is in use"
         )
{
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(1));
  CHECK(a.is_in_use(pin_id(1))==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_available_pin_checks_wrapped_allocator"
         , "When allocating an available pin pin_cache_allocator passes request to wrapped allocator"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(2));
  CHECK(mock_allocator::in_use==true);
}
