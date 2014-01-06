// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/leds_and_switches.cpp
/// @brief GPIO output and input - assumed to be connected to LEDs and switches
//
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell
//

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <unistd.h>     // for usleep
#include <thread>

using namespace dibase::rpi::peripherals;

bool g_running{ true };

void count_switch_presses()
{
  try
    {
      opin led0{gpio_gen6};
      opin led1{gpio_gen5};
      opin led2{gpio_gen4};
      opin led3{gpio_gen3};

      ipin switch0{gpio_gen2};

      unsigned const switch_delay_ms{70};
      unsigned const switch_delay_us{switch_delay_ms*1000};
      
      unsigned count{0};
      while (g_running) 
        {
          while (!switch0.get() && g_running)
            usleep(switch_delay_us);
          while (switch0.get() && g_running)
            usleep(switch_delay_us);
          if (g_running)
            {
              ++count;
              led0.put((count&1));
              led1.put((count&2));
              led2.put((count&4));
              led3.put((count&8));
              std::cout << "Count: " << count << std::endl;
            }
        }
      led0.put(false);
      led1.put(false);
      led2.put(false);
      led3.put(false);
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
}

int main()
{
  std::cout << "Press enter to quit....\n";
  std::thread counter{ count_switch_presses };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  counter.join();
}
