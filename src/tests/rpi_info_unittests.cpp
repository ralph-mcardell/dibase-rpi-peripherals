// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_info_unittests.cpp
/// @brief Unit tests for Raspberry Pi information class.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "rpi_info.h"
#include "rpi_init.h"

using namespace dibase::rpi;

namespace dibase { namespace rpi {
namespace internal
{
  extern rpi_init * rpi_initialiser;
}
}}

std::size_t const test_rpi_board_version{3};

struct test_rpi_init : internal::rpi_init
{
  virtual std::size_t init_major_version(); // override
};

std::size_t test_rpi_init::init_major_version()
{
  return test_rpi_board_version;
}

static test_rpi_init test_rpi_initialiser;

struct test_init
{
  test_init()
  : original_pointer(internal::rpi_initialiser)
  {
    internal::rpi_initialiser = &test_rpi_initialiser;
  }
  ~test_init()
  {
    internal::rpi_initialiser = original_pointer;
  }
private:
  internal::rpi_init * const original_pointer;
};

TEST_CASE( "Unit_tests/rpi_info/major_version"
         , "rpi_info().major_version() should return a 1 based positive integer"
         )
{
  test_init setup;
  REQUIRE(rpi_info().major_version()==test_rpi_board_version);
}

TEST_CASE( "Unit_tests/rpi_info/index_version"
         , "rpi_info().index_version() should return a 0 based positive integer, one less than major_version"
         )
{
  test_init setup;
  REQUIRE(rpi_info().index_version()==(test_rpi_board_version-1));
}
