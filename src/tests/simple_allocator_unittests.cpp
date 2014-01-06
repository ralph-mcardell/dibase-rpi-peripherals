// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file simple_allocator_unittests.cpp 
/// @brief Unit tests for simple_allocator internal type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "simple_allocator.h"
#include <limits>

using namespace dibase::rpi::peripherals::internal;

namespace
{
  static_assert(std::numeric_limits<unsigned>::radix==2, "Expected unsigned int to use a binary representation");
  constexpr unsigned max_resources{std::numeric_limits<unsigned>::digits};
}

TEST_CASE( "Unit-tests/simple_allocator/0000/default construct"
         , "Default constructed simple_allocator objects possible & have "
           "nothing allocated"
         )
{
  simple_allocator<max_resources> simple_alloc;
  CHECK_FALSE(simple_alloc.any_in_use());
}

TEST_CASE( "Unit-tests/simple_allocator/0010/allocate single pin in range"
         , "Allocate a pin states just that pin in use and some pins in use "
           "nothing allocated"
         )
{
  simple_allocator<2> simple_alloc;
  CHECK(simple_alloc.allocate(0));
  CHECK(simple_alloc.any_in_use());
  CHECK(simple_alloc.is_in_use(0));
  CHECK_FALSE(simple_alloc.is_in_use(1));
}

TEST_CASE( "Unit-tests/simple_allocator/0020/re-allocate pin fails"
         , "Allocating a pin that is allocated fails and leave state as was"
         )
{
  simple_allocator<2> simple_alloc;
  CHECK(simple_alloc.allocate(1));
  CHECK_FALSE(simple_alloc.allocate(1));
  CHECK(simple_alloc.any_in_use());
  CHECK(simple_alloc.is_in_use(1));
  CHECK_FALSE(simple_alloc.is_in_use(0));
}

TEST_CASE( "Unit-tests/simple_allocator/0030/allocate out-of-range pin fails"
         , "Allocating a pin with out of range index fails leaving state as was"
         )
{
  simple_allocator<max_resources> simple_alloc;
  CHECK_FALSE(simple_alloc.allocate(max_resources));
  CHECK_FALSE(simple_alloc.any_in_use());
}

TEST_CASE( "Unit-tests/simple_allocator/0040/de-allocate pin show pin not in use"
         , "De-allocating a pin states just that pin deallocated and no "
           "other allocated pins"
         )
{
  simple_allocator<2> simple_alloc;
  REQUIRE(simple_alloc.allocate(0));
  REQUIRE(simple_alloc.allocate(1));
  CHECK(simple_alloc.deallocate(1));
  CHECK(simple_alloc.any_in_use());
  CHECK(simple_alloc.is_in_use(0));
  CHECK_FALSE(simple_alloc.is_in_use(1));
  CHECK(simple_alloc.deallocate(0));
  CHECK_FALSE(simple_alloc.any_in_use());
}

TEST_CASE( "Unit-tests/simple_allocator/0040/de-allocate unallocated pin fails"
         , "De-allocating pin that is not allocated fails leaving state as was"
         )
{
  simple_allocator<2> simple_alloc;
  REQUIRE(simple_alloc.allocate(0));
  CHECK_FALSE(simple_alloc.deallocate(1));
  CHECK(simple_alloc.any_in_use());
  CHECK(simple_alloc.is_in_use(0));
  CHECK_FALSE(simple_alloc.is_in_use(1));
  CHECK(simple_alloc.deallocate(0));
  CHECK_FALSE(simple_alloc.any_in_use());
}

TEST_CASE( "Unit-tests/simple_allocator/0050/de-allocate out of range pin fails"
         , "De-allocating pin with out of range index fails leaving state as was"
         )
{
  simple_allocator<max_resources> simple_alloc;
  REQUIRE(simple_alloc.allocate(0));
  CHECK_FALSE(simple_alloc.deallocate(max_resources));
  CHECK(simple_alloc.any_in_use());
  CHECK(simple_alloc.is_in_use(0));
  CHECK_FALSE(simple_alloc.is_in_use(1));
  CHECK(simple_alloc.deallocate(0));
  CHECK_FALSE(simple_alloc.any_in_use());
}
