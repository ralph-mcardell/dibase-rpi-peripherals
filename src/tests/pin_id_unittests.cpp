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
         , "pin_id created from valid value convertible to that value"
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

// Additional test scaffolding to allow mocking Raspberry Pi version information
#include "rpi_info.h"
#include "rpi_init.h"

using namespace dibase::rpi;

namespace dibase { namespace rpi {
namespace internal
{
  extern rpi_init * rpi_initialiser;
}
}}

struct test_pin_id_rpi_init : internal::rpi_init
{
  test_pin_id_rpi_init()
  : test_rpi_board_version(-1)
  {
  }

  virtual std::size_t init_major_version(); // override

  std::size_t test_rpi_board_version;
};

std::size_t test_pin_id_rpi_init::init_major_version()
{
  return test_rpi_board_version;
}

static test_pin_id_rpi_init test_rpi_initialiser;

struct test_rpi_version_init
{
  test_rpi_version_init()
  : original_pointer(internal::rpi_initialiser)
  {
    internal::rpi_initialiser = &test_rpi_initialiser;
  }
  ~test_rpi_version_init()
  {
    internal::rpi_initialiser = original_pointer;
  }
private:
  internal::rpi_init * const original_pointer;
};

struct test_rpi_version_mapped_pin_id : rpi_version_mapped_pin_id
{
  test_rpi_version_mapped_pin_id
  ( pin_id_int_t pin
  , pin_id_int_t const * map
  , pin_id_int_t n_pins
  , std::size_t n_versions
  )
  : rpi_version_mapped_pin_id(pin,map,n_pins,n_versions)
  {
  }
};

TEST_CASE( "Unit_tests/pin_id/rpi_version_mapped_pin_id_returns_pin_id_via_mapping"
         , "rpi_version_mapped_pin_id creates a pin_id viz: pin_id(map[N])"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  pin_id_int_t map[] = {20,21,22,23,24,24};
  std::size_t const map_size{sizeof(map)/sizeof(pin_id_int_t)};

  for ( std::size_t id=0; id!=map_size; ++id )
    {
      CHECK( (test_rpi_version_mapped_pin_id(id, map,map_size,1))==map[id] );
    }
}

TEST_CASE( "Unit_tests/pin_id/bad_mapped_pin_id_key_throws"
         , "Passing rpi_version_mapped_pin_id an invalid key throws exception"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  pin_id_int_t map[] = {20,21,22,23,24,24};
  std::size_t const map_size{sizeof(map)/sizeof(pin_id_int_t)};
  REQUIRE_THROWS_AS(test_rpi_version_mapped_pin_id(map_size,map,map_size,1), std::invalid_argument);
  REQUIRE_THROWS_AS(test_rpi_version_mapped_pin_id(-1, map,map_size,1), std::invalid_argument);
}

TEST_CASE( "Unit_tests/pin_id/bad_mapped_pin_id_value_throws"
         , "rpi_version_mapped_pin_id mapping gives bad pin id throws exception"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  pin_id_int_t map[] = {max_gpio_number+1,min_gpio_number-1};
  std::size_t const map_size{sizeof(map)/sizeof(pin_id_int_t)};
  REQUIRE_THROWS_AS(test_rpi_version_mapped_pin_id(0,map,map_size,1), std::invalid_argument);
  REQUIRE_THROWS_AS(test_rpi_version_mapped_pin_id(1,map,map_size,1), std::invalid_argument);
}

// Raspberry Pi P1 and P5 connector pin data:

// from Raspberry Pi V1 schematic...
pin_id_int_t p1_gpio_pins[] 
                        = { 3, 5, 7, 8,10,11,12,13,15,16,18,19,21,22,23,24,26 };
pin_id_int_t p1_v1_gpio_chip_ids[]
                        = { 0, 1, 4,14,15,17,18,21,22,23,24,10, 9,25,11, 8, 7 };
std::size_t const number_of_p1_gpio_pins{17};

