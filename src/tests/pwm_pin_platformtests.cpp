// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin_platformtests.cpp 
/// @brief Platform tests for pwm_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "pwm_pin.h"
#include "pwm_ctrl.h"
#include "pin.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

TEST_CASE( "Platform-tests/pwm_pin/0000/create & destroy good pins"
         , "Creates pwm_pin for each channel, check they are in expected "
           "state, destroys and checks pin & PWM channel free"
         )
{
  auto range_default(pwm_pin::range_default);
  {
    pwm_pin p{pin_id{18}}; // GPIO18, PWM0, ALT5
    REQUIRE_THROWS_AS(ipin(pin_id(18)), bad_peripheral_alloc); // expect pin in use
    CHECK(pwm_ctrl::instance().alloc.is_in_use(0));      // PWM0 allocated
    CHECK((pwm_ctrl::instance().regs->control&0xff)==0U);// All PWM0 ctrl bits 0
    CHECK(pwm_ctrl::instance().regs->range1==range_default); // range should be default
    CHECK(pwm_ctrl::instance().regs->data1==0U);// data should be zero (100% low)
  }
  ipin(pin_id(18)); // pin 18 should be free for other uses
  CHECK_FALSE(pwm_ctrl::instance().alloc.is_in_use(0)); // PWM0 should be free
  {
    pwm_pin p{pin_id{19}}; // GPIO19, PWM1, ALT5
    REQUIRE_THROWS_AS(ipin(pin_id(19)), bad_peripheral_alloc); // expect pin in use
    CHECK(pwm_ctrl::instance().alloc.is_in_use(1));      // PWM1 allocated
    CHECK((pwm_ctrl::instance().regs->control&0xbf00)==0U);// All PWM1 ctrl bits 0
    CHECK(pwm_ctrl::instance().regs->range2==range_default); // range should be default
    CHECK(pwm_ctrl::instance().regs->data2==0U);// data should be zero (100% low)
  }
  ipin(pin_id(19)); // pin 18 should be free for other uses
  CHECK_FALSE(pwm_ctrl::instance().alloc.is_in_use(1)); // PWM1 should be free
}

TEST_CASE( "Platform-tests/pwm_pin/0020/create good, explicit range"
         , "Creating pwm_pin with explicit range value sets range register OK"
         )
{
  unsigned expected_range{1234U};
  pwm_pin p{pin_id{18}, expected_range}; // GPIO18, PWM0, ALT5
  CHECK(pwm_ctrl::instance().regs->range1==expected_range);
}

TEST_CASE( "Platform-tests/pwm_pin/0030/create bad: explicit range too small"
         , "Creating pwm_pin with very small explicit range value fails"
         )
{
  unsigned bad_range{pwm_pin::range_minimum-1};
  REQUIRE_THROWS_AS(pwm_pin(pin_id(18), bad_range), std::out_of_range);
  ipin(pin_id(18)); // pin should not be in use
}

TEST_CASE( "Platform-tests/pwm_pin/0040/create bad: pin has no PWM function"
         , "Creating pwm_pin for a pin with no PWM special function fails"
         )
{
  REQUIRE_THROWS_AS(pwm_pin(pin_id(8)), std::invalid_argument);
  ipin(pin_id(8)); // pin should not be in use
}

TEST_CASE( "Platform-tests/pwm_pin/0050/create bad: pin in use"
         , "Creating pwm_pin for a pin which is in use fails"
         )
{
  ipin i(pin_id(18)); // Use pin 18 
  REQUIRE_THROWS_AS(pwm_pin(pin_id(18)), bad_peripheral_alloc);
}

TEST_CASE( "Platform-tests/pwm_pin/0060/create bad: PWM channel in use"
         , "Creating pwm_pin for a pin which its PWM channel is in use fails"
         )
{
  pwm_pin p{pin_id{18}}; // Use PWM0
  REQUIRE_THROWS_AS(pwm_pin(pin_id(12)), bad_peripheral_alloc);
  ipin(pin_id(12)); // pin should not be in use
}

TEST_CASE( "Platform-tests/pwm_pin/0100/newly created reported not running"
         , "A newly created pwm_pin object returns false from is_running"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  CHECK_FALSE(a_pwm_pin.is_running());
}

TEST_CASE( "Platform-tests/pwm_pin/0110/Started pwm_pin reports running"
         , "A started pwm_pin object returns true from is_running"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.start();
  CHECK(a_pwm_pin.is_running());
}

TEST_CASE( "Platform-tests/pwm_pin/0120/Stopped pwm_pin reports not running"
         , "A started then stopped pwm_pin object returns false from is_running"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.start();
  REQUIRE(a_pwm_pin.is_running());
  a_pwm_pin.stop();
  CHECK_FALSE(a_pwm_pin.is_running());
}

