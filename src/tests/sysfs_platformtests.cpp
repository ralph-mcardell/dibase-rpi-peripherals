// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs_platformtests.cpp 
/// @brief System tests for sys filesystem utilities.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "sysfs.h"
#include <stdio.h>
#include <unistd.h>
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

// Change if P1 GPIO_GEN0 is in use on your system...
pin_id const available_pin_id_1d{7}; // P1 pin 26, SPI_CE1_N, 1 digit value
pin_id const available_pin_id_2d{17}; // P1 pin 11, GPIO_GEN0, 2 digit value

TEST_CASE( "Platform_tests/0000/sysfs/is_exported_initially_reports_free"
         , "The available_pin_id is initially not exported"
         )
{
  REQUIRE(is_exported(available_pin_id_1d)==false);
  REQUIRE(is_exported(available_pin_id_2d)==false);
}

TEST_CASE( "Platform_tests/0010/sysfs/export_pin_exported_unexported_not"
         , "Exported pin is exported, then when unexported it is not exported."
         )
{
  export_pin(available_pin_id_1d);
  CHECK(is_exported(available_pin_id_1d)==true);
  unexport_pin(available_pin_id_1d);
  CHECK(is_exported(available_pin_id_1d)==false);

  export_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==true);
  unexport_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==false);
}

TEST_CASE( "Platform_tests/0020/sysfs/exporting_exported_pin_OK"
         , "Re-exporting a pin supported - check exported  state with "
           "is_exported if this is not what is required."
         )
{
  export_pin(available_pin_id_1d);
  CHECK(is_exported(available_pin_id_1d)==true);
  CHECK(export_pin(available_pin_id_1d)==true);
  CHECK(is_exported(available_pin_id_1d)==true);
  unexport_pin(available_pin_id_1d);
  CHECK(is_exported(available_pin_id_1d)==false);

  export_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==true);
  CHECK(export_pin(available_pin_id_2d)==true);
  CHECK(is_exported(available_pin_id_2d)==true);
  unexport_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==false);
}

TEST_CASE( "Platform_tests/0030/sysfs/unexporting an unexported pin OK"
         , "A pin that is not exported can be unexported - use is_exported to"
           " check exported state if this is not what is required"
         )
{
  REQUIRE(is_exported(available_pin_id_1d)==false);
  CHECK(unexport_pin(available_pin_id_1d)==true);
  CHECK(is_exported(available_pin_id_1d)==false);

  REQUIRE(is_exported(available_pin_id_2d)==false);
  CHECK(unexport_pin(available_pin_id_2d)==true);
  CHECK(is_exported(available_pin_id_2d)==false);
}

TEST_CASE( "Platform_tests/0040/sysfs/opening unexported pin for events fails"
         , "A pin must be exported before it can be opened for edge event IO "
           "multiplexing use."
         )
{
  REQUIRE(is_exported(available_pin_id_1d)==false);
  REQUIRE_THROWS_AS( open_ipin_for_edge_events
                      (available_pin_id_1d, edge_event_mode::both)
                   , std::runtime_error
                   );

  REQUIRE(is_exported(available_pin_id_2d)==false);
  REQUIRE_THROWS_AS( open_ipin_for_edge_events
                      (available_pin_id_2d, edge_event_mode::both)
                   , std::runtime_error
                   );
}

TEST_CASE( "Platform_tests/0050/sysfs/opening pin for events with bad mode fails"
         , "Passing a bad edge_event_mode value to open_ipin_for_edge_events "
           "fails"
         )
{
  export_pin(available_pin_id_1d);
  REQUIRE(is_exported(available_pin_id_1d)==true);
  REQUIRE_THROWS_AS( open_ipin_for_edge_events
                      (available_pin_id_1d, static_cast<edge_event_mode>(1234))
                   , std::invalid_argument
                   );
  unexport_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==false);

  export_pin(available_pin_id_2d);
  REQUIRE(is_exported(available_pin_id_2d)==true);
  REQUIRE_THROWS_AS( open_ipin_for_edge_events
                      (available_pin_id_2d, static_cast<edge_event_mode>(1234))
                   , std::invalid_argument
                   );
  unexport_pin(available_pin_id_2d);
  CHECK(is_exported(available_pin_id_2d)==false);
}

TEST_CASE( "Platform_tests/0060/sysfs/closing non-open pin for events fd fails"
         , "Closing a pin for edge event file descriptor that is not open fails"
         )
{
  CHECK(close_ipin_for_edge_events(-1)==false);
  FILE* ftmp{tmpfile()};
  REQUIRE(ftmp!=0);
  auto fd(fileno(ftmp));
  REQUIRE(fd>STDERR_FILENO);
  REQUIRE(fclose(ftmp)==0);
  CHECK(close_ipin_for_edge_events(fd)==false);
}

TEST_CASE( "Platform_tests/0070/sysfs/open/close exported pin for events OK"
         , "Passing a valid argument values to open_ipin_for_edge_events "
           "is successful"
         )
{
  export_pin(available_pin_id_1d);
  REQUIRE(is_exported(available_pin_id_1d)==true);

  {
    auto fd(open_ipin_for_edge_events(available_pin_id_1d, edge_event_mode::both));
    CHECK(fd!=-1);
    CHECK(close_ipin_for_edge_events(fd)==true);
    unexport_pin(available_pin_id_1d);
    CHECK(is_exported(available_pin_id_1d)==false);
  }
  export_pin(available_pin_id_2d);
  REQUIRE(is_exported(available_pin_id_2d)==true);
  {
    auto fd(open_ipin_for_edge_events(available_pin_id_2d, edge_event_mode::both));
    CHECK(fd!=-1);
    CHECK(close_ipin_for_edge_events(fd)==true);
    unexport_pin(available_pin_id_2d);
    CHECK(is_exported(available_pin_id_2d)==false);
  }
}

TEST_CASE( "Platform_tests/0080/sysfs/repeat open same pin OK if edge event mode same"
         , "Can open same pin_id more than once IFF edge event mode is the same"
         )
{
  export_pin(available_pin_id_1d);
  REQUIRE(is_exported(available_pin_id_1d)==true);
  {
    auto fd(open_ipin_for_edge_events(available_pin_id_1d, edge_event_mode::rising));
    CHECK(fd!=-1);
    auto fd2(open_ipin_for_edge_events(available_pin_id_1d, edge_event_mode::rising));
    CHECK(fd2!=-1);
    CHECK(close_ipin_for_edge_events(fd)==true);
    CHECK(close_ipin_for_edge_events(fd2)==true);
    unexport_pin(available_pin_id_1d);
    CHECK(is_exported(available_pin_id_1d)==false);
  }
  export_pin(available_pin_id_2d);
  REQUIRE(is_exported(available_pin_id_2d)==true);
  {
    auto fd(open_ipin_for_edge_events(available_pin_id_2d, edge_event_mode::rising));
    CHECK(fd!=-1);
    auto fd2(open_ipin_for_edge_events(available_pin_id_2d, edge_event_mode::rising));
    CHECK(fd2!=-1);
    CHECK(close_ipin_for_edge_events(fd)==true);
    CHECK(close_ipin_for_edge_events(fd2)==true);
    unexport_pin(available_pin_id_2d);
    CHECK(is_exported(available_pin_id_2d)==false);
  }
}
