// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

// ##### Ongoing version. Will be updated when more facilities avaialble #####

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <chrono>
#include <thread>

using namespace dibase::rpi::peripherals;
bool g_running{ true };  ///< Global flag used to communicate quit request

void switch_oc_output_on_input()
{
  try
    {
      opin gpio_clk_out(gpio_gclk); // Gertboard J2 GP4 -- connect to RLYn
      ipin in{spi_ce0_n};           // Gertboard J2 GP8 -- connect to i/p device

  // Open collector state of RLYn follows that of GP8 input value
  // track changes of input state until not running...
      bool in_value{in.get()};
      gpio_clk_out.put(in_value);
      unsigned const input_change_wait_ms{50};
      while (g_running)
        {
          while (in.get()==in_value && g_running)
            std::this_thread::sleep_for
                              (std::chrono::milliseconds(input_change_wait_ms));
          if (g_running)
            {
              in_value = ! in_value;
              gpio_clk_out.put(in_value);
            }
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}

int main()
{
  std::cout << "Press enter to quit....\n";
  std::thread worker{ switch_oc_output_on_input };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  worker.join();
}
