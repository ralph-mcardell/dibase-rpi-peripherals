// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/pwm-motor.cpp 
/// @brief Gertboard_sw motor program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

#include "pin.h"
#include "pwm_pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <iomanip>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace dibase::rpi::peripherals;
bool g_running{ true };  ///< Global flag used to communicate quit request

void vary_motor_speed_and_direction()
{
  opin direction_pin;
  try
    {
      direction_pin.open(gpio_gen0); // Gertboard GP17 (GPI017)
      pwm_pin::set_clock( rpi_oscillator
                        , clock_frequency{kilohertz{600}, clock_filter::none}
                        );
      pwm_pin power_pin{gpio_gen1};  // Gertboard GP18 (GPI018)
      bool direction{false};
      direction_pin.put(direction);
      int power_value{0}; // range [-100,100]; negative values reverse direction
      int power_change{1};// Start by incrementing speed forwards
      unsigned const change_wait_ms{200U}; // Delay between changes
      power_pin.start();
    // Ramp speed up by varying power from 0 to 100%, then decrease to zero, 
    // reverse direction, varying power to 100% in reverse, decrease reverse
    // power to zero, ramp forward speed up by varying power...etc. until
    // user bored and quits program run!
      while (g_running)
        {// When the motor's direction is reversed by raising the direction
         // pin's state we have to invert the PWM high/low ratio
          unsigned percent_power{direction?100+power_value:power_value};

          power_pin.set_ratio(pwm_hundredths(percent_power));
          std::cout << (direction?"<<<":">>>")
                    << std::setw(3) << std::abs(power_value) << "%\r";
          std::cout.flush();
          std::this_thread::sleep_for(std::chrono::milliseconds(change_wait_ms));
          if ( std::abs(power_value)==100 )
            {  // at range ends switch between increase<->decrease
              power_change *= -1;
            }
          else if ( power_value==-power_change ) // ASUMMES power_change 1 or -1
            { // at zero-crossing switch direction pin state
              direction = !direction;
              direction_pin.put(direction);
            }
          power_value += power_change;
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
  catch (...)
    {
      std::cerr << "A problem occurred. Caught by catch all handler\n";
    }
  direction_pin.put(false);
}

int main()
{
  std::cout << "Press enter to quit...." << std::endl;
  std::thread worker{ vary_motor_speed_and_direction };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  worker.join();
}
