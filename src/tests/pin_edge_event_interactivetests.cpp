// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event_interactivetests.cpp 
/// @brief System tests for pin edge event types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_edge_event.h"
#include "sysfs.h"
#include "pinexcept.h"
#include "interactivetests_config.h"
#include <iostream>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  auto short_wait_time(std::chrono::milliseconds(100U));
  auto long_wait_time(std::chrono::seconds(5U));
  auto wait_timeout_error_time(std::chrono::milliseconds(20));
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
  REQUIRE(in_pin.is_open());
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
  REQUIRE(in_pin.is_open());
  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::falling);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "Waiting 100ms for time-out...\n" 
               "If execution blocked try setting BCM2835 GPIO"
            << test::in_pin_id << " high then low... " << std::endl;
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
  REQUIRE(in_pin.is_open());
  REQUIRE(is_exported(test::in_pin_id)==true);
  pin_edge_event pin_evt(in_pin, pin_edge_event::both);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  REQUIRE_FALSE(pin_evt.signalled());
  std::cout << "Waiting until now+100ms for time-out...\n" 
               "If execution blocked try changing BCM2835 GPIO"
            << test::in_pin_id << " state... " << std::endl;
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