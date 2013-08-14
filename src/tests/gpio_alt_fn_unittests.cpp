// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_alt_fn_unittests.cpp 
/// @brief Unit tests for gpio_alt_fn support.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "gpio_alt_fn.h"
#include <algorithm>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::pin_alt_fn;

TEST_CASE( "Unit-tests/pin_alt_fn_descriptor/0000/construct valid arguments"
         , ""
         )
{
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2};
  CHECK(pafd.pin()==4U);
  CHECK(pafd.alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafd.special_fn()==gpio_special_fn::gpclk2);
}

TEST_CASE( "Unit-tests/descriptor/0010/construct invalid arguments"
         , ""
         )
{
  CHECK_THROWS_AS( descriptor
                    (pin_id(4), gpio_pin_fn::input, gpio_special_fn::gpclk2)
                 , std::invalid_argument
                 );
  CHECK_THROWS_AS( descriptor
                    (pin_id(4), gpio_pin_fn::output, gpio_special_fn::gpclk2)
                 , std::invalid_argument
                 );
}

TEST_CASE( "Unit-tests/descriptor/0020/copy and assignemnt"
         , ""
         )
{
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2};
  descriptor pafd_copy(pafd);
  CHECK(pafd_copy.pin()==4U);
  CHECK(pafd_copy.alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafd_copy.special_fn()==gpio_special_fn::gpclk2);
  descriptor
    pafd_assign{pin_id{6}, gpio_pin_fn::alt3, gpio_special_fn::no_fn};
  REQUIRE(pafd_assign.pin()==6U);
  REQUIRE(pafd_assign.alt_fn()==gpio_pin_fn::alt3);
  REQUIRE(pafd_assign.special_fn()==gpio_special_fn::no_fn);
  pafd_assign = pafd;
  CHECK(pafd_assign.pin()==4U);
  CHECK(pafd_assign.alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafd_assign.special_fn()==gpio_special_fn::gpclk2);
}

TEST_CASE( "Unit-tests/result_set_builder/0000/default construct empty"
         , ""
         )
{
  result_set_builder pafrsb;
  CHECK(pafrsb.size()==0U);
  CHECK(pafrsb.empty());
}

TEST_CASE( "Unit-tests/result_set_builder/0010/add item"
         , "Add one descriptor"
         )
{
  result_set_builder pafrsb;
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2};
  pafrsb.add(pafd);
  CHECK(pafrsb.size()==1U);
  CHECK_FALSE(pafrsb.empty());
}

TEST_CASE( "Unit-tests/result_set_builder/0020/emplace add item"
         , "Add one descriptor constructing it in place"
         )
{
  result_set_builder pafrsb;
  pafrsb.emplace_add(pin_id{4}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2);
  CHECK(pafrsb.size()==1U);
  CHECK_FALSE(pafrsb.empty());
}

TEST_CASE( "Unit-tests/result_set_builder/0030/drain items"
         , "Drain added items into vector leaving an empty builder"
         )
{
  result_set_builder pafrsb;
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt4, gpio_special_fn::gpclk1};
  pafrsb.add(pafd);
  pafrsb.emplace_add(pin_id{5}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2);
  REQUIRE(pafrsb.size()==2U);
  REQUIRE_FALSE(pafrsb.empty());
  std::vector<descriptor> items{pafrsb.drain()};
  CHECK(pafrsb.size()==0U);
  CHECK(pafrsb.empty());
  REQUIRE(items.size()==2U);
  CHECK_FALSE(items.empty());
  CHECK(items.back().pin()==5U);
  CHECK(items.back().alt_fn()==gpio_pin_fn::alt5);
  CHECK(items.back().special_fn()==gpio_special_fn::gpclk2);
  items.pop_back();
  CHECK(items.back().pin()==4U);
  CHECK(items.back().alt_fn()==gpio_pin_fn::alt4);
  CHECK(items.back().special_fn()==gpio_special_fn::gpclk1);
}

TEST_CASE( "Unit-tests/result_set/0000/construct from empty builder"
         , "Constructing from an empty builder yields an empty result set"
         )
{
  result_set_builder pafrsb;
  REQUIRE(pafrsb.empty());
  result_set pafrs{pafrsb};
  CHECK(pafrs.size()==0U);
  CHECK(pafrs.empty());
}

