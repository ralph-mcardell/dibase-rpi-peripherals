// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/pulse_counter.cpp 
/// @brief Use ipin_edge_event to count pluses on a GPIO input pin
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

#include "pin_edge_event.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <iomanip>      // for std::setw
#include <chrono>
#include <thread>
#include <atomic>

using namespace dibase::rpi::peripherals;
bool g_running{ true };  ///< Global flag used to communicate quit request

std::atomic<unsigned> g_count{0U};

void count_pulses()
{
  try
    {
      ipin in{gpio_gclk};           // Gertboard J2 GP4 -- connect to i/p device

      pin_edge_event pin_evt(in, pin_edge_event::falling);
      constexpr auto timeout(std::chrono::milliseconds{65});

      while (g_running)
        {
          if (pin_evt.wait_for(timeout))
            {
              ++g_count;
              pin_evt.clear();
            }
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
}

void display_frequency()
{
  constexpr auto sample_duration(std::chrono::milliseconds{50});
  auto t_sample(std::chrono::system_clock::now());
  while (g_running)
    {
      do
        { // Set next sample time point
          t_sample += sample_duration;
        }
      while ( t_sample < std::chrono::system_clock::now() );
      auto sample_t0(std::chrono::system_clock::now());
      std::this_thread::sleep_until( t_sample );
      unsigned cnt{g_count.load()};
      auto sample_t(std::chrono::system_clock::now()-sample_t0);
      
      g_count.store(0U);
      std::cout << "Frequency: " << std::setw(8) 
                << (float(cnt)*std::chrono::system_clock::period::den)
                    /(sample_t.count()*std::chrono::system_clock::period::num)
                << "Hz (count=" << cnt << ", sample_t = " << sample_t.count()
                << "*[" << std::chrono::system_clock::period::num << '/'
                << std::chrono::system_clock::period::den << "])"
                <<"\r";
      std::cout.flush();
    }
  std::cout << std::endl;
}

int main()
{
  std::cout << "Press enter to quit....\n";
  std::thread outputter{ display_frequency };
  std::thread counter{ count_pulses };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  counter.join();
  outputter.join();
}
