// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins_interactivetests.cpp 
/// @brief Platform tests for spi0_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "spi0_pins.h"
#include "spi0_ctrl.h"
#include "periexcept.h"
#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <cstdint>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  auto const short_wait_time(std::chrono::microseconds(20U));

  static void prompt
  ( std::string prefix
  , std::string suffix=std::string()
  , unsigned pin0 = 0U
  , unsigned pin1 = 0U
  , unsigned pin2 = 0U
  , unsigned pin3 = 0U
  , unsigned pin4 = 0U
  )
  {
    std::cout << prefix;
    if (pin0) std::cout << ' ' << pin0;
    if (pin1) std::cout << ' ' << pin1;
    if (pin2) std::cout << ' ' << pin2;
    if (pin3) std::cout << ' ' << pin3;
    if (pin4) std::cout << ' ' << pin4;
    if (!suffix.empty()) std::cout << ' ' << suffix;
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

  static bool stop{false};
  static std::uint64_t rcount{0ULL};
  static std::uint64_t wcount{0ULL};
  
  void do_frequency_test(hertz test_frequency)
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);
    spi0_conversation sc(spi0_slave::chip0, test_frequency);
    sc.open(sp);
    while (!stop)
      {
        while (sp.read_fifo_has_data())
          {
            std::uint8_t data;
            if (sc.read(data) && data==0x5a)
              {
                ++rcount;
              }
          }
        if (sp.write_fifo_has_space())
          {
            if (sc.write(0x5a))
              {
                ++wcount;
              }
          }
         else
          {
            std::this_thread::sleep_for(short_wait_time);
          }
      }
  }
}


TEST_CASE( "Interactive_tests/spi0_pins/0000/clock_fequency"
         , "Check the CLK pin frequency is as expected"
         )
{
  std::cout << "\nSPI0 SCLK frequency test:\n."
               "Connect SPI_SCLK - pin " 
            << rpi_p1_spi0_full_pin_set.sclk()
            << " to a frequency measuring instrument (oscilloscope etc.).\n\n";
  prompt("1) Expect 1MHz square waveform. Press <enter> when ready...");

  std::thread mhz1(do_frequency_test, megahertz(1U));
  CHECK(yn_query("Has the SCLK output a 1MHz frequency"));
  stop = true;
  mhz1.join();
  stop = false;
  std::cout << "Wrote " << wcount << " bytes, read " << rcount << " bytes.\n";
  rcount = wcount = 0ULL;
}
