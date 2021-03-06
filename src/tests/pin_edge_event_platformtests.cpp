// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event_platformtests.cpp 
/// @brief System tests for pin edge event types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_edge_event.h"
#include "sysfs.h"
#include <unistd.h>
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

pin_id const available_pin_id{21}; // P1 pin GPIO_GEN2

TEST_CASE( "Platform_tests/pin_edge_event/000/create from unexported pin fails"
         , "Creating a pin_edge_event from a pin that is unexported throws"
         )
{
  ipin in_pin{available_pin_id};
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);

  REQUIRE_THROWS_AS(pin_edge_event(in_pin,pin_edge_event::rising)
                   ,bad_pin_export_state);
  export_pin(available_pin_id);
}

TEST_CASE( "Platform_tests/pin_edge_event/010/create with bad edge mode fails"
         , "Creating a pin_edge_event with an bad edge event mode value throws"
         )
{
  ipin in_pin{available_pin_id};
  REQUIRE(is_exported(available_pin_id)==true);
  REQUIRE_THROWS_AS(pin_edge_event(in_pin,pin_edge_event::edge_mode(1232))
                   ,std::invalid_argument);
}

TEST_CASE( "Platform_tests/pin_edge_event/020/ipin create cleans up on destruction"
         , "Successfully creating from an ipin cleans up on destruction"
         )
{
  int pin_evt_fd{::dup(0)};
  REQUIRE(::close(pin_evt_fd)==0);
  {
    ipin open_ipin{available_pin_id};
    REQUIRE(is_exported(available_pin_id)==true);
    pin_edge_event pin_evt(open_ipin,pin_edge_event::rising);
    int next_fd{::dup(0)};
    CHECK(next_fd==pin_evt_fd+1);
    REQUIRE(::close(next_fd)==0);
  }
  int post_evt_close_next_fd{::dup(0)};
  CHECK(post_evt_close_next_fd==pin_evt_fd);
  REQUIRE(::close(post_evt_close_next_fd)==0);
}

TEST_CASE( "Platform_tests/pin_edge_event/030/only 1 pin_edge_evet per pin at a time"
         , "Can only have one pin_edge_event object per GPIO pin at a time"
         )
{
  {
    ipin in_pin{available_pin_id};
    REQUIRE(is_exported(available_pin_id)==true);
    pin_edge_event pin_evt(in_pin,pin_edge_event::rising);
    REQUIRE_THROWS_AS(pin_edge_event(in_pin,pin_edge_event::falling)
                     ,bad_peripheral_alloc);
   }
  ipin in_pin{available_pin_id};
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::falling);
}

TEST_CASE( "Platform_tests/pin_edge_event/040/initially signalled"
         , "Creating a pin_edge_event is initially signalled"
         )
{
  ipin in_pin{available_pin_id};
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::rising);
  CHECK(pin_evt.signalled());
}

TEST_CASE( "Platform_tests/pin_edge_event/050/not signalled after cleared"
         , "Clearing signalled pin_edge_event is not longer signalled"
         )
{
  ipin in_pin{available_pin_id};
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(in_pin,pin_edge_event::rising);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  CHECK_FALSE(pin_evt.signalled());
}
