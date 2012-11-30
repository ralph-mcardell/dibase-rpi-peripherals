// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_id_unittests.cpp 
/// @brief Unit tests for pin id validating wrapper types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_id.h"

using namespace dibase::rpi::peripherals;

pin_id_int_t const min_gpio_number(0);
pin_id_int_t const max_gpio_number(53);

TEST_CASE( "Unit_tests/pin_id/create_valid_converts_to_integer"
         , "pin_id created from valid value convertable to that value"
         )
{
  for (pin_id_int_t id=min_gpio_number; id<=max_gpio_number; ++id)
    {
      pin_id valid_id{id};
      CHECK( valid_id==id );
    }
  pin_id id_from_literal(23);
  CHECK( id_from_literal==23u );
  CHECK ( pin_id(44)==44u );
}

TEST_CASE( "Unit_tests/pin_id/create_invalid_throws_exception"
         , "Constructing invalid pin_id instances raises an exception"
         )
{
  REQUIRE_THROWS_AS( pin_id(max_gpio_number+1), std::invalid_argument );
  REQUIRE_THROWS_AS( pin_id(min_gpio_number-1), std::invalid_argument );
}

TEST_CASE( "Unit_tests/pin_id/copy_construct"
         , "pin_id instances can be value copied"
         )
{
  pin_id id{12};
  CHECK( id==pin_id{id} );
}

TEST_CASE( "Unit_tests/pin_id/pin_id_in_exprn_with_volatile_data"
         , "A pin_id has to be able work in expressions with volatile data"
         )
{
  pin_id_int_t volatile volatile_value(1);
  CHECK( pin_id(1)==volatile_value );
  CHECK( (volatile_value = pin_id(1))==1 );
  CHECK( (volatile_value |= 1U<<(pin_id(1)%32))==3 );
}

TEST_CASE( "Unit_tests/pin_id/mapped_pin_id_returns_pin_id_via_mapping"
         , "mapped_pin_id creates a pin_id viz: pin_id(map[N])"
         )
{
  pin_id_int_t map[] = {20,21,22,23,24,24};
  std::size_t map_size{sizeof(map)/sizeof(pin_id_int_t)};
  for ( std::size_t id=0; id!=map_size; ++id )
    {
      CHECK( mapped_pin_id(id, map, map_size)==map[id] );
    }
}

TEST_CASE( "Unit_tests/pin_id/bad_mapped_pin_id_key_throws"
         , "Passing mapped_pin_id an invalid key throws std::invalid_argument"
         )
{
  pin_id_int_t map[] = {20,21,22,23,24,24};
  std::size_t map_size{sizeof(map)/sizeof(pin_id_int_t)};
  REQUIRE_THROWS_AS(mapped_pin_id(map_size, map, map_size), std::invalid_argument);
  REQUIRE_THROWS_AS(mapped_pin_id(-1, map, map_size), std::invalid_argument);
}

TEST_CASE( "Unit_tests/pin_id/bad_mapped_pin_id_value_throws"
         , "mapped_pin_id mapping results in bad pin id throws std::invalid_argument"
         )
{
  pin_id_int_t map[] = {max_gpio_number+1,min_gpio_number-1};
  std::size_t map_size{sizeof(map)/sizeof(pin_id_int_t)};
  REQUIRE_THROWS_AS(mapped_pin_id(0, map, map_size), std::invalid_argument);
  REQUIRE_THROWS_AS(mapped_pin_id(1, map, map_size), std::invalid_argument);
}

// from Raspberry Pi V1 schematic...
pin_id_int_t p1_gpio_pins[] 
                        = { 3, 5, 7, 8,10,11,12,13,15,16,18,19,21,22,23,24,26 };
pin_id_int_t p1_v1_gpio_chip_ids[]
                        = { 0, 1, 4,14,15,17,18,21,22,23,24,10, 9,25,11, 8, 7 };
std::size_t const number_of_p1_gpio_pins{17};

TEST_CASE( "Unit_tests/pin_id/P1_V1_pins_map_as_expected"
         , "P1 version 1 connector pins map to expected pin_id values"
         )
{
  REQUIRE((sizeof(p1_gpio_pins)/sizeof(pin_id_int_t))==number_of_p1_gpio_pins);
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK(mapped_pin_id(p1_gpio_pins[pin_idx], p1_gpio_pin_map[0], p1_map_size)==p1_v1_gpio_chip_ids[pin_idx]);
    }
}

// from Raspberry Pi V2 schematic...
pin_id_int_t p1_v2_gpio_chip_ids[]
                        = { 2, 3, 4,14,15,17,18,27,22,23,24,10, 9,25,11, 8, 7 };

TEST_CASE( "Unit_tests/pin_id/P1_V2_pins_map_as_expected"
         , "P1 version 2 connector pins map to expected pin_id values"
         )
{
  REQUIRE((sizeof(p1_gpio_pins)/sizeof(pin_id_int_t))==number_of_p1_gpio_pins);
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK(mapped_pin_id(p1_gpio_pins[pin_idx], p1_gpio_pin_map[1], p1_map_size)==p1_v2_gpio_chip_ids[pin_idx]);
    }
}

// from Raspberry Pi V1 schematic...
pin_id_int_t p1_non_gpio_pins[] = { 0, 1, 2, 4, 6, 9, 14, 17, 20, 25 };
std::size_t const number_of_p1_non_gpio_pins{10};
TEST_CASE( "Unit_tests/pin_id/P1_V1_V2_non_gpio_pins_throw"
         , "Non-GPIO P1 version 1 & 2 connector pins throw std::invalid_argument"
         )
{
  REQUIRE((sizeof(p1_non_gpio_pins)/sizeof(pin_id_int_t))==number_of_p1_non_gpio_pins);
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_non_gpio_pins;++pin_idx)
    {
      REQUIRE_THROWS_AS(mapped_pin_id(p1_non_gpio_pins[pin_idx], p1_gpio_pin_map[0], p1_map_size), std::invalid_argument);
      REQUIRE_THROWS_AS(mapped_pin_id(p1_non_gpio_pins[pin_idx], p1_gpio_pin_map[1], p1_map_size), std::invalid_argument);
    }
}
