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
#include <sstream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <cstdint>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  auto const short_wait_time(std::chrono::microseconds(5U));

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
  static unsigned duration_ms{0U};
  static unsigned actual_cdiv{0U};
  
  void do_frequency_test(hertz test_frequency)
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);
    spi0_conversation sc(spi0_slave::chip0, test_frequency);
    sc.open(sp);
    actual_cdiv = spi0_ctrl::instance().regs->get_clock_divider();
  // g++ c++ library has non-standard monotonic_clock rather than steady_clock
    auto t_start(std::chrono::monotonic_clock::now());
    while (!stop)
      {
        while (sp.read_fifo_has_data())
          {
            std::uint8_t data;
            if (sc.read(data))
              {
                ++rcount;
              }
          }
        if (sp.write_fifo_has_space())
          {
            while (sc.write(0x5a))
              {
                ++wcount;
              }
          }
         else
          {
            std::this_thread::sleep_for(short_wait_time);
          }
      }
    auto t_elapsed((std::chrono::monotonic_clock::now()-t_start));
    duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_elapsed).count();
  }

  bool test_clock_frequency(hertz f)
  {
    std::string f_units("Hz");
    double f_display(f.count());
    if (f.count()>999999U)
      {
        f_units = "MHz";
        f_display = f_megahertz(f).count();
      }
    else if (f.count()>999U)
      {
        f_units = "KHz";
        f_display = f_kilohertz(f).count();
      }
    std::ostringstream oss;
    oss << "Expect " << f_display << f_units 
        << " square waveform. Press <enter> when ready...";
    prompt(oss.str());
    std::thread spi_run(do_frequency_test, f);
    oss.str("");
    oss << "Has the SCLK output a " << f_display << f_units << " frequency";
    bool result(yn_query(oss.str().c_str()));
    stop = true;
    spi_run.join();
    stop = false;
    std::cout << "Wrote " << wcount << " bytes, read " << rcount << " bytes in "
              << duration_ms << "ms using a CDIV of " << actual_cdiv << "\n";
    rcount = wcount = 0ULL;
    duration_ms = actual_cdiv = 0U;
    return result;
  }

  void welcome()
  {
    struct write_text
    {
      write_text()
      {
        std::cout << "\nSPI0 Interactive tests\n"
                  << "These tests use a loop-back configuration:\n"
                  << "Connect SPI0_MOSI - GPIO"
                  << rpi_p1_spi0_full_pin_set.mosi()
                  << ", to SPI0_MISO - GPIO"
                  << rpi_p1_spi0_full_pin_set.miso()
                  << "\n\n";
      }
      void dummy() {}
    };
    static write_text t;
    t.dummy(); // Keeps compiler quiet
  }

  hertz cdiv_to_hz(std::uint16_t cdiv)
  {
    cdiv &= ~1U; // strip LSB
    double cdivd{cdiv?cdiv:65536};
    return hertz(unsigned(rpi_apb_core_frequency.count() / cdivd + 0.5));
  }
}


TEST_CASE( "Interactive_tests/spi0_pins/0000/clock_fequency"
         , "Check the CLK pin frequency is as expected"
         )
{
  welcome();
  std::cout << "\nSPI0 SCLK frequency test:\n"
               "Connect SPI_SCLK - GPIO" 
            << rpi_p1_spi0_full_pin_set.sclk()
            << " to a frequency measuring instrument (oscilloscope etc.).\n\n";
 
  CHECK(test_clock_frequency(kilohertz(5)));
  CHECK(test_clock_frequency(kilohertz(50)));
  CHECK(test_clock_frequency(kilohertz(500)));
  CHECK(test_clock_frequency(megahertz(1)));
  CHECK(test_clock_frequency(megahertz(2)));
  CHECK(test_clock_frequency(cdiv_to_hz(0)));
  CHECK(test_clock_frequency(cdiv_to_hz(1)));
}
