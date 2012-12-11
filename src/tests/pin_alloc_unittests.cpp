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
    if (in_use) throw std::domain_error("Oops - unexpected call to mock_allocator::allocate");
    in_use = true;
  }
  void deallocate( pin_id /*pin*/ )
  {
    if (!in_use) throw std::domain_error("Oops - unexpected call to mock_allocator::deallocate");
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

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_unavailable_pin_throws"
         , "When allocating an unavailable pin pin_cache_allocator throws a bad_pin_alloc exception"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(3));
  REQUIRE(a.is_in_use(pin_id(3))==true);
  REQUIRE_THROWS_AS(a.allocate(pin_id(3)), bad_pin_alloc);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_used_elsewhere_pin_throws_not_locally_in_use"
         , "When allocating an pin used elsewhere pin_cache_allocator does not mark pin as in use locally"
         )
{
  mock_allocator::in_use = true;
  pin_cache_allocator<mock_allocator> a;
  REQUIRE_THROWS_AS(a.allocate(pin_id(4)), std::domain_error);
  REQUIRE(mock_allocator::in_use==true);
//2nd alloc. request only passed to mock_allocator if pin still marked free by a
  REQUIRE_THROWS_AS(a.allocate(pin_id(4)), std::domain_error);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_pins_independent"
         , "Allocating one pin does not interfere with allocating another"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(5));
  REQUIRE(mock_allocator::in_use==true);
// 2nd pin alloc. request only passed to mock_allocator if pin marked free by a
  REQUIRE_THROWS_AS(a.allocate(pin_id(6)), std::domain_error);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_min_pin_id_OK"
         , "Allocating minimum supported pin_id works OK"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(pin_id::min_id));
  CHECK(a.is_in_use(pin_id(pin_id::min_id))==true);
  CHECK(mock_allocator::in_use==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_max_pin_id_OK"
         , "Allocating maximum supported pin_id works OK"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(pin_id::max_id));
  CHECK(a.is_in_use(pin_id(pin_id::max_id))==true);
  CHECK(mock_allocator::in_use==true);
}
