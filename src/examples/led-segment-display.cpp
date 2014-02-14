// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-segment-display.cpp
/// @brief GPIO input and output to a 7-segment (or better) LED display
//
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <thread>
#include <chrono>

/// @brief Class encapsulating resources and operations on a 7 segment display
///
/// Uses 7 GPIO output pins - one per display segment - and can handle
/// segments being on when output values are high or low as indicated by a
/// flag passed during construction.
///
/// The 7-segment display is assumed to have the following structure:
///
///     A
///    ---
/// F | G | D
///    ---
/// E | B | C
///    ---
///
/// And should be connected such that for (integer) values representing the
/// segments to turn on with bit 0 being the least significant:
///   bit 0 represents segment A,        bit 1 represents segment B,
///   bit 2 represents segment C,        bit 3 represents segment D,
///   bit 4 represents segment E,        bit 5 represents segment F,
///   bit 6 represents segment G
class seven_segment
{
  typedef dibase::rpi::peripherals::opin    opin;
  typedef dibase::rpi::peripherals::pin_id  pin_id;

  opin segment_a;
  opin segment_b;
  opin segment_c;
  opin segment_d;
  opin segment_e;
  opin segment_f;
  opin segment_g;
  opin * segments[7];
  bool active_low;

public:
/// @brief construct from pin ids for the 7 segments & segment on if low flag
/// @param segA           Id of GPIO output pin connected to segment A
/// @param segB           Id of GPIO output pin connected to segment B
/// @param segC           Id of GPIO output pin connected to segment C
/// @param segD           Id of GPIO output pin connected to segment D
/// @param segE           Id of GPIO output pin connected to segment E
/// @param segF           Id of GPIO output pin connected to segment F
/// @param segG           Id of GPIO output pin connected to segment G
/// @param is_on_when_low If true then segments ON when GPIO pins are LOW
  seven_segment
  ( pin_id segA
  , pin_id segB
  , pin_id segC
  , pin_id segD
  , pin_id segE
  , pin_id segF
  , pin_id segG
  , bool is_on_when_low
  )
  : segment_a{segA}
  , segment_b{segB}
  , segment_c{segC}
  , segment_d{segD}
  , segment_e{segE}
  , segment_f{segF}
  , segment_g{segG}
  , segments{&segment_a,&segment_b,&segment_c
            ,&segment_d,&segment_e,&segment_f,&segment_g}
  , active_low{is_on_when_low}
  {}
  seven_segment(seven_segment const&) = delete;
  seven_segment(seven_segment &&) = delete;
  seven_segment& operator=(seven_segment const&) = delete;
  seven_segment& operator=(seven_segment &&) = delete;

/// @brief Destroy: set all outputs LOW irrespective of segment-on pin state
  ~seven_segment()
  {
    for (auto seg : segments)
      seg->put(false);
  }
  
/// @brief Clear 7 segment display turning all segments OFF
  void clear();

/// @brief Show decimal least significant digit of value on 7-segment display
/// @param digit  Value to show least significant digit of on 7-segment display
  void show(unsigned int digit);
};

/// @brief Class encapsulating dual 7-segment display multiplexing segment pins
///
/// Special treatment of 2-digit 7-segment display that multiplexes the
/// segment state control pins for each digit onto a single set of 7 pins.
///
/// Hence only 8 output pins are used: 7 for the multiplexed segment values and
/// one that selects which digit to apply the segment values to. It would be 
/// possible to re-work to use two select pins but that seems wasteful of GPIO
/// pins.
///
/// The trick is to quickly switch continuously between each digit's display
/// value in the show operation.
class multiplexed_dual_7_segment
{
  typedef dibase::rpi::peripherals::opin    opin;
  typedef dibase::rpi::peripherals::pin_id  pin_id;

  seven_segment digit_display;
  opin          digit_select;

public:
/// @brief construct from digit select & 7 segments pin ids & on if low flag
/// The 7 segment pin ids and on if low flag are used to construct a
/// contained seven_segment member object.
///
/// The digit_sel parameter is used to indicate which digit of a 2 digit
/// display is currently selected: connect so that LOW selects right most, and
/// HIGH: left most display digit for usual digit ordering.
///
/// @param digit_sel      Digit select GPIO output pin id
/// @param segA           Id of GPIO output pin connected to segment A
/// @param segB           Id of GPIO output pin connected to segment B
/// @param segC           Id of GPIO output pin connected to segment C
/// @param segD           Id of GPIO output pin connected to segment D
/// @param segE           Id of GPIO output pin connected to segment E
/// @param segF           Id of GPIO output pin connected to segment F
/// @param segG           Id of GPIO output pin connected to segment G
/// @param is_on_when_low If true then segments ON when GPIO pins are LOW
  multiplexed_dual_7_segment
  ( pin_id digit_sel
  , pin_id segA
  , pin_id segB
  , pin_id segC
  , pin_id segD
  , pin_id segE
  , pin_id segF
  , pin_id segG
  , bool is_on_when_low
  )
  : digit_display{segA, segB, segC, segD, segE, segF, segG, is_on_when_low}
  , digit_select{digit_sel}
  {
      digit_display.clear();
  }
  multiplexed_dual_7_segment(multiplexed_dual_7_segment const&) = delete;
  multiplexed_dual_7_segment(multiplexed_dual_7_segment &&) = delete;
  multiplexed_dual_7_segment& operator=(multiplexed_dual_7_segment const&) = delete;
  multiplexed_dual_7_segment& operator=(multiplexed_dual_7_segment &&) = delete;

/// @brief Destroy: set digit_select pin LOW
  ~multiplexed_dual_7_segment()
  {
      digit_select.put(false);
  }

/// @brief Show decimal least significant 2 digits of value on dual display
/// Will display each digit in turn up to the specified maximum time, or once 
/// for each digit if max_time_ms is 0.
/// @param value        Value to show least 2 significant digits of on display
/// @param max_time_ms  Maximum time to take displaying values before returning
///                     in milliseconds.
  void show(unsigned int value, unsigned int max_time_ms=0);
};

