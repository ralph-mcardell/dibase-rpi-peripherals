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

TEST_CASE( "System_tests/pin_edge_event/030/OK create cleans up on destruction"
         , "Sucessfully creating an object cleans up on destruction"
         )
{
  export_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==true);
  {
    pin_edge_event pin_evt(available_pin_id,pin_edge_event::rising);
  }
  unexport_pin(available_pin_id);
  REQUIRE(is_exported(available_pin_id)==false);
}