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
#include <sched.h>
#include <string.h>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  auto const short_wait_time(std::chrono::microseconds(5U));

  static void prompt( std::string text )
  {
    std::cout << text;
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

  struct xfer_stats // Transfer performance data.
  {
    xfer_stats()
    : rcount{0ULL}
    , wcount{0ULL}
    , rfcount{0ULL}
    , wecount{0ULL}
    , duration_ms{0U}
    , actual_cdiv{0U}
    , hi_pri{false}
    {}
    
    std::uint64_t rcount;
    std::uint64_t wcount;

    unsigned      rfcount; // read full count
    unsigned      wecount; // write empty count
    
    unsigned duration_ms;
    unsigned actual_cdiv;
    bool     hi_pri;
  };

  bool set_hi_pri()
  {
    struct sched_param sched;
    memset (&sched, 0, sizeof(sched));
    sched.sched_priority = sched_get_priority_max(SCHED_RR);
    return sched_setscheduler(0, SCHED_RR, &sched)!=-1;
  }
  
  void do_bkgd_xfer
  ( hertz test_frequency
  , bool const & stop, xfer_stats & perf
  , bool use_hi_pri
  )
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);
    spi0_conversation sc(spi0_slave::chip0, test_frequency);
    sc.open(sp);
    perf.actual_cdiv = spi0_ctrl::instance().regs->get_clock_divider();
    std::uint8_t data{0U};
    std::uint64_t rcount_prev{0ULL};
    std::uint64_t wcount_prev{0ULL};
    if (use_hi_pri)
      {
        perf.hi_pri = set_hi_pri();
      }
    auto const t_start(std::chrono::system_clock::now());
    while ( !stop )
      {
        rcount_prev = perf.rcount;
        wcount_prev = perf.wcount;
        if (sp.read_fifo_is_full())   ++perf.rfcount;
        if (sc.read(data))            ++perf.rcount;
        if (sp.write_fifo_is_empty()) ++perf.wecount;
        if (sc.write(0x5a))           ++perf.wcount;
        if (perf.rcount==rcount_prev && perf.wcount==wcount_prev)
          { // nothing read or written - let something else run...
            std::this_thread::sleep_for(std::chrono::microseconds(2));
          }
      }
    while (sp.read_fifo_has_data() || !sp.write_fifo_is_empty())
      {
        if (sp.read_fifo_is_full())   ++perf.rfcount;
        if (sc.read(data))            ++perf.rcount;
      }
    auto t_elapsed((std::chrono::monotonic_clock::now()-t_start));
    perf.duration_ms = std::chrono::duration_cast
                        <std::chrono::milliseconds>(t_elapsed).count();
  }

  std::string hertz_to_string(hertz f)
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
    oss << f_display << f_units;
    return oss.str();
  }
  
  bool test_transfer(hertz f, bool use_hi_pri=false)
  {
    std::cout << "Transfer at " << hertz_to_string(f) 
              << " for approximately 1 second...\n";
    bool stop{false};
    xfer_stats perf;
    std::thread spi_run(do_bkgd_xfer, f, std::cref(stop), std::ref(perf), use_hi_pri);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stop = true;
    spi_run.join();
    --perf.wecount; // discount count for initial empty state
    std::cout << "Wrote " << perf.wcount 
              << " bytes (FIFO empty " << perf.wecount 
              << " times), read " << perf.rcount 
              << " bytes (FIFO full " << perf.rfcount 
              << " times) in " << perf.duration_ms 
              << "ms using a CDIV of " << perf.actual_cdiv 
              << (perf.hi_pri?"  HIGH":"  NORMAL") << " priority"
              << '\n';
    double scale_to_per_sec(1000.0/perf.duration_ms);
    std::cout << "      " << std::uint64_t(perf.wcount*scale_to_per_sec+0.5)
              << " Bps               " 
              << unsigned(perf.wecount*scale_to_per_sec+0.5)
              << "/sec          " 
              <<  std::uint64_t(perf.rcount*scale_to_per_sec+0.5)
              << " Bps              " 
              << unsigned(perf.rfcount*scale_to_per_sec+0.5)
              << "/sec\n\n";
    return true;
  }

  bool test_clock_frequency(hertz f)
  {
    std::ostringstream oss;
    oss << "Expect " <<  hertz_to_string(f)
        << " clock waveform. Press <enter> when ready...";
    prompt(oss.str());
    bool stop{false};
    xfer_stats perf;
    std::thread spi_run(do_bkgd_xfer, f, std::cref(stop), std::ref(perf), false);
    oss.str("");
    oss << "Has the SCLK output a " <<  hertz_to_string(f) << " frequency";
    bool result(yn_query(oss.str().c_str()));
    stop = true;
    spi_run.join();
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
}

TEST_CASE( "Interactive_tests/spi0_pins/0000/read write standard SPI"
         , "Check the CLK pin frequency is as expected"
         )
{
  welcome();
  std::cout << "\nSPI0 SPI standard mode write-read test:\n\n";
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  hertz freq(kilohertz(100));
  spi0_conversation sc(spi0_slave::chip0, freq);
  sc.open(sp);
  constexpr unsigned ByteRange{256U};
  std::array<int, ByteRange> transfer_data;
  transfer_data.fill(-999); // indicates never written
  std::uint8_t data{0U};
  for (unsigned v=0U; v!=ByteRange; ++v)
    {
      if (sp.read_fifo_has_data())
        {
           if (sc.read(data))
            {
              std::cout << "R("<< int(data) << ") ";
              ++transfer_data[data];
            }
        }
      while (!sp.write_fifo_has_space())
        {
          std::this_thread::yield();
        }
      if (sc.write(v))
        {
          std::cout << "W("<< v << ") ";
          transfer_data[v] = 0;
        }
    }
 // Drain read data in FIFOs...
  bool draining{true};
  while (draining)
    {
      if ( draining && sp.write_fifo_is_empty() )
        { // TX FIFO just noticed to be empty: wait enough time for last
          // TX item to be definitely clocked into RX FIFO
          std::this_thread::sleep_for(std::chrono::nanoseconds(std::uint64_t((9.0/freq.count())*1000000000)));
          draining = false;
        }
      while (sc.read(data))
        {
          std::cout << "R("<< int(data) << ") ";
          ++transfer_data[data];
        }
    }
  std::cout << std::endl;
  for (unsigned i=0U; i!=ByteRange; ++i)
    {
      CHECK(transfer_data[i]==1); // implies written and read once
    }
}

TEST_CASE( "Interactive_tests/spi0_pins/0020/fifo full,empty: normal priority"
         , "Observe the ability of a program to continuously keep data in the "
           "TX FIFO and prevent the RX FIFO filling"
         )
{
  welcome();
  std::cout << "\nSPI0 continuous transfer tests @ NORMAL priority\n\n";
 
  CHECK(test_transfer(kilohertz(5)));
  CHECK(test_transfer(kilohertz(50)));
  CHECK(test_transfer(kilohertz(100)));
  CHECK(test_transfer(kilohertz(200)));
  CHECK(test_transfer(kilohertz(500)));
  CHECK(test_transfer(megahertz(1)));
  CHECK(test_transfer(megahertz(2)));
}

TEST_CASE( "Interactive_tests/spi0_pins/0030/fifo full,empty: high priority"
         , "Observe the ability of a program to continuously keep data in the "
           "TX FIFO and prevent the RX FIFO filling"
         )
{
  welcome();
  std::cout << "\nSPI0 continuous transfer tests @ HIGH priority\n\n";

  CHECK(test_transfer(kilohertz(5),true));
  CHECK(test_transfer(kilohertz(50),true));
  CHECK(test_transfer(kilohertz(100),true));
  CHECK(test_transfer(kilohertz(200),true));
  CHECK(test_transfer(kilohertz(500),true));
  CHECK(test_transfer(megahertz(1),true));
  CHECK(test_transfer(megahertz(2),true));
}

TEST_CASE( "Interactive_tests/spi0_pins/0040/clock frequency"
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
  CHECK(test_clock_frequency(kilohertz(100)));
  CHECK(test_clock_frequency(kilohertz(200)));
  CHECK(test_clock_frequency(kilohertz(500)));
  CHECK(test_clock_frequency(megahertz(1)));
  CHECK(test_clock_frequency(megahertz(2)));
}
