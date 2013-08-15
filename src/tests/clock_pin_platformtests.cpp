// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin_platformtests.cpp 
/// @brief Platform tests for clock_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_pin.h"
#include "pin.h"
#include "pinexcept.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Platform-tests/clock_pin/0000/create good"
         , "Creates clock_pin in expected state when passed valid paramters"
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
         , ""
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
         , ""
         )
{
  ipin pin4{pin_id{4}};
  CHECK_THROWS_AS(clock_pin( pin_id{4}
                           , fixed_oscillator_clock_source(f_megahertz(19.2))
                           , clock_frequency(kilohertz(600), clock_filter::none)
                           )
                 , bad_pin_alloc
                 );
}

TEST_CASE( "Platform-tests/clock_pin/0040/create bad: clock in use"
         , ""
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
                 , bad_pin_alloc
                 );
}
