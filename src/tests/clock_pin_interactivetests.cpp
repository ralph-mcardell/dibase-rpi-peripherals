// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin_interactivetests.cpp 
/// @brief Interactive platform tests for clock_pin type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_pin.h"
#include "pin.h"
#include "pinexcept.h"
#include <iostream>
#include <string>
#include "interactivetests_config.h"
#include <thread>

using namespace dibase::rpi::peripherals;

namespace
{
  pin_id const available_clk_pin_id(test::clk_pin_id); 
  auto const short_wait_time(std::chrono::milliseconds(100U));

  static void prompt(std::string prefix, std::string suffix)
  {
    std::cout << prefix << available_clk_pin_id << suffix;
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

TEST_CASE( "Interactive_tests/clock_pin/0000/create start, stop 600KHz clock"
         , "Creates clock_pin in expected state when passed valid parameters"
         )
{
  std::cout << "\nClock output test:\n"
               "Connect pin BCM2835 GPIO" << available_clk_pin_id 
            << " to a waveform monitoring device (oscilloscope etc.)."
            << " Expect a 600KHz square waveform.\n\n";
  clock_pin clk { pin_id{4}
                , fixed_oscillator_clock_source{f_megahertz{19.2}}
                , clock_frequency{kilohertz{600}, clock_filter::none}
                };
  CHECK(clk.frequency_min()==hertz{600000U});
  CHECK(clk.frequency_avg()==hertz{600000U});
  CHECK(clk.frequency_max()==hertz{600000U});
  CHECK_FALSE(clk.is_running());
  prompt("Press <Enter> to start clock on BCM2835 GPIO", "...");
  clk.start();
  std::this_thread::sleep_for(short_wait_time);
  CHECK(clk.is_running());
  CHECK( yn_query("Is the clock running") );

  prompt("Press <Enter> to stop clock on BCM2835 GPIO", "...");
  clk.stop();
  std::this_thread::sleep_for(short_wait_time);
  CHECK_FALSE(clk.is_running());
  CHECK( yn_query("Has the clock stopped") );
}