TEST_CASE( "Unit-tests/result_set/0010/construct from 2 item builder"
         , "Constructing from an non-empty builder yields a similarly "
           "non-empty result set and an empty builder"
         )
{
  result_set_builder pafrsb;
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt4, gpio_special_fn::gpclk1};
  pafrsb.add(pafd);
  pafrsb.emplace_add(pin_id{5}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2);
  REQUIRE(pafrsb.size()==2U);
  REQUIRE_FALSE(pafrsb.empty());
  result_set pafrs{pafrsb};
  CHECK(pafrsb.size()==0U);
  CHECK(pafrsb.empty());
  REQUIRE(pafrs.size()==2U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[1].pin()==5U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::gpclk2);
  CHECK(pafrs.at(0).pin()==4U);
  CHECK(pafrs.at(0).alt_fn()==gpio_pin_fn::alt4);
  CHECK(pafrs.at(0).special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs.size()==2U); // access did not change size
  CHECK_THROWS_AS(pafrs.at(pafrs.size()),std::out_of_range);
}

TEST_CASE( "Unit-tests/result_set/0020/begin / end iterator support"
         , "Using begin and end for const_iterator access to results"
         )
{
  result_set_builder pafrsb;
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt4, gpio_special_fn::gpclk1};
  pafrsb.add(pafd);
  pafrsb.emplace_add(pin_id{5}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2);
  REQUIRE(pafrsb.size()==2U);
  REQUIRE_FALSE(pafrsb.empty());
  result_set pafrs{pafrsb};
  CHECK(pafrs.size()==2U);
  unsigned expected_pin_id_value{4};
  for (auto const & result : pafrs)
    {
      CHECK( result.pin()==expected_pin_id_value);
      ++expected_pin_id_value;
    }
  CHECK(pafrs.size()==2U); // access did not change size
}

TEST_CASE( "Unit-tests/result_set/0020/cbegin/cend iterator support"
         , "Using cbegin and cend for const_iterator access to results"
         )
{
  result_set_builder pafrsb;
  descriptor pafd{pin_id{4}, gpio_pin_fn::alt4, gpio_special_fn::gpclk1};
  pafrsb.add(pafd);
  pafrsb.emplace_add(pin_id{5}, gpio_pin_fn::alt5, gpio_special_fn::gpclk2);
  REQUIRE(pafrsb.size()==2U);
  REQUIRE_FALSE(pafrsb.empty());
  result_set pafrs{pafrsb};
  CHECK(pafrs.size()==2U);
  unsigned expected_pin_id_value{4};
  std::for_each ( pafrs.cbegin(), pafrs.cend(),
                  [&](descriptor const & result)
                  {
                    CHECK( result.pin()==expected_pin_id_value);
                    ++expected_pin_id_value;
                  }
                );
  CHECK(pafrs.size()==2U); // access did not change size
}


// Values in BCM2835 ARM Peripheral datasheet PDF table 6-31 columns
// ALT0 to ALT6 that are _not_ blank, <reserved> or <Internal>
// The values were calcutated by scanning each ALTn row of the table
// and subtracting the count of each block of cells which were blank, 
// <reserved> or <Internal> - noting that all pins numbered above 45 have no
// (published) special functions.
constexpr unsigned cnt_pins_alt0_specl_fn{46-2-6-2-1};
constexpr unsigned cnt_pins_alt1_specl_fn{46-2};
constexpr unsigned cnt_pins_alt2_specl_fn{46-28-4-4};
constexpr unsigned cnt_pins_alt3_specl_fn{46-16-2-12};
constexpr unsigned cnt_pins_alt4_specl_fn{46-16-12};
constexpr unsigned cnt_pins_alt5_specl_fn{46-4-5-8-6-2};
constexpr unsigned cnt_pins_alt_fn_with_specl_fns
                    { cnt_pins_alt0_specl_fn
                    + cnt_pins_alt1_specl_fn
                    + cnt_pins_alt2_specl_fn
                    + cnt_pins_alt3_specl_fn
                    + cnt_pins_alt4_specl_fn
                    + cnt_pins_alt5_specl_fn
                    };
TEST_CASE( "Unit-tests/select/0000/select all less no_fn"
         , "Calling select with no parameters the returned "
           "result_set should have result items for all "
           "pins alt functions that have a specific special function"
         )
{
  auto pafrs = result_set(select());
  CHECK(pafrs.size()==cnt_pins_alt_fn_with_specl_fns);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==0U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[pafrs.size()-1].pin()==45U);
  CHECK(pafrs[pafrs.size()-1].alt_fn()==gpio_pin_fn::alt4);
  for (auto const & d : pafrs)
    {
      CHECK(d.special_fn()!=gpio_special_fn::no_fn);
    }
}

TEST_CASE( "Unit-tests/select/0010/select all with no_fn"
         , "Calling select just specifying include_no_fn the "
           "returned result_set should have result items for all "
           "pins' alt function 'slots'"
         )
{
  auto pafrs = result_set(select(select_options::include_no_fn) );
  constexpr unsigned expected_size{54U*6U}; // 54 GPIO pins with 6 ALT functions
  CHECK(pafrs.size()==expected_size);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==0U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[pafrs.size()-1].pin()==53U);
  CHECK(pafrs[pafrs.size()-1].alt_fn()==gpio_pin_fn::alt5);
}

