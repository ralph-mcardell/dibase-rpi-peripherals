// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-string-display.cpp
/// @brief GPIO output to 8 LEDs - assumed to be in a 'string' like Xmas lights
//
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <thread>
#include <chrono>
#include <vector>
#include <utility>

using namespace dibase::rpi::peripherals;
bool g_running{ true };  ///< Global flag used to communicate quit request

/// @brief Description of LED state: LED index [0,7], on|off 
typedef std::pair<int,bool>           led_desc;

/// @brief  Set of LED state changes: collection of LED state descriptions to apply
typedef std::vector<led_desc>         led_string_delta;

/// @brief  LED lighting effect sequence: 
/// Time between state changes, set of state changes
struct led_sequence
{
  std::chrono::milliseconds       delay;
  std::vector<led_string_delta>   deltas;  
};

/// @brief  Collection of LED lighting effect sequences
typedef std::vector<led_sequence> led_sequences;

/// @brief  Definition of all LED lighting effect sequences used by program
led_sequences
  seqs =  { { std::chrono::milliseconds{200}
            , { { {0,true},{1,false},{2,false},{3,false}
                , {4,false},{5,false},{6,false},{7,false} }
              , {{0,false},{1,true}}
              , {{1,false},{2,true}}
              , {{2,false},{3,true}}
              , {{3,false},{4,true}}
              , {{4,false},{5,true}}
              , {{5,false},{6,true}}
              , {{6,false},{7,true}}
              }
            }
          , { std::chrono::milliseconds{600}
            , { { {0,true},{1,true},{2,true},{3,true}
                , {4,true},{5,true},{6,true},{7,true} }
              , { {0,false},{1,false},{2,false},{3,false}
                , {4,false},{5,false},{6,false},{7,false} }
              }
            }
          , { std::chrono::milliseconds{120}
            , { { {0,true},{1,false},{2,false},{3,false}
                , {4,false},{5,false},{6,false},{7,false} }
              , {{0,false},{1,true}}
              , {{1,false},{2,true}}
              , {{2,false},{3,true}}
              , {{3,false},{4,true}}
              , {{4,false},{5,true}}
              , {{5,false},{6,true}}
              , {{6,false},{7,true}}
              , {{7,false},{6,true}}
              , {{6,false},{5,true}}
              , {{5,false},{4,true}}
              , {{4,false},{3,true}}
              , {{3,false},{2,true}}
              , {{2,false},{1,true}}
              }
            }
          , { std::chrono::milliseconds{140}
            , { { {0,true},{1,false},{2,true},{3,false}
                , {4,true},{5,false},{6,true},{7,false} }
              , { {0,false},{1,true},{2,false},{3,true}
                , {4,false},{5,true},{6,false},{7,true} }
              , { {0,true},{1,false},{2,true},{3,false}
                , {4,true},{5,false},{6,true},{7,false} }
              , { {0,false},{1,true},{2,false},{3,true}
                , {4,false},{5,true},{6,false},{7,true} }
              }
            }
          , { std::chrono::milliseconds{200}
            , { { {0,false},{1,false},{2,false},{3,true}
                , {4,true},{5,false},{6,false},{7,false} }
              , {{3,false},{4,false},{2,true},{5,true}}
              , {{2,false},{5,false},{1,true},{6,true}}
              , {{1,false},{6,false},{0,true},{7,true}}
              }
            }
          , { std::chrono::milliseconds{200}
            , { { {0,true},{1,false},{2,false},{3,true}
                , {4,false},{5,false},{6,true},{7,false} }
              , { {0,false},{1,true},{2,false},{3,false}
                , {4,true},{5,false},{6,false},{7,true} }
              , { {0,false},{1,false},{2,true},{3,false}
                , {4,false},{5,true},{6,false},{7,false} }
              }
            }
          };

// Apply a single set of LED state changes
void apply(led_string_delta const & delta, opin ** leds)
{
  for (auto & desc : delta)
    {
      leds[desc.first]->put(desc.second);
    }
}

/// @brief  Core program logic
///
/// - create the 8 GPIO opin, out pins that drive the LEDs
/// - set initial state
/// - while running:
///    - apply sequences a number of times each
///      - switch sequence after each one completes a set number of times
///        - wrap from last to first sequence
void do_light_show()
{
  try
    {
      opin led0{gpio_gen6}; // Gertboard J2 GP25
      opin led1{gpio_gen5}; // Gertboard J2 GP24
      opin led2{gpio_gen4}; // Gertboard J2 GP23
      opin led3{gpio_gen3}; // Gertboard J2 GP22
      opin led4{rxd};       // Gertboard J2 GP15
      opin led5{gpio_gen2}; // Gertboard J2 GP21 (GPIO27 on rev.2 boards)
      opin led6{gpio_gen1}; // Gertboard J2 GP18 
      opin led7{gpio_gen0}; // Gertboard J2 GP17 
      opin * leds[] = {&led0, &led1, &led2, &led3, &led4, &led5, &led6, &led7};

      auto t_wake(std::chrono::system_clock::now());
      constexpr auto t_snooze(std::chrono::milliseconds{50});
      auto t_do_change(std::chrono::system_clock::now());
      constexpr int iterations_per_sequence{10};
      std::size_t seq_idx{0};
      auto seq(seqs[seq_idx]);
      auto pos = seq.deltas.begin();
      int count{0};
      while (g_running) 
        {
          std::this_thread::sleep_until(t_wake);
          if ( t_do_change < std::chrono::system_clock::now() )
            {
              apply(*pos,leds);
              t_do_change += seq.delay;
              ++pos;
              if ( pos==seq.deltas.end())
                {
                  if (++count>=iterations_per_sequence)
                   {
                      count = 0;
                      ++seq_idx;
                      if (seq_idx >= seqs.size())
                        {
                          seq_idx = 0;
                        }
                     seq = seqs[seq_idx];
                   }
                  pos = seq.deltas.begin();
                }
            }
          t_wake += t_snooze;
        }

      for (int i = 0; i != 8; ++i)
        leds[i]->put(false);
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
}

/// @brief Tell user how to quit, spawn worker thread, wait for user quit request
///
/// Runs main work on separate thread while main thread is waiting for console
/// input from user, whereupon no quit running request signalled by setting
/// global g_running flag to false, and the worker thread is joined to wait
/// for it to exit before returning.
int main()
{
  std::cout << "Press enter to quit....\n";
  std::thread counter{ do_light_show };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  counter.join();
}
