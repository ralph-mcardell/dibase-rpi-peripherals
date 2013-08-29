// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc_unittests.cpp 
/// @brief Unit tests for pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_alloc.h"

using namespace dibase::rpi::peripherals::internal;
using namespace dibase::rpi::peripherals;

struct mock_allocator
{
  void allocate( pin_id /*pin*/ )
  {
    if (in_use) throw std::domain_error
                        ("Oops - unexpected call to mock_allocator::allocate");
    in_use = true;
  }
  void deallocate( pin_id /*pin*/ )
  {
    if (!in_use) throw std::domain_error
                        ("Oops - unexpected call to mock_allocator::deallocate");
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
         , "Available pin in use after allocating with pin_cache_allocator"
         )
{
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(1));
  CHECK(a.is_in_use(pin_id(1))==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_available_pin_checks_wrapped_allocator"
         , "Allocating available pin request passed to wrapped allocator"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(2));
  CHECK(mock_allocator::in_use==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_unavailable_pin_throws"
         , "Allocating unavailable pin pin_cache_allocator throws exception"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(3));
  REQUIRE(a.is_in_use(pin_id(3))==true);
  REQUIRE_THROWS_AS(a.allocate(pin_id(3)), bad_peripheral_alloc);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_used_elsewhere_pin_throws_not_locally_in_use"
         , "Allocating pin used elsewhere throws exception and pin not marked "
           "as in use locally"
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
         , "Able to allocate minimum supported pin_id"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(pin_id::min_id));
  CHECK(a.is_in_use(pin_id(pin_id::min_id))==true);
  CHECK(mock_allocator::in_use==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/alloc_max_pin_id_OK"
         , "Able to allocate maximum supported pin_id"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(pin_id::max_id));
  CHECK(a.is_in_use(pin_id(pin_id::max_id))==true);
  CHECK(mock_allocator::in_use==true);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/dealloc_unused_pin_throws"
         , "Deallocating a pin not in use throws std::logic_error"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  REQUIRE_THROWS_AS(a.deallocate(pin_id(7)), std::logic_error);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/dealloc_locally_in_use_pin_succeeds"
         , "OK to deallocate pin locally in use - pin marked as free"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(8));
  REQUIRE(a.is_in_use(pin_id(8))==true);
  a.deallocate(pin_id(8));
  CHECK(a.is_in_use(pin_id(8))==false);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/dealloc_in_use_pin_does_pass_on_request"
         , "Deallocating pin locally in use pass on deallocation request"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(9));
  REQUIRE(mock_allocator::in_use==true);
  a.deallocate(pin_id(9));
  CHECK(mock_allocator::in_use==false);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/dealloc_pins_independent"
         , "Deallocating one pin does not interfere with deallocating another"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;
  a.allocate(pin_id(10));
  REQUIRE(mock_allocator::in_use==true);
  mock_allocator::in_use = false;
  a.allocate(pin_id(11));
  REQUIRE(mock_allocator::in_use==true);
  
  a.deallocate(pin_id(10));
  REQUIRE(mock_allocator::in_use==false);
// 2nd pin dealloc. request only passed to mock_allocator if pin in use by a
  REQUIRE_THROWS_AS(a.deallocate(pin_id(11)), std::domain_error);
}

TEST_CASE( "Unit_tests/pin_cache_allocator/all_pins_initially_free"
         , "After construction of a pin_cache_allocator all pins should be free"
         )
{
  mock_allocator::in_use = false;
  pin_cache_allocator<mock_allocator> a;

  for ( pin_id_int_t id=0; id!=pin_id::number_of_pins; ++id )
    {
      REQUIRE( a.is_in_use(pin_id(id))==false );
    }
}

TEST_CASE( "Unit_tests/pin_cache_allocator/is_in_use_only_passes_on_request_if_locally_free"
         , "is_in_use will report cached in use value or contained allocator's "
           "is_in_use value if locally free"
         )
{
  pin_cache_allocator<mock_allocator> a;
  mock_allocator::in_use = true;
  CHECK(a.is_in_use(pin_id(12))==true);
  mock_allocator::in_use = false;
  CHECK(a.is_in_use(pin_id(12))==false);
  a.allocate(pin_id(12));
  REQUIRE(mock_allocator::in_use==true);
  CHECK(a.is_in_use(pin_id(12))==true);
  mock_allocator::in_use = false;
  CHECK(a.is_in_use(pin_id(12))==true);
  mock_allocator::in_use = true;
  a.deallocate(pin_id(12));
  REQUIRE(mock_allocator::in_use==false);
  CHECK(a.is_in_use(pin_id(12))==false);
  mock_allocator::in_use = true;
  CHECK(a.is_in_use(pin_id(12))==true);
}
