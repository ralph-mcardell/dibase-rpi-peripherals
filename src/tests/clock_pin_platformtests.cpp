// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin_platformtests.cpp 
/// @brief Platform tests for clock_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_pin.h"
#include "pin.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Platform-tests/clock_pin/0000/create good"
         , "Creates clock_pin in expected state when passed valid parameters"
         )
{
  clock_pin clk { pin_id{4}
                , fixed_oscillator_clock_source{f_megahertz{19.2}}
                , clock_frequency{kilohertz{600}, clock_filter::none}
                };
  CHECK(clk.frequency_min()==hertz{600000U});
  CHECK(clk.frequency_avg()==hertz{600000U});
  CHECK(clk.frequency_max()==hertz{600000U});
  CHECK_FALSE(clk.is_running());
}

TEST_CASE( "Platform-tests/clock_pin/0010/destruction releases resources"
         , "After destruction can open clock pin on same pin..."
         )
{
  {
    clock_pin clk { pin_id{4}
                  , fixed_oscillator_clock_source{f_megahertz{19.2}}
                  , clock_frequency{kilohertz{600}, clock_filter::none}
                  };
    CHECK(clk.frequency_min()==hertz{600000U});
    CHECK(clk.frequency_avg()==hertz{600000U});
    CHECK(clk.frequency_max()==hertz{600000U});
    CHECK_FALSE(clk.is_running());
  }

  clock_pin clk { pin_id{4}
                , fixed_oscillator_clock_source{f_megahertz{19.2}}
                , clock_frequency{kilohertz{600}, clock_filter::none}
                };
  CHECK(clk.frequency_min()==hertz{600000U});
  CHECK(clk.frequency_avg()==hertz{600000U});
  CHECK(clk.frequency_max()==hertz{600000U});
  CHECK_FALSE(clk.is_running());
}

TEST_CASE( "Platform-tests/clock_pin/0020/create bad: pin has no clock function"
         , "Exception thrown creating clock_pin for pin with no clock function"
         )
{
  CHECK_THROWS_AS(clock_pin( pin_id{3}
                           , fixed_oscillator_clock_source(f_megahertz(19.2))
                           , clock_frequency(kilohertz(600), clock_filter::none)
                           )
                 , std::invalid_argument
                 );
}

TEST_CASE( "Platform-tests/clock_pin/0030/create bad: pin in use"
         , "Exception thrown creating clock_pin for pin already used elsewhere"
         )
{
  ipin pin4{pin_id{4}};
  CHECK_THROWS_AS(clock_pin( pin_id{4}
                           , fixed_oscillator_clock_source(f_megahertz(19.2))
                           , clock_frequency(kilohertz(600), clock_filter::none)
                           )
                 , bad_peripheral_alloc
                 );
}

TEST_CASE( "Platform-tests/clock_pin/0040/create bad: clock in use"
         , "Exception thrown creating clock_pin for clock already in use"
         )
{
  clock_pin clk { pin_id{4} // Supports GPCLK0
                , fixed_oscillator_clock_source{f_megahertz{19.2}}
                , clock_frequency{kilohertz{600}, clock_filter::none}
                };
  CHECK_THROWS_AS(clock_pin( pin_id{20} // Another pin supporting GLCLK0
                           , fixed_oscillator_clock_source(f_megahertz(19.2))
                           , clock_frequency(kilohertz(600), clock_filter::none)
                           )
                 , bad_peripheral_alloc
                 );
}

TEST_CASE( "Platform-tests/clock_pin/0050/create bad: invalid frequencies"
         , "Exception thrown creating clock_pin for bad clock frequencies"
         )
{
  CHECK_THROWS_AS(clock_pin( pin_id{5}
                           , fixed_oscillator_clock_source(f_megahertz(19.2))
                           , clock_frequency(kilohertz(1), clock_filter::none)
                           )
                 , std::invalid_argument
                 );
  CHECK_THROWS_AS(clock_pin( pin_id{5}
                           , fixed_oscillator_clock_source(megahertz(24))
                           , clock_frequency(megahertz(16), clock_filter::minimum)
                           )
                 , std::range_error
                 );
}
