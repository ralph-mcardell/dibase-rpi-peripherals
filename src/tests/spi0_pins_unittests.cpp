// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins_unittests.cpp 
/// @brief Unit tests for spi0_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "spi0_pins.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Platform-tests/spi0_pin_set/0000/pins in set as expected"
         , "Defined spi_pin_set returns expected (pin) values"
         )
{
  constexpr pin_id_int_t expected_ce0{1U};
  constexpr pin_id_int_t expected_ce1{2U};
  constexpr pin_id_int_t expected_sclk{3U};
  constexpr pin_id_int_t expected_mosi{4U};
  constexpr pin_id_int_t expected_miso{5U};
  spi0_pin_set<expected_ce0, expected_ce1
              , expected_sclk, expected_mosi, expected_miso> full_pin_set;
  CHECK(full_pin_set.ce0()==expected_ce0);
  CHECK(full_pin_set.ce1()==expected_ce1);
  CHECK(full_pin_set.sclk()==expected_sclk);
  CHECK(full_pin_set.mosi()==expected_mosi);
  CHECK(full_pin_set.miso()==expected_miso);
  spi0_pin_set<expected_ce0, expected_ce1
              , expected_sclk, expected_mosi> two_wire_pin_set;
  CHECK(two_wire_pin_set.ce0()==expected_ce0);
  CHECK(two_wire_pin_set.ce1()==expected_ce1);
  CHECK(two_wire_pin_set.sclk()==expected_sclk);
  CHECK(two_wire_pin_set.mosi()==expected_mosi);
  CHECK(two_wire_pin_set.miso()==spi0_pin_not_used);
}

TEST_CASE( "Platform-tests/rpi_p1_spi0_full_pin_set/0000/pins in set as expected"
         , "rpi_p1_spi0_full_pin_set instances return expected (pin) values"
         )
{
  CHECK(rpi_p1_spi0_full_pin_set.ce0()==pin_id(spi_ce0_n));
  CHECK(rpi_p1_spi0_full_pin_set.ce1()==pin_id(spi_ce1_n));
  CHECK(rpi_p1_spi0_full_pin_set.sclk()==pin_id(spi_sclk));
  CHECK(rpi_p1_spi0_full_pin_set.mosi()==pin_id(spi_mosi));
  CHECK(rpi_p1_spi0_full_pin_set.miso()==pin_id(spi_miso));
}

TEST_CASE( "Platform-tests/rpi_p1_spi0_2_wire_only_pin_set/0000/pins in set as expected"
         , "rpi_p1_spi0_2_wire_only_pin_set instances return expected (pin) values"
         )
{
  CHECK(rpi_p1_spi0_2_wire_only_pin_set.ce0()==pin_id(spi_ce0_n));
  CHECK(rpi_p1_spi0_2_wire_only_pin_set.ce1()==pin_id(spi_ce1_n));
  CHECK(rpi_p1_spi0_2_wire_only_pin_set.sclk()==pin_id(spi_sclk));
  CHECK(rpi_p1_spi0_2_wire_only_pin_set.mosi()==pin_id(spi_mosi));
  CHECK(rpi_p1_spi0_2_wire_only_pin_set.miso()==spi0_pin_not_used);
}