TEST_CASE( "Platform-tests/pwm_pin/0200/Ratio of 1.0, data==range"
         , "Setting high/low ratio to 1.0 gives data value equal to the range"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(1.0);
  CHECK(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
}

TEST_CASE( "Platform-tests/pwm_pin/0210/Ratio of 0.0, data==0"
         , "Setting high/low ratio to 0.0 gives data value equal to zero"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(1.0);
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  a_pwm_pin.set_ratio(0.0);
  CHECK(pwm_ctrl::instance().regs->data1==0U);
}

TEST_CASE( "Platform-tests/pwm_pin/0220/Ratio of 0.5, data==range/2"
         , "Setting high/low ratio to 0.5 gives data value equal to half the range"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(1.0);
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  a_pwm_pin.set_ratio(0.5);
  CHECK(pwm_ctrl::instance().regs->data1==(pwm_ctrl::instance().regs->range1/2));
}

TEST_CASE( "Platform-tests/pwm_pin/0230/Ratios round to nearest whole number (double)"
         , "Data value from ratio is rounded up to the nearest whole number"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(1.0);
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  double ratio_for_data_value_of_point5(0.5/pwm_pin::range_default);
  a_pwm_pin.set_ratio(ratio_for_data_value_of_point5);
  CHECK(pwm_ctrl::instance().regs->data1==1);
  a_pwm_pin.set_ratio(ratio_for_data_value_of_point5-0.0000001);
  CHECK(pwm_ctrl::instance().regs->data1==0);
}

TEST_CASE( "Platform-tests/pwm_pin/0240/Negative ratios fail (double)"
         , "Attempting to set a negative high/low ratio fails"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  REQUIRE_THROWS_AS(a_pwm_pin.set_ratio(-0.00000001), std::out_of_range);
}

TEST_CASE( "Platform-tests/pwm_pin/0250/Ratios greater than unity fail (double)"
         , "Attempting to set a high/low ratio greater than 1.0 fails"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  REQUIRE_THROWS_AS(a_pwm_pin.set_ratio(1.00000001), std::out_of_range);
}


TEST_CASE( "Platform-tests/pwm_pin/0300/Ratio of 1/1 data==range"
         , "Setting high/low ratio to 1/1 gives data value equal to the range"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(pwm_tenths(10));
  CHECK(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
}

TEST_CASE( "Platform-tests/pwm_pin/0310/Ratio of 0/n, data==0"
         , "Setting high/low ratio to 0/n gives data value equal to zero"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(pwm_hundredths(100));
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  a_pwm_pin.set_ratio(pwm_hundredths(0));
  CHECK(pwm_ctrl::instance().regs->data1==0U);
}

TEST_CASE( "Platform-tests/pwm_pin/0320/Ratio of 1/2, data==range/2"
         , "Setting high/low ratio to 1/2 gives data value equal to half the range"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(pwm_thousandths(1000));
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  a_pwm_pin.set_ratio(pwm_thousandths(500));
  CHECK(pwm_ctrl::instance().regs->data1==(pwm_ctrl::instance().regs->range1/2));
}

TEST_CASE( "Platform-tests/pwm_pin/0330/Ratios round to nearest whole number (pwm_ratio)"
         , "Data value from ratio is rounded up to the nearest whole number"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  a_pwm_pin.set_ratio(pwm_millionths(1000000));
  REQUIRE(pwm_ctrl::instance().regs->data1==pwm_ctrl::instance().regs->range1);
  unsigned count_for_data_value_of_half(500000.0/pwm_pin::range_default+0.999);
  a_pwm_pin.set_ratio(pwm_millionths(count_for_data_value_of_half));
  CHECK(pwm_ctrl::instance().regs->data1==1);
  a_pwm_pin.set_ratio(pwm_millionths(count_for_data_value_of_half-1));
  CHECK(pwm_ctrl::instance().regs->data1==0);
}

TEST_CASE( "Platform-tests/pwm_pin/0340/Negative ratios fail (pwm_ratio)"
         , "Attempting to set a negative high/low ratio fails"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  REQUIRE_THROWS_AS(a_pwm_pin.set_ratio(pwm_tenths(-1)), std::out_of_range);
}

TEST_CASE( "Platform-tests/pwm_pin/0350/Ratios greater than unity fail (pwm_ratio)"
         , "Attempting to set a high/low ratio greater than 1.0 fails"
         )
{
  pwm_pin a_pwm_pin{pin_id{18}};
  REQUIRE_THROWS_AS(a_pwm_pin.set_ratio(pwm_millionths(1000001)), std::out_of_range);
}


TEST_CASE( "Platform-tests/pwm_pin/1000/static default frequencies 100MHz"
         , "Check the default values for the PWM clock are all 100MHz"
         )
{
  CHECK(pwm_pin::clock_frequency_min().count()==100000000U);
  CHECK(pwm_pin::clock_frequency_avg().count()==100000000U);
  CHECK(pwm_pin::clock_frequency_max().count()==100000000U);
}

TEST_CASE( "Platform-tests/pwm_pin/1010/static set clock good"
         , "Check the setting the PWM clock to valid value works as expected"
         )
{
  pwm_pin::set_clock( fixed_oscillator_clock_source{f_megahertz{19.2}}
                    , clock_frequency{kilohertz{600}, clock_filter::none});
  CHECK(pwm_pin::clock_frequency_min().count()==600000U);
  CHECK(pwm_pin::clock_frequency_avg().count()==600000U);
  CHECK(pwm_pin::clock_frequency_max().count()==600000U);
}

TEST_CASE( "Platform-tests/pwm_pin/1020/static set clock bad parameters"
         , "Check attempting to set PWM clock with bad clock parameters fails"
         )
{
  REQUIRE_THROWS_AS(  pwm_pin::set_clock
                      ( fixed_oscillator_clock_source{f_megahertz{24}}
                      , clock_frequency{megahertz{16}, clock_filter::minimum}
                      )
                   , std::range_error
                   );

}
