// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins_platformtests.cpp 
/// @brief Platform tests for spi0_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "spi0_pins.h"
#include "spi0_ctrl.h"
#include "gpio_ctrl.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

TEST_CASE( "Platform-tests/spi0_pins/0000/create & destroy with good full pin sets"
         , "Creating spi0_pins from a set of full 3-wire SPI0 pin set is in "
           "the expected state, destroys and checks pin & SPI0 peripheral free"
         )
{
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
    CHECK(spi0_ctrl::instance().allocated);
    CHECK(sp.has_std_mode_support());
    CHECK_FALSE(sp.has_conversation());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  CHECK_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0010/create & destroy with good 2 wire pin sets"
         , "Creating spi0_pins from a set of 2-wire only SPI0 pin set is in "
           "the expected state, destroys and checks pin & SPI0 peripheral free"
         )
{
  {
    spi0_pins sp(rpi_p1_spi0_2_wire_only_pin_set);
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
    CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
    CHECK(spi0_ctrl::instance().allocated);
    CHECK_FALSE(sp.has_std_mode_support());
    CHECK_FALSE(sp.has_conversation());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  CHECK_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0020/create bad: no expected alt-fn"
         , "Creating spi0_pins from an SPI0 pin set with a pin that does not "
           "support the stated special function throws an exception"
         )
{
  spi0_pin_set<0U,7U,11U,10U,9U> full_bad_ce0;
  spi0_pin_set<8U,0U,11U,10U,9U> full_bad_ce1;
  spi0_pin_set<8U,7U,0U,10U,9U> full_bad_sclk;
  spi0_pin_set<8U,7U,11U,0U,9U> full_bad_mosi;
  spi0_pin_set<8U,7U,11U,10,0U> full_bad_miso;
  REQUIRE_THROWS_AS((spi0_pins(full_bad_ce0 )), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  REQUIRE_THROWS_AS((spi0_pins(full_bad_ce1 )), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_sclk)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_mosi)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_miso)), std::invalid_argument);

  spi0_pin_set<0U,7U,11U,10U> two_wire_bad_ce0;
  spi0_pin_set<8U,0U,11U,10U> two_wire_bad_ce1;
  spi0_pin_set<8U,7U,0U,10U>  two_wire_bad_sclk;
  spi0_pin_set<8U,7U,11U,0U>  two_wire_bad_mosi;
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_ce0 )), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_ce1 )), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_sclk)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_mosi)), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
}

TEST_CASE( "Platform-tests/spi0_pins/0030/create bad: SPI0 in use"
         , "Creating spi0_pins from a good SPI0 pin set when the SPI0 "
           "peripheral is marked as in use throws an exception"
         )
{
  spi0_ctrl::instance().allocated = true;
  REQUIRE(spi0_ctrl::instance().allocated);

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  spi0_ctrl::instance().allocated = false;
  REQUIRE_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0040/create bad: pins in use"
         , "Creating spi0_pins from a good SPI0 pin set when a pin in the set "
           "is marked as in use throws an exception"
         )
{
  REQUIRE_FALSE(spi0_ctrl::instance().allocated);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));


  gpio_ctrl::instance().alloc.allocate(spi_ce0_n);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_ce0_n);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));

  gpio_ctrl::instance().alloc.allocate(spi_ce1_n);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_ce1_n);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));

  gpio_ctrl::instance().alloc.allocate(spi_sclk);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_sclk);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));

  gpio_ctrl::instance().alloc.allocate(spi_mosi);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_mosi);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));

  gpio_ctrl::instance().alloc.allocate(spi_miso);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_miso);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
}


TEST_CASE( "Platform-tests/spi0_conversation/0000/create & destroy good"
         , "Creating spi0_conversation from a good set of parameters leaves "
           "object in the expected state"
         )
{
  spi0_conversation sc(spi0_slave::chip1, megahertz(1));
}