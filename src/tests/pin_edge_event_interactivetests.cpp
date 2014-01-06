// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event_interactivetests.cpp 
/// @brief System tests for pin edge event types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_edge_event.h"
#include "gpio_ctrl.h"
#include "sysfs.h"
#include "periexcept.h"
#include "interactivetests_config.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  auto const short_wait_time(std::chrono::milliseconds(100U));
  auto const long_wait_time(std::chrono::seconds(5U));
  auto const wait_timeout_error_time(std::chrono::milliseconds(20));
}

TEST_CASE( "Interactive_tests/pin_edge_event/0000/wait waits for event"
         , "wait will wait indefinitely for a pin edge event"
         )
{
  std::cout << "pin_edge_event: Wait indefinitely for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  ipin in_pin(test::in_pin_id);
  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::rising);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "Set BCM2835 GPIO" << test::in_pin_id << " low then high... " 
            << std::endl;
  pin_evt.wait();
  CHECK(pin_evt.signalled()); 
}

TEST_CASE( "Interactive_tests/pin_edge_event/0010/wait waits for time for event"
         , "wait will wait for a pin edge event for up to a given duration"
         )
{
  std::cout << "\n\npin_edge_event: Wait for a duration for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  ipin in_pin(test::in_pin_id);
  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::falling);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "Waiting 100ms for time-out...\n" 
               "If execution blocked try setting BCM2835 GPIO"
            << test::in_pin_id << " high then low...\n" << std::endl;
  auto start_time(std::chrono::system_clock::now());
  auto requested_wait_time(short_wait_time);
  CHECK_FALSE(pin_evt.wait_for(requested_wait_time));
  auto actual_wait_time(std::chrono::system_clock::now()-start_time);
  CHECK(actual_wait_time>=requested_wait_time);
  CHECK(actual_wait_time<requested_wait_time+wait_timeout_error_time);
  
  CHECK_FALSE(pin_evt.signalled());
  std::cout << "Set BCM2835 GPIO" << test::in_pin_id << " high then low "
               "within 5 seconds... " << std::endl;
  CHECK(pin_evt.wait_for(long_wait_time));
  CHECK(pin_evt.signalled()); 
}

TEST_CASE( "Interactive_tests/pin_edge_event/0020/wait waits until time event"
         , "wait will wait for a pin edge event until a given time point"
         )
{
  std::cout << "\n\npin_edge_event: Wait until a time point for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  ipin in_pin(test::in_pin_id);
  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin, pin_edge_event::both);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "Waiting until now+100ms for time-out...\n" 
               "If execution blocked try changing BCM2835 GPIO"
            << test::in_pin_id << " state...\n" << std::endl;
  auto start_time(std::chrono::system_clock::now());
  auto requested_wait_timepoint(start_time+short_wait_time);
  CHECK_FALSE(pin_evt.wait_until(requested_wait_timepoint));
  auto actual_wait_timepoint(std::chrono::system_clock::now());
  CHECK(actual_wait_timepoint>=requested_wait_timepoint);
  CHECK(actual_wait_timepoint<requested_wait_timepoint+wait_timeout_error_time);
  
  CHECK_FALSE(pin_evt.signalled());
  std::cout << "Change BCM2835 GPIO" << test::in_pin_id << " state "
               "within 5 seconds... " << std::endl;
  CHECK(pin_evt.wait_for(long_wait_time));
  CHECK(pin_evt.signalled());

// Double check signalled state is sticky!
  in_pin.get();
  CHECK(pin_evt.signalled());
  pin_evt.clear();
  CHECK_FALSE(pin_evt.signalled());
}

bool g_running{true};
std::atomic<bool> g_state{true};
void toggle_test_out_pin()
{
  try
  {
    constexpr auto t_pause(std::chrono::milliseconds{150});  
    opin out_pin(test::out_pin_id);
    bool state(false);
    while ( g_running )
      {
        out_pin.put(state);
        g_state.exchange(state);
        state = !state;
        std::this_thread::sleep_for( t_pause );
      }
  }
  catch (std::exception const & e)
  {
    std::cerr << "Exception thrown in toggle_test_out_pin: " << e.what() << "\n";
  }
}

