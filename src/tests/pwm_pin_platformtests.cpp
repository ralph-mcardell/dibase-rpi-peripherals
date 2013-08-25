// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin_platformtests.cpp 
/// @brief Platform tests for pwm_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "pwm_pin.h"
//#include "pin.h"
//#include "periexcept.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Platform-tests/pwm_pin/0000/static default frequencies 100MHz"
         , "Check the default values for the PWM clock are all 100MHz"
         )
{
  CHECK(pwm_pin::clock_frequency_min().count()==100000000U);
  CHECK(pwm_pin::clock_frequency_avg().count()==100000000U);
  CHECK(pwm_pin::clock_frequency_max().count()==100000000U);
}

TEST_CASE( "Platform-tests/pwm_pin/0010/static set clock good"
         , "Check the setting the PWM clock to valid value works as expected"
         )
{
  pwm_pin::set_clock( fixed_oscillator_clock_source{f_megahertz{19.2}}
                    , clock_frequency{kilohertz{600}, clock_filter::none});
  CHECK(pwm_pin::clock_frequency_min().count()==600000U);
  CHECK(pwm_pin::clock_frequency_avg().count()==600000U);
  CHECK(pwm_pin::clock_frequency_max().count()==600000U);
}


/*
TEST_CASE( "Platform-tests/pwm_pin/1000/create good pin"
         , "Creates pwm_pin and check it is in the expected state"
         )
{
 
}
*/
