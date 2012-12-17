// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_systemtests.cpp 
/// @brief System tests for GPIO pin IO types.
///
/// Some tests require observation of and interaction with GPIO pins
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin.h"
#include <iostream>
#include <string>
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
  ipin o{available_out_pin_id};
  CHECK(o.is_open()==true);
  REQUIRE_THROWS_AS(o.open(available_out_pin_id), bad_pin_alloc);
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


TEST_CASE( "System_tests/100/opin/put_true_put_false"
         , "Calling opin.put with true then false should toggle the state of the available_pin"
         )
{
  std::cout << "\nPin output test:\n"
               "Connect pin BCM2835 GPIO" << available_out_pin_id 
            << " to a state monitoring device (LED, voltmeter etc.)\n\n";
  opin o{available_out_pin_id};
  CHECK(o.is_open()==true);
  std::cout << "Press <Enter> to set BCM2835 GPIO" << available_out_pin_id << " high...";
  std::string dummy;
  std::getline(std::cin, dummy);
  o.put(true);
  std::cout << "Press <Enter> to set BCM2835 GPIO" << available_out_pin_id << " low...";
  std::getline(std::cin, dummy);
  o.put(false);
}


TEST_CASE( "System_tests/200/ipin/get_true_get_false"
         , "Calling ipin.get when pin high should return true and false when low"
         )
{
  std::cout << "\nPin input test (no pullup):\n"
               "Connect pin BCM2835 GPIO" << available_in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  ipin i{available_in_pin_id};
  CHECK(i.is_open()==true);
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " high then press <Enter>...";
  std::string dummy;
  std::getline(std::cin, dummy);
  CHECK(i.get()==true);
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " low then press <Enter>...";
  std::getline(std::cin, dummy);
  CHECK(i.get()==false);
}