// from Raspberry Pi V2 schematic...
pin_id_int_t p1_v2_gpio_chip_ids[]
                        = { 2, 3, 4,14,15,17,18,27,22,23,24,10, 9,25,11, 8, 7 };

// from Raspberry Pi B+ schematic...
pin_id_int_t j8_gpio_pins[] 
                        = { 3, 5, 7, 8,10,11,12,13,15,16,18,19,21,22,23,24,26 
                          ,29,31,32,33,35,36,37,38,40};
pin_id_int_t j8_v3_gpio_chip_ids[]
                        = { 2, 3, 4,14,15,17,18,27,22,23,24,10, 9,25,11, 8, 7 
                          , 5, 6,12,13,19,16,26,20,21};
std::size_t const number_of_j8_gpio_pins{26};

// from Raspberry Pi V1 schematic...
// Note: includes extra non-GPIO pin slot for phantom pin 0
// Note: P1/J8 is P1 for Models A & B revs. 1 & 2 and has only 26 pins so pins
//       27, ..., 40 do not exist and are therefore non-GPIO pins.
pin_id_int_t p1_non_gpio_pins[] = { 0, 1, 2, 4, 6, 9, 14, 17, 20, 25, 27, 28, 29
                                  , 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40
                                  };
std::size_t const number_of_p1_non_gpio_pins{24};

// from Raspberry Pi B+ schematic...
// Note: includes extra non-GPIO pin slot for phantom pin 0
pin_id_int_t j8_non_gpio_pins[] = { 0, 1, 2, 4, 6, 9, 14, 17, 20, 25
                                  , 27, 28, 30, 34, 39
                                  };
std::size_t const number_of_j8_non_gpio_pins{15};

TEST_CASE( "Unit_tests/pin_id/P1_V1_V2_J8 V3_map_size_pin_count"
         , "Sum of GPIO and non-GPIO P1/J8 pins should equal map size, pin count+1"
         )
{
  REQUIRE((sizeof(p1_gpio_pins)/sizeof(pin_id_int_t))==number_of_p1_gpio_pins);
  REQUIRE((sizeof(p1_non_gpio_pins)/sizeof(pin_id_int_t))==number_of_p1_non_gpio_pins);
  REQUIRE((sizeof(j8_gpio_pins)/sizeof(pin_id_int_t))==number_of_j8_gpio_pins);
  REQUIRE((sizeof(j8_non_gpio_pins)/sizeof(pin_id_int_t))==number_of_j8_non_gpio_pins);
  CHECK((number_of_p1_gpio_pins+number_of_p1_non_gpio_pins)==p1_map_size);
  CHECK((number_of_p1_gpio_pins+number_of_p1_non_gpio_pins-1)==p1_pin_count);
  CHECK((number_of_j8_gpio_pins+number_of_j8_non_gpio_pins)==p1_map_size);
  CHECK((number_of_j8_gpio_pins+number_of_j8_non_gpio_pins-1)==p1_pin_count);
}
// from Raspberry Pi V2 schematic...
pin_id_int_t p5_gpio_pins[]         = {  3,  4,  5,  6 };
pin_id_int_t p5_v2_gpio_chip_ids[]  = { 28, 29, 30, 31 };
std::size_t const number_of_p5_gpio_pins{4};

// from Raspberry Pi V2 schematic...
// Note: includes extra non-GPIO pin slot for phantom pin 0
pin_id_int_t p5_v2_non_gpio_pins[] = { 0, 1, 2, 7, 8 };
std::size_t const number_of_p5_v2_non_gpio_pins{5};

// Version 1 boards have no P5 support so all pins effectively non-GPIO
// Note: includes extra non-GPIO pin slot for phantom pin 0
std::size_t const number_of_p5_v1_non_gpio_pins{9};

TEST_CASE( "Unit_tests/pin_id/P5_V1_V2_map_size_pin_count"
         , "Sum of GPIO and non-GPIO P5 pins should equal map size, pin count+1"
         )
{
  REQUIRE((sizeof(p5_gpio_pins)/sizeof(pin_id_int_t))==number_of_p5_gpio_pins);
  REQUIRE((sizeof(p5_v2_non_gpio_pins)/sizeof(pin_id_int_t))==number_of_p5_v2_non_gpio_pins);
  CHECK((number_of_p5_gpio_pins+number_of_p5_v2_non_gpio_pins)==p5_map_size);
  CHECK((number_of_p5_gpio_pins+number_of_p5_v2_non_gpio_pins-1)==p5_pin_count);
  CHECK(number_of_p5_v1_non_gpio_pins==p5_map_size);
}

TEST_CASE( "Unit_tests/pin_id/rpi_version_mapped_pin_id_as_expected"
         , "p1/j8: rpi_version_mapped_pin_id has expected values for valid versions"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK( test_rpi_version_mapped_pin_id( p1_gpio_pins[pin_idx]
                                           , &(p1_gpio_pin_map[0][0])
                                           , p1_map_size
                                           , pinout_versions
                                           )==p1_v1_gpio_chip_ids[pin_idx]
            );
    }
  test_rpi_initialiser.test_rpi_board_version=2; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK( test_rpi_version_mapped_pin_id( p1_gpio_pins[pin_idx]
                                           , &(p1_gpio_pin_map[0][0])
                                           , p1_map_size
                                           , pinout_versions
                                           )==p1_v2_gpio_chip_ids[pin_idx]
            );
    }
  test_rpi_initialiser.test_rpi_board_version=3; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_j8_gpio_pins;++pin_idx)
    {
      CHECK( test_rpi_version_mapped_pin_id( j8_gpio_pins[pin_idx]
                                           , &(p1_gpio_pin_map[0][0])
                                           , p1_map_size
                                           , pinout_versions
                                           )==j8_v3_gpio_chip_ids[pin_idx]
            );
    }
}

TEST_CASE( "Unit_tests/pin_id/rpi_version_mapped_pin_id_fail_as_expected"
         , "rpi_version_mapped_pin_id fail for invalid pin id and/or version"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
// Non-GPIO connector pin number in map
  REQUIRE_THROWS_AS( test_rpi_version_mapped_pin_id( p1_non_gpio_pins[0]
                                                   , &(p1_gpio_pin_map[0][0])
                                                   , p1_map_size
                                                   , pinout_versions
                                                   )
                    , std::invalid_argument
                    );
// Bad connector pin number
  REQUIRE_THROWS_AS( test_rpi_version_mapped_pin_id( p1_map_size
                                                   , &(p1_gpio_pin_map[0][0])
                                                   , p1_map_size
                                                   , pinout_versions
                                                   )
                    , std::invalid_argument
                    );
// Bad rpi board version
  test_rpi_initialiser.test_rpi_board_version=pinout_versions+1;
  REQUIRE_THROWS_AS( test_rpi_version_mapped_pin_id( p1_non_gpio_pins[0]
                                                   , &(p1_gpio_pin_map[0][0])
                                                   , p1_map_size
                                                   , pinout_versions
                                                   )
                    , std::invalid_argument
                    );
}

TEST_CASE( "Unit_tests/pin_id/good_p1_pin_arguments_produce_expected_pin-ids"
         , "p1_pin GPIO pin numbers should produce correct mapped pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK( p1_pin(p1_gpio_pins[pin_idx])==p1_v1_gpio_chip_ids[pin_idx] );
    }
  test_rpi_initialiser.test_rpi_board_version=2; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_gpio_pins;++pin_idx)
    {
      CHECK( p1_pin(p1_gpio_pins[pin_idx])==p1_v2_gpio_chip_ids[pin_idx] );
    }
  test_rpi_initialiser.test_rpi_board_version=3; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_j8_gpio_pins;++pin_idx)
    {
      CHECK( j8_pin(j8_gpio_pins[pin_idx])==j8_v3_gpio_chip_ids[pin_idx] );
    }
}

TEST_CASE( "Unit_tests/pin_id/bad_p1_pin_arguments_throws"
         , "Bad and non-GPIO p1_pin pin numbers throws exception"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  REQUIRE_THROWS_AS(p1_pin(0),std::invalid_argument);
  REQUIRE_THROWS_AS((p1_pin(p1_map_size)), std::invalid_argument);
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_non_gpio_pins;++pin_idx)
    {
      REQUIRE_THROWS_AS((p1_pin(p1_non_gpio_pins[pin_idx])),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=2; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_p1_non_gpio_pins;++pin_idx)
    {
      REQUIRE_THROWS_AS((p1_pin(p1_non_gpio_pins[pin_idx])),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=3; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_j8_non_gpio_pins;++pin_idx)
    {
      REQUIRE_THROWS_AS((j8_pin(j8_non_gpio_pins[pin_idx])),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=pinout_versions+1;
  REQUIRE_THROWS_AS((p1_pin(p1_gpio_pins[0])),std::invalid_argument);
}

TEST_CASE( "Unit_tests/pin_id/good_p5_pin_arguments_produce_expected_pin-ids"
         , "p5_pin GPIO pin numbers produce correct mapped pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_p5_gpio_pins;++pin_idx)
    {
      CHECK( p5_pin(p5_gpio_pins[pin_idx])==p5_v2_gpio_chip_ids[pin_idx] );
    }
}

TEST_CASE( "Unit_tests/pin_id/bad_p5_pin_arguments_throws"
         , "Bad and non-GPIO p5_pin pin numbers throws exception"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  REQUIRE_THROWS_AS((p5_pin(p1_map_size)), std::invalid_argument);
  for (std::size_t pin_num=0; pin_num!=p5_map_size;++pin_num)
    {
      REQUIRE_THROWS_AS((p5_pin(pin_num)),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=3;
  for (std::size_t pin_num=0; pin_num!=p5_map_size;++pin_num)
    {
      REQUIRE_THROWS_AS((p5_pin(pin_num)),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=2; 
  for (std::size_t pin_idx=0; pin_idx!=number_of_p5_v2_non_gpio_pins;++pin_idx)
    {
      REQUIRE_THROWS_AS((p5_pin(p5_v2_non_gpio_pins[pin_idx])),std::invalid_argument);
    }
  test_rpi_initialiser.test_rpi_board_version=pinout_versions+1;
  REQUIRE_THROWS_AS((p5_pin(p5_gpio_pins[0])),std::invalid_argument);
}

TEST_CASE( "Unit_tests/pin_id/sda_correct"
         , "Static Raspberry Pi GPIO pin object sda has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(sda)==pin_id(0) );
  CHECK( pin_id(sda)==p1_pin(3) );
}

TEST_CASE( "Unit_tests/pin_id/scl_correct"
         , "Static Raspberry Pi GPIO pin object scl has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(scl)==pin_id(3) );
  CHECK( pin_id(scl)==p1_pin(5) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(scl)==pin_id(3) );
  CHECK( pin_id(scl)==p1_pin(5) );
}

TEST_CASE( "Unit_tests/pin_id/txd_correct"
         , "Static Raspberry Pi GPIO pin object txd has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(txd)==pin_id(14) );
  CHECK( pin_id(txd)==p1_pin(8) );
}

TEST_CASE( "Unit_tests/pin_id/rxd_correct"
         , "Static Raspberry Pi GPIO pin object rxd has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(rxd)==pin_id(15) );
  CHECK( pin_id(rxd)==p1_pin(10) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(rxd)==pin_id(15) );
  CHECK( pin_id(rxd)==p1_pin(10) );
}

TEST_CASE( "Unit_tests/pin_id/spi_mosi_correct"
         , "Static Raspberry Pi GPIO pin object spi_mosi has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(spi_mosi)==pin_id(10) );
  CHECK( pin_id(spi_mosi)==p1_pin(19) );
}

TEST_CASE( "Unit_tests/pin_id/spi_miso_correct"
         , "Static Raspberry Pi GPIO pin object spi_miso has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(spi_miso)==pin_id(9) );
  CHECK( pin_id(spi_miso)==p1_pin(21) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(spi_miso)==pin_id(9) );
  CHECK( pin_id(spi_miso)==p1_pin(21) );
}

TEST_CASE( "Unit_tests/pin_id/spi_sclk_correct"
         , "Static Raspberry Pi GPIO pin object spi_sclk has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(spi_sclk)==pin_id(11) );
  CHECK( pin_id(spi_sclk)==p1_pin(23) );
}

TEST_CASE( "Unit_tests/pin_id/spi_ce0_n_correct"
         , "Static Raspberry Pi GPIO pin object spi_ce0_n has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(spi_ce0_n)==pin_id(8) );
  CHECK( pin_id(spi_ce0_n)==p1_pin(24) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(spi_ce0_n)==pin_id(8) );
  CHECK( pin_id(spi_ce0_n)==p1_pin(24) );
}

TEST_CASE( "Unit_tests/pin_id/spi_ce1_n_correct"
         , "Static Raspberry Pi GPIO pin object spi_ce1_n has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(spi_ce1_n)==pin_id(7) );
  CHECK( pin_id(spi_ce1_n)==p1_pin(26) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gclk_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gclk has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gclk)==pin_id(4) );
  CHECK( pin_id(gpio_gclk)==p1_pin(7) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(gpio_gclk)==pin_id(4) );
  CHECK( pin_id(gpio_gclk)==p1_pin(7) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen0_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen0 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(gpio_gen0)==pin_id(17) );
  CHECK( pin_id(gpio_gen0)==p1_pin(11) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen1_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen1 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen1)==pin_id(18) );
  CHECK( pin_id(gpio_gen1)==p1_pin(12) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(gpio_gen1)==pin_id(18) );
  CHECK( pin_id(gpio_gen1)==p1_pin(12) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen2_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen2 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(gpio_gen2)==pin_id(21) );
  CHECK( pin_id(gpio_gen2)==p1_pin(13) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen3_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen3 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen3)==pin_id(22) );
  CHECK( pin_id(gpio_gen3)==p1_pin(15) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(gpio_gen3)==pin_id(22) );
  CHECK( pin_id(gpio_gen3)==p1_pin(15) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen4_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen4 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(gpio_gen4)==pin_id(23) );
  CHECK( pin_id(gpio_gen4)==p1_pin(16) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen5_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen5 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen5)==pin_id(24) );
  CHECK( pin_id(gpio_gen5)==p1_pin(18) );
  test_rpi_initialiser.test_rpi_board_version=3;
  CHECK( pin_id(gpio_gen5)==pin_id(24) );
  CHECK( pin_id(gpio_gen5)==p1_pin(18) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen6_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen6 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=1;
  CHECK( pin_id(gpio_gen6)==pin_id(25) );
  CHECK( pin_id(gpio_gen6)==p1_pin(22) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen7_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen7 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen7)==pin_id(28) );
  CHECK( pin_id(gpio_gen7)==p5_pin(3) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen8_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen8 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen8)==pin_id(29) );
  CHECK( pin_id(gpio_gen8)==p5_pin(4) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen9_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen9 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen9)==pin_id(30) );
  CHECK( pin_id(gpio_gen9)==p5_pin(5) );
}

TEST_CASE( "Unit_tests/pin_id/gpio_gen10_correct"
         , "Static Raspberry Pi GPIO pin object gpio_gen10 has correct pin_id value"
         )
{
  test_rpi_version_init setup;
  test_rpi_initialiser.test_rpi_board_version=2;
  CHECK( pin_id(gpio_gen10)==pin_id(31) );
  CHECK( pin_id(gpio_gen10)==p5_pin(6) );
}
