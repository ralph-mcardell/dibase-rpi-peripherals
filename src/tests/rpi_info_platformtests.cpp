// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_info_platformtests.cpp
/// @brief System tests for Raspberry Pi information class.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "rpi_info.h"
#include <iostream>

using namespace dibase::rpi;

TEST_CASE( "Platform_tests/rpi_info/major_version"
         , "rpi_info().major_version() should return a board version of 1 or 2"
         )
{
  REQUIRE( ((rpi_info().major_version()==1)||(rpi_info().major_version()==2)) );
  std::cout << "\nINFORMATION: Raspberry Pi major revision determined to be: "
            << rpi_info().major_version() << '\n';
}

TEST_CASE( "Platform_tests/rpi_info/index_version"
         , "rpi_info().index_version() should return a board version index of 0 or 1"
         )
{
  REQUIRE( ((rpi_info().index_version()==0)||(rpi_info().index_version()==1)) );
}
