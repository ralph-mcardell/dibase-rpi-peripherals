// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_systemtests.cpp 
/// @brief System tests for GPIO pin IO types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin.h"
#include "pin_alloc.h"

using namespace dibase::rpi::peripherals;

// Change if P1 GPIO_GCLK/GPIO_GEN0 in use on your system...
pin_id available_out_pin_id{4}; // P1 pin GPIO_GCLK
pin_id available_in_pin_id{17}; // P1 pin GPIO_GEN0

TEST_CASE( "System_tests/000/opin/close_closed_pin_benign"
         , "An opin may be closed even if it is not open"
         )
{
  opin o;
  CHECK(o.is_open()==false);
  o.close();
}

TEST_CASE( "System_tests/001/opin/open_close"
         , "An opin may be default constructed then opened and closed explicitly"
         )
{
  opin o;
  CHECK(o.is_open()==false);
  o.open(available_out_pin_id);
  CHECK(o.is_open()==true);
  o.close();
  CHECK(o.is_open()==false);
}

TEST_CASE( "System_tests/002/opin/RAII_implicit_open_close"
         , "An opin may be opened on contruction and is closed on destruction"
         )
{
  {
    opin o{available_out_pin_id};
    CHECK(o.is_open()==true);
  }
  opin o{available_out_pin_id}; // should throw if pin still open
  REQUIRE(o.is_open()==true);
  o.close();
}

TEST_CASE( "System_tests/003/opin/open_same_pin_twice_throws"
         , "Opening the same pin more than once should fail by throwing"
         )
{
  opin o{available_out_pin_id};
  CHECK(o.is_open()==true);
  REQUIRE_THROWS_AS(o.open(available_out_pin_id), bad_pin_alloc);
}

TEST_CASE( "System_tests/004/opin/put_to_closed_pin_ignored"
         , "Attempting to change a opin state when closed is ignored"
         )
{
  opin o;
  CHECK(o.is_open()==false);
  o.put(true);
}


TEST_CASE( "System_tests/020/ipin/close_closed_pin_benign"
         , "An opin may be closed even if it is not open"
         )
{
  ipin i;
  CHECK(i.is_open()==false);
  i.close();
}

TEST_CASE( "System_tests/021/ipin/open_close"
         , "An opin may be default constructed then opened and closed explicitly"
         )
{
  ipin i;
  CHECK(i.is_open()==false);
  i.open(available_in_pin_id);
  CHECK(i.is_open()==true);
  i.close();
  CHECK(i.is_open()==false);
}

TEST_CASE( "System_tests/022/ipin/RAII_implicit_open_close"
         , "An opin may be opened on contruction and is closed on destruction"
         )
{
  {
    ipin i{available_in_pin_id};
    CHECK(i.is_open()==true);
  }
  ipin i{available_in_pin_id}; // should throw if pin still open
  REQUIRE(i.is_open()==true);
  i.close();
}

TEST_CASE( "System_tests/023/ipin/open_same_pin_twice_throws"
         , "Opening the same pin more than once should fail by throwing"
         )
{
  ipin i{available_in_pin_id};
  CHECK(i.is_open()==true);
  REQUIRE_THROWS_AS(i.open(available_in_pin_id), bad_pin_alloc);
}

TEST_CASE( "System_tests/004/ipin/get_from_closed_pin_false"
         , "Getting a closed ipin's state returns false"
         )
{
  ipin i;
  CHECK(i.is_open()==false);
  CHECK(i.get()==false);
}
