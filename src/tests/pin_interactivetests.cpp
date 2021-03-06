// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_interactivetests.cpp 
/// @brief System tests for GPIO pin IO types requiring user observation of
///        and interaction with GPIO pins.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin.h"
#include <iostream>
#include <string>
#include "interactivetests_config.h"

using namespace dibase::rpi::peripherals;

namespace
{
  pin_id const available_out_pin_id(test::out_pin_id); 
  pin_id const available_in_pin_id(test::in_pin_id);

  static bool yn_query(char const * query_text)
  {
    std::string response;
    do
      {
        std::cout << query_text << " (y/n)? ";
        std::getline(std::cin, response);
      }
    while (response.size()!=1 || response.find_first_of("yYnN")!=0);
    return response.find_first_of("Yy")==0;
  }
}
TEST_CASE( "Interactive_tests/pin/out/000/put_true_put_false"
         , "Calling opin.put with true then false should toggle the state of the available_pin"
         )
{
  std::cout << "\nPin output test:\n"
               "Connect pin BCM2835 GPIO" << available_out_pin_id 
            << " to a state monitoring device (LED, voltmeter etc.)\n\n";
  opin o{available_out_pin_id};
  std::cout << "Press <Enter> to set BCM2835 GPIO" << available_out_pin_id << " high...";
  std::string dummy;
  std::getline(std::cin, dummy);
  o.put(true);
  CHECK( yn_query("Is the GPIO pin high") );
  std::cout << "Press <Enter> to set BCM2835 GPIO" << available_out_pin_id << " low...";
  std::getline(std::cin, dummy);
  o.put(false);
  CHECK( yn_query("Is the GPIO pin low") );
}

TEST_CASE( "Interactive_tests/pin/in/100/get_true_get_false"
         , "Calling ipin.get when pin high should return true and false when low"
         )
{
  std::cout << "\nPin input test (no pullup/pulldown):\n"
               "Connect pin BCM2835 GPIO" << available_in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  ipin i{available_in_pin_id};
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " high then press <Enter>...";
  std::string dummy;
  std::getline(std::cin, dummy);
  CHECK(i.get()==true);
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " low then press <Enter>...";
  std::getline(std::cin, dummy);
  CHECK(i.get()==false);
}

TEST_CASE( "Interactive_tests/pin/in/110/get_true_get_false_pullup"
         , "Calling ipin.get with pullup enabled when pin high should return true and false when low"
         )
{
  std::cout << "\nPin input test (with pin pullup):\n"
               "Connect pin BCM2835 GPIO" << available_in_pin_id 
            << " to allow state changing between open pull up (high) and "
               "ground (switch+resistors etc.)\n\n";
  ipin i{available_in_pin_id, ipin::pull_up};
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " high then press <Enter>...";
  std::string dummy;
  std::getline(std::cin, dummy);
  CHECK(i.get()==true);
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " low then press <Enter>...";
  std::getline(std::cin, dummy);
  CHECK(i.get()==false);
}

TEST_CASE( "Interactive_tests/pin/in/120/get_true_get_false_pulldown"
         , "Calling ipin.get with pulldown enabled when pin high should return true and false when low"
         )
{
  std::cout << "\nPin input test (with pin pulldown):\n"
               "Connect pin BCM2835 GPIO" << available_in_pin_id 
            << " to allow state changing between high voltage and and open "
               " pull down (low) (switch+resistors etc.)\n\n";
  ipin i{available_in_pin_id, ipin::pull_down};
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " high then press <Enter>...";
  std::string dummy;
  std::getline(std::cin, dummy);
  CHECK(i.get()==true);
  std::cout << "Set BCM2835 GPIO" << available_in_pin_id << " low then press <Enter>...";
  std::getline(std::cin, dummy);
  CHECK(i.get()==false);
}
