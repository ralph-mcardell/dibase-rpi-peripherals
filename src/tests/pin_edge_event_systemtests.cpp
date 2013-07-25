// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event_systemtests.cpp 
/// @brief System tests for pin edge event types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "pin_edge_event.h"
#include "sysfs.h"
#include "pinexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

pin_id const available_pin_id{21}; // P1 pin GPIO_GEN2

TEST_CASE( "System_tests/pin_edge_event/000/create from unexported pin fails"
         , "Creating a pin_edge_event from a pin that is unexported throws"
         )
{
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);

  REQUIRE_THROWS_AS(pin_edge_event(available_pin_id,pin_edge_event::rising)
                   ,bad_pin_export_state);
}

TEST_CASE( "System_tests/pin_edge_event/010/create from closed ipin fails"
         , "Creating a pin_edge_event from an ipin that is closed throws"
         )
{
  REQUIRE_THROWS_AS(pin_edge_event(ipin(),pin_edge_event::rising)
                   ,std::invalid_argument);
}

TEST_CASE( "System_tests/pin_edge_event/020/create with bad edge mode fails"
         , "Creating a pin_edge_event with an bad edge event mode value throws"
         )
{
  export_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==true);
  REQUIRE_THROWS_AS(pin_edge_event(available_pin_id,pin_edge_event::edge_mode(1232))
                   ,std::invalid_argument);
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);
  {
    ipin open_ipin{available_pin_id};
    REQUIRE(open_ipin.is_open()==true);
    REQUIRE(is_exported(available_pin_id)==true);
    REQUIRE_THROWS_AS(pin_edge_event(open_ipin,pin_edge_event::edge_mode(1232))
                     ,std::invalid_argument);
  }
  CHECK(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/pin_edge_event/030/pin_id create cleans up on destruction"
         , "Sucessfully creating from a pin_id cleans up on destruction"
         )
{
  export_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==true);
  int pin_evt_fd{::dup(0)};
  REQUIRE(::close(pin_evt_fd)==0);
  {
    pin_edge_event pin_evt(available_pin_id,pin_edge_event::rising);
    int next_fd{::dup(0)};
    CHECK(next_fd==pin_evt_fd+1);
    REQUIRE(::close(next_fd)==0);
  }
  int post_evt_close_next_fd{::dup(0)};
  CHECK(post_evt_close_next_fd==pin_evt_fd);
  REQUIRE(::close(post_evt_close_next_fd)==0);
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/pin_edge_event/040/ipin create cleans up on destruction"
         , "Sucessfully creating from an ipin cleans up on destruction"
         )
{
  int pin_evt_fd{::dup(0)};
  REQUIRE(::close(pin_evt_fd)==0);
  {
    ipin open_ipin{available_pin_id};
    REQUIRE(open_ipin.is_open()==true);
    pin_edge_event pin_evt(open_ipin,pin_edge_event::rising);
    int next_fd{::dup(0)};
    CHECK(next_fd==pin_evt_fd+1);
    REQUIRE(::close(next_fd)==0);
  }
  int post_evt_close_next_fd{::dup(0)};
  CHECK(post_evt_close_next_fd==pin_evt_fd);
  REQUIRE(::close(post_evt_close_next_fd)==0);
}

TEST_CASE( "System_tests/pin_edge_event/050/pin_id: only 1 mode per pin id at a time"
         , "Can only have one non-none mode for a given GPIO pin at a time"
         )
{
  {
    ipin open_ipin{available_pin_id};
    REQUIRE(open_ipin.is_open()==true);
    REQUIRE(is_exported(available_pin_id)==true);
    pin_edge_event pin_evt(available_pin_id,pin_edge_event::rising);
    REQUIRE_THROWS_AS(pin_edge_event(available_pin_id,pin_edge_event::falling)
                     ,bad_pin_edge_event);
    REQUIRE_THROWS_AS(pin_edge_event(open_ipin,pin_edge_event::both)
                     ,bad_pin_edge_event);
    pin_edge_event pin_evt2(available_pin_id,pin_edge_event::rising);
  }
  ipin open_ipin{available_pin_id};
  REQUIRE(open_ipin.is_open()==true);
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(available_pin_id,pin_edge_event::falling);
}

TEST_CASE( "System_tests/pin_edge_event/060/ipin: only 1 mode per pin id at a time"
         , "Can only have one non-none mode for a given GPIO pin at a time"
         )
{
  {
    ipin open_ipin{available_pin_id};
    REQUIRE(open_ipin.is_open()==true);
    REQUIRE(is_exported(available_pin_id)==true);
    pin_edge_event pin_evt(open_ipin,pin_edge_event::rising);
    REQUIRE_THROWS_AS(pin_edge_event(available_pin_id,pin_edge_event::falling)
                     ,bad_pin_edge_event);
    REQUIRE_THROWS_AS(pin_edge_event(open_ipin,pin_edge_event::both)
                     ,bad_pin_edge_event);
    pin_edge_event pin_evt2(open_ipin,pin_edge_event::rising);
  }
  ipin open_ipin{available_pin_id};
  REQUIRE(open_ipin.is_open()==true);
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(open_ipin,pin_edge_event::both);
}

TEST_CASE( "System_tests/pin_edge_event/070/initially signalled"
         , "Creating a pin_edge_event is initially signalled"
         )
{
  export_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(available_pin_id,pin_edge_event::rising);
  CHECK(pin_evt.signalled());
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/pin_edge_event/080/not signalled after cleared"
         , "Clearing signalled pin_edge_event is not longer signalled"
         )
{
  export_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==true);
  pin_edge_event pin_evt(available_pin_id,pin_edge_event::rising);
  REQUIRE(pin_evt.signalled());
  pin_evt.clear();
  CHECK_FALSE(pin_evt.signalled());
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);
}