using namespace dibase::rpi::peripherals;

bool g_running{ true };  ///< Global flag used to communicate quit request

void seven_segment::show(unsigned digit)
{
  typedef unsigned int display_type; 
  static display_type const digit_to_seven_segment_value[] =
    { /* 0: segments  ABCDEF */ 63U
    , /* 1: segments      DE */ 12U
    , /* 2: segments   ABDEG */ 91U
    , /* 3: segments   ABCDG */ 79U
    , /* 4: segments    CDFG */ 108U
    , /* 5: segments   ABCFG */ 103U
    , /* 6: segments  ABCEFG */ 119U
    , /* 7: segments     ACD */ 13U
    , /* 8: segments ABCDEFG */ 127U
    , /* 9: segments   ABDEG */ 111U
    };
  unsigned const number_of_segments{7U};
  display_type const all_segments_mask{(1U<<number_of_segments)-1U};
  unsigned const number_base{10U};
  if ( digit >= number_base )
    {
      digit %= number_base;
    }
  auto display_value = digit_to_seven_segment_value[digit];
  if ( active_low )
   {
      display_value = (~display_value)&all_segments_mask;
   }
  for (std::size_t i=0U;i<number_of_segments;++i)
    {
      display_type bit_i{1U<<i};
      segments[i]->put(display_value&bit_i);
    }
}

void seven_segment::clear()
{
  for (auto seg : segments)
    seg->put(active_low);
}

void multiplexed_dual_7_segment::show(unsigned value, unsigned max_time_ms)
{
  static unsigned const digit_display_ms{15};
  for (unsigned int i=0; i<max_time_ms; i+=2*digit_display_ms)
    {
      digit_select.put(false);
      digit_display.show(value);
      std::this_thread::sleep_for(std::chrono::milliseconds(digit_display_ms));
      digit_select.put(true);
      digit_display.show(value/10);
      std::this_thread::sleep_for(std::chrono::milliseconds(digit_display_ms));
    }
}

/// @brief Count switch presses & display on console and dual 7-segment display
///
/// Thread function spawned from main. Repeats tasks until g_running detected
/// as being false whereupon the function returns, terminating the thread.
///
/// Also terminates it an exception is thrown, writing a message to std::cerr
/// in the process.
///
/// Repeatedly waits for switch press-release, displaying current count on
/// 7-segment display while waiting. Appropriate delays are used to de-bounce
/// switch state changes while still being responsive. Upon receiving a switch
/// press-release the count is incremented and displayed on the console before
/// returning to waiting for switch activity or thread end request.
///
/// Requires: 8 GPIO lines for 7-segment display output (1 for each segment + 1
/// for digit selection) and 1 GPIO input to read switch state.
void count_switch_presses()
{
  try
    {
      multiplexed_dual_7_segment two_digit_display
                      { spi_miso  // digit sel : Gertboard J2 GP9
                      , gpio_gen6 // segment A : Gertboard J2 GP25
                      , gpio_gen5 // segment B : Gertboard J2 GP24
                      , gpio_gen4 // segment C : Gertboard J2 GP23
                      , gpio_gen3 // segment D : Gertboard J2 GP22
                      , rxd       // segment E : Gertboard J2 GP15
                      , txd       // segment F : Gertboard J2 GP14
                      , spi_ce1_n // segment G : Gertboard J2 GP7
                      , true      // Segments ON when outputs are LOW
                      };
      ipin switch0{gpio_gen2};  // Gertboard J2 GP21

      unsigned const switch_delay_ms{60};
      
      unsigned count{0};
      while (g_running) 
        {
          while (!switch0.get() && g_running)
            {
              two_digit_display.show(count, switch_delay_ms);
            }
          while (switch0.get() && g_running)
            {
              two_digit_display.show(count, switch_delay_ms);
            }
          if (g_running)
            {
              ++count;
              std::cout << "Count: " << count << std::endl;
            }
        }
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
  std::thread counter{ count_switch_presses };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  counter.join();
}