TEST_CASE( "Unit-tests/select/0020/1 pin: select not no_fn"
         , "Calling select with a single pin_id the returned "
           "result_set should have result items for pin's alt "
           "functions that have a specific special function"
         )
{
  auto pafrs = result_set(select(pin_id{18}));
  REQUIRE(pafrs.size()==5U);
  CHECK_FALSE(pafrs.empty());
// GPIO pin 18 ALT special functions read from datasheet table 6-31
  CHECK(pafrs[0].pin()==18U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::pcm_clk);
  CHECK(pafrs[1].pin()==18U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::sd10);
  CHECK(pafrs[2].pin()==18U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt3);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::bscsl_sda_mosi);
  CHECK(pafrs[3].pin()==18U);
  CHECK(pafrs[3].alt_fn()==gpio_pin_fn::alt4);
  CHECK(pafrs[3].special_fn()==gpio_special_fn::spi1_ce0_n);
  CHECK(pafrs[4].pin()==18U);
  CHECK(pafrs[4].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[4].special_fn()==gpio_special_fn::pwm0);
}


TEST_CASE( "Unit-tests/select/0030/1 pin: select with no_fn"
         , "Calling select with a single pin_id the returned "
           "result_set should have result items for pin's alt "
           "functions."
         )
{
  auto pafrs = result_set( select(pin_id{18},select_options::include_no_fn) );
  REQUIRE(pafrs.size()==6U);
  CHECK_FALSE(pafrs.empty());
// GPIO pin 18 ALT special functions read from datasheet table 6-31
  CHECK(pafrs[0].pin()==18U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::pcm_clk);
  CHECK(pafrs[1].pin()==18U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::sd10);
  CHECK(pafrs[2].pin()==18U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt2);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::no_fn);
  CHECK(pafrs[3].pin()==18U);
  CHECK(pafrs[3].alt_fn()==gpio_pin_fn::alt3);
  CHECK(pafrs[3].special_fn()==gpio_special_fn::bscsl_sda_mosi);
  CHECK(pafrs[4].pin()==18U);
  CHECK(pafrs[4].alt_fn()==gpio_pin_fn::alt4);
  CHECK(pafrs[4].special_fn()==gpio_special_fn::spi1_ce0_n);
  CHECK(pafrs[5].pin()==18U);
  CHECK(pafrs[5].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[5].special_fn()==gpio_special_fn::pwm0);
}

TEST_CASE( "Unit-tests/select/0040/select specific special fn"
         , "Calling select with a single special function the "
           "returned result_set should have result items for all "
           "pin-alt functions having that function."
         )
{
  auto pafrs=result_set(select(gpio_special_fn::gpclk0));
// GPCLK0 function instances read from datasheet table 6-31
  REQUIRE(pafrs.size()==4U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==4U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk0);
  CHECK(pafrs[1].pin()==20U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::gpclk0);
  CHECK(pafrs[2].pin()==32U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::gpclk0);
  CHECK(pafrs[3].pin()==34U);
  CHECK(pafrs[3].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[3].special_fn()==gpio_special_fn::gpclk0);
}

TEST_CASE( "Unit-tests/select/0050/select specific special fn for specific pin"
         , "Calling select with a single pin & a special "
           "function the returned result_set should have either an "
           "empty result or result with a single item"
         )
{
// Pin 11 has no GPCLK0 function....
  auto e_pafrs = result_set( select(pin_id{1}, gpio_special_fn::gpclk0) );
  CHECK(e_pafrs.size()==0U);
  CHECK(e_pafrs.empty());

// Pin 20 does have a GPCLK0 function...  
  auto pafrs = result_set( select(pin_id{20}, gpio_special_fn::gpclk0) );
  REQUIRE(pafrs.size()==1U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==20U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk0);
}

TEST_CASE( "Unit-tests/select/0060/pin list: select not no_fn"
         , "Calling select with a pin_id list the returned "
           "a result_set should have result items for all listed "
           "pins' alt functions that have a specific special function"
         )
{
  auto pafrs = result_set(select({pin_id{1}, pin_id{0}}));
  REQUIRE(pafrs.size()==4U);
  CHECK_FALSE(pafrs.empty());
// GPIO pin 18 ALT special functions read from datasheet table 6-31
  CHECK(pafrs[0].pin()==1U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::scl0);
  CHECK(pafrs[1].pin()==1U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::sa4);
  CHECK(pafrs[2].pin()==0U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::sda0);
  CHECK(pafrs[3].pin()==0U);
  CHECK(pafrs[3].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[3].special_fn()==gpio_special_fn::sa5);
}