TEST_CASE( "Interactive_tests/pin_edge_event/0100/wait rising"
         , "For rising edge event notifications only, wait only returns "
           "for low to high state changes (rising edges)"
         )
{
  g_running = true;
  std::thread test_output_generator{ toggle_test_out_pin };
  std::cout << "pin_edge_event: Wait indefinitely for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to pin BCM2835 GPIO" << test::out_pin_id
            << " to allow automatic rising edge detection.\n\n";
  ipin in_pin(test::in_pin_id);

  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::rising);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "GPREN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpren.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "GPFEN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpfen.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "Toggle BCM2835 GPIO" << test::in_pin_id << " state to count 5 "
               "rising edge state changes\nPress enter to begin...";
  std::string dummy;
  std::getline(std::cin, dummy);
  while (g_state.load()) 
    {
      std::this_thread::yield();
    }
  for ( int i=1; i<6; ++i)
    {
      pin_evt.clear();
      REQUIRE_FALSE(pin_evt.signalled());
      pin_evt.wait();
      std::cout << i << " rising edge (ipin=" 
                << in_pin.get() << ", opin=" << g_state.load() << ")\n";
    }
  CHECK(pin_evt.signalled());
  g_running = false;
  test_output_generator.join();
}

TEST_CASE( "Interactive_tests/pin_edge_event/0110/wait falling"
         , "For falling edge event notifications only, wait only returns "
           "for high to low state changes (falling edges)"
         )
{
  g_running = true;
  std::thread test_output_generator{ toggle_test_out_pin };
  std::cout << "pin_edge_event: Wait indefinitely for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to pin BCM2835 GPIO" << test::out_pin_id
            << " to allow automatic falling edge detection.\n\n";
  ipin in_pin(test::in_pin_id);

  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::falling);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "GPREN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpren.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "GPFEN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpfen.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "Toggle BCM2835 GPIO" << test::in_pin_id << " state to count 5 "
               "falling edge state changes\nPress enter to begin...";
  std::string dummy;
  std::getline(std::cin, dummy);
  while (g_state.load()) 
    {
      std::this_thread::yield();
    }
  for ( int i=1; i<6; ++i)
    {
      pin_evt.clear();
      REQUIRE_FALSE(pin_evt.signalled());
      pin_evt.wait();
      std::cout << i << " falling edge (ipin=" 
                << in_pin.get() << ", opin=" << g_state.load() << ")\n";
    }
  CHECK(pin_evt.signalled());
  g_running = false;
  test_output_generator.join();
}


TEST_CASE( "Interactive_tests/pin_edge_event/0120/wait both"
         , "For either edge event notifications only, wait returns "
           "for any state transition"
         )
{
  g_running = true;
  std::thread test_output_generator{ toggle_test_out_pin };
  std::cout << "pin_edge_event: Wait indefinitely for an edge event:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to pin BCM2835 GPIO" << test::out_pin_id
            << " to allow automatic any edge state change detection.\n\n";
  ipin in_pin(test::in_pin_id);


  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::both);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "GPREN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpren.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "GPFEN:"<< test::in_pin_id << " = " << (internal::gpio_ctrl::instance().regs->gpfen.get_bit(test::in_pin_id)!=0U) << std::endl;
  std::cout << "Toggle BCM2835 GPIO" << test::in_pin_id << " state to count 5 "
               "edge state changes\nPress enter to begin...";
  std::string dummy;
  std::getline(std::cin, dummy);
  while (g_state.load()) 
    {
      std::this_thread::yield();
    }
  for ( int i=1; i<6; ++i)
    {
      pin_evt.clear();
      REQUIRE_FALSE(pin_evt.signalled());
      pin_evt.wait();
      std::cout << i << " edge state transition (ipin=" 
                << in_pin.get() << ", opin=" << g_state.load() << ")\n";
    }
  CHECK(pin_evt.signalled());
  g_running = false;
  test_output_generator.join();
}



