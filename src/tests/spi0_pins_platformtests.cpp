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
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  CHECK_FALSE(spi0_ctrl::instance().allocated);
}
