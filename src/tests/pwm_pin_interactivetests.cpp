// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin_interactivetests.cpp 
/// @brief Interactive platform tests for pwm_pin type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "pwm_pin.h"
#include <iostream>
#include <string>
#include "interactivetests_config.h"
#include <thread>

using namespace dibase::rpi::peripherals;

namespace
{
  pin_id const available_pwm_pin_id(test::pwm_pin_id); 
  auto const short_wait_time(std::chrono::milliseconds(100U));

  static void prompt(std::string prefix, std::string suffix)
  {
    std::cout << prefix << available_pwm_pin_id << suffix;
    std::string dummy;
    std::getline(std::cin, dummy);
  }

  static bool yn_query(char const * query_text)
  {
    std::string response;
    do
      {
        std::cout << query_text << " (y/n)? ";
        std::getline(std::cin, response);
      }
    while (response.size()!=1 || response.find_first_of("yYnN")!=0);
    return response.find_first_of("Yy")==0;
  }
}

TEST_CASE( "Interactive_tests/pwm_pin/0000/create start, vary ratio, stop pwm"
         , "Creates pwm_pin in expected state when passed valid parameters "
           "and starts, shows correct high/low signal ratios and stops"
         )
{
  std::cout << "\nPWM output test:\n"
               "Connect pin BCM2835 GPIO" << available_pwm_pin_id 
            << " to a waveform monitoring device (oscilloscope etc.).\n"
            << "Expect PWM output derived from a 600KHz square waveform.\n\n";
  pwm_pin::set_clock( rpi_oscillator
                    , clock_frequency{kilohertz{600}, clock_filter::none}
                    );
  CHECK(pwm_pin::clock_frequency_min()==hertz{600000U});
  CHECK(pwm_pin::clock_frequency_avg()==hertz{600000U});
  CHECK(pwm_pin::clock_frequency_max()==hertz{600000U});

  pwm_pin pwm { available_pwm_pin_id };
  CHECK_FALSE(pwm.is_running());
  prompt("Press <Enter> to start PWM with 1:0 high-low ratio on BCM2835 GPIO", "...");
  pwm.set_ratio(1);
  pwm.start();
  std::this_thread::sleep_for(short_wait_time);
  CHECK(pwm.is_running());
  CHECK( yn_query("Is the PWM output fully high") );

  prompt("Press <Enter> to set PWM to 0:1 high-low ratio on BCM2835 GPIO", "...");
  pwm.set_ratio(0);
  CHECK( yn_query("Is the PWM output fully low") );

  prompt("Press <Enter> to set PWM to 1:4 high-low ratio on BCM2835 GPIO", "...");
  pwm.set_ratio(pwm_hundredths(25));
  CHECK( yn_query("Is the PWM output 1:4  high-low") );

  prompt("Press <Enter> to set PWM to 1:2 high-low ratio on BCM2835 GPIO", "...");
  pwm.set_ratio(pwm_hundredths(50));
  CHECK( yn_query("Is the PWM output 1:2  high:low") );

  prompt("Press <Enter> to set PWM to 3:4 high-low ratio on BCM2835 GPIO", "...");
  pwm.set_ratio(pwm_hundredths(75));
  CHECK( yn_query("Is the PWM output 3:4  high-low") );

  prompt("Press <Enter> to stop PWM on BCM2835 GPIO", "...");
  pwm.stop();
  std::this_thread::sleep_for(short_wait_time);
  CHECK_FALSE(pwm.is_running());
  CHECK( yn_query("Has the PWM channel stopped") );
}