TEST_CASE( "Unit-tests/select/0070/pin list: select with no_fn"
         , "Calling select with a pin_id list the returned"
           "a result_set should have result items for all listed "
           "pins' alt functions"
         )
{
  auto pafrs = result_set( select( {pin_id{1}, pin_id{0}}
                                 , select_options::include_no_fn
                                 )
                         );
  REQUIRE(pafrs.size()==12U);
  CHECK_FALSE(pafrs.empty());
// GPIO pin 18 ALT special functions read from datasheet table 6-31
  CHECK(pafrs[0].pin()==1U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::scl0);
  CHECK(pafrs[1].pin()==1U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::sa4);
  CHECK(pafrs[6].pin()==0U);
  CHECK(pafrs[6].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[6].special_fn()==gpio_special_fn::sda0);
  CHECK(pafrs[7].pin()==0U);
  CHECK(pafrs[7].alt_fn()==gpio_pin_fn::alt1);
  CHECK(pafrs[7].special_fn()==gpio_special_fn::sa5);
}

TEST_CASE( "Unit-tests/select/0080/select some specific special fns"
         , "Calling select with a special function list the "
           "returned result_set should have result items for all "
           "pin-alt functions having any of those functions."
         )
{
  auto pafrs=result_set( select( { gpio_special_fn::gpclk1
                                 , gpio_special_fn::gpclk2
                                 }
                               )
                       );
// GPCLK1/2 function instances read from datasheet table 6-31
  REQUIRE(pafrs.size()==6U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==5U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs[1].pin()==6U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::gpclk2);
  CHECK(pafrs[2].pin()==21U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs[3].pin()==42U);
  CHECK(pafrs[3].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[3].special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs[4].pin()==43U);
  CHECK(pafrs[4].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[4].special_fn()==gpio_special_fn::gpclk2);
  CHECK(pafrs[5].pin()==44U);
  CHECK(pafrs[5].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[5].special_fn()==gpio_special_fn::gpclk1);
}

TEST_CASE( "Unit-tests/select/0090/select some specific special fns for some specific pins"
         , "returned  with lists for pins & special functions the returned "
           "result_set should have result items for any listed "
           "pin's alt functions having any of the listed special functions."
         )
{
  auto pafrs=result_set(select( { pin_id{4}, pin_id{5}, pin_id{6}
                                , pin_id{20}, pin_id{21}
                                }
                              , { gpio_special_fn::gpclk1
                                , gpio_special_fn::gpclk2 
                                }
                              )
                       );
// GPCLK1/2 function instances read from datasheet table 6-31
  REQUIRE(pafrs.size()==3U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==5U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs[1].pin()==6U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::gpclk2);
  CHECK(pafrs[2].pin()==21U);
  CHECK(pafrs[2].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[2].special_fn()==gpio_special_fn::gpclk1);
}

TEST_CASE( "Unit-tests/select/0100/select some specific special fns for a specific pin"
         , "returned  with pin & list of special functions the returned "
           "result_set should have result items for any of the "
           "pin's alt functions having any of the listed special functions."
         )
{
  auto pafrs=result_set( select( pin_id{5}
                               , { gpio_special_fn::gpclk1
                                 , gpio_special_fn::gpclk2 
                                 }
                               )
                       );
// GPCLK1/2 function instances read from datasheet table 6-31
  REQUIRE(pafrs.size()==1U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==5U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk1);
}

TEST_CASE( "Unit-tests/select/0110/select some specific special fn for some specific pins"
         , "functions & constructing a with list for pins & a special function "
           "the returned result_set should have result items "
           "for any listed pin's alt functions having the special function."
         )
{
  auto pafrs=result_set( select( { pin_id{4}, pin_id{5}, pin_id{6}
                                 , pin_id{20}, pin_id{21}
                                 }
                               , gpio_special_fn::gpclk1                 
                               )
                       );
// GPCLK1 function instances read from datasheet table 6-31
  REQUIRE(pafrs.size()==2U);
  CHECK_FALSE(pafrs.empty());
  CHECK(pafrs[0].pin()==5U);
  CHECK(pafrs[0].alt_fn()==gpio_pin_fn::alt0);
  CHECK(pafrs[0].special_fn()==gpio_special_fn::gpclk1);
  CHECK(pafrs[1].pin()==21U);
  CHECK(pafrs[1].alt_fn()==gpio_pin_fn::alt5);
  CHECK(pafrs[1].special_fn()==gpio_special_fn::gpclk1);
}
