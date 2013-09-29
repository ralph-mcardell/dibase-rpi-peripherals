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

class motor
{
    pwm_pin   power_pin;
    opin      direction_pin;

public:
    motor(pin_id pwr_pin, pin_id dir_pin)
    : power_pin{pwr_pin}
    , direction_pin{dir_pin}
    {
      power_pin.start();
      set_speed(0.0);
    }

    // -1.0 (full reverse) - 0 (stop) - 1.0 (full forward)
    void set_speed(double speed);
 
    ~motor()
    {
      set_speed(0.0);  
    }
};

void motor::set_speed(double speed)
{
  if (speed<-1.0) speed = -1.0;
  if (speed>1.0)  speed = 1.0;
  if ( speed<0.0)
    {
      direction_pin.put(true);
      power_pin.set_ratio(1.0+speed);
    }
  else
    {
      direction_pin.put(false);
      power_pin.set_ratio(speed);
    }
}

void vary_motor_speed_and_direction()
{
  try
    {
      pwm_pin::set_clock( rpi_oscillator
                        , clock_frequency{kilohertz{600}, clock_filter::none}
                        );
      int speed_value{0}; // range [-100,100]; negative values reverse direction
      int speed_change{1};// Start by incrementing speed forwards
      unsigned const change_wait_ms{200U}; // Delay between changes
      motor m(gpio_gen1, gpio_gen0);
    // Repeatedly ramp speed forward from 0 to 100%, then decrease to 0, ramp
    // speed to 100% in reverse, then to 0...etc. until user bored & quits!
      while (g_running)
        {
          m.set_speed(speed_value/100.0);
          std::cout << (speed_value<0.0?"<<<":">>>")
                    << std::setw(3) << std::abs(speed_value) << "%\r";
          std::cout.flush();
          std::this_thread::sleep_for(std::chrono::milliseconds(change_wait_ms));
          if ( std::abs(speed_value)==100 )
            {  // at range ends switch between increase<->decrease
              speed_change *= -1;
            }
          speed_value += speed_change;
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
