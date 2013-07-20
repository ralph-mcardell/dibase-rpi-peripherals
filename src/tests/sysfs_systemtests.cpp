// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs_systemtests.cpp 
/// @brief System tests for sys filesystem utilities.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "sysfs.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

// Change if P1 GPIO_GEN0 is in use on your system...
pin_id const available_pin_id{17}; // P1 pin GPIO_GEN0

TEST_CASE( "System_tests/000/sysfs/is_exported_initially_reports_free"
         , "The available_pin_id is initially not exported"
         )
{
  REQUIRE(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/001/sysfs/export_pin_exported_unexported_not"
         , "Exported pin is exported, then when unexported it is not exported."
         )
{
  export_pin(available_pin_id);
  CHECK(is_exported(available_pin_id)==true);
  unexport_pin(available_pin_id);
  CHECK(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/002/sysfs/exporting_exported_pin_OK"
         , "Re-exporting a pin WORKS - check is_exported to check if this is"
           " not what is required."
         )
{
  export_pin(available_pin_id);
  CHECK(is_exported(available_pin_id)==true);
  CHECK(export_pin(available_pin_id)==true);
  CHECK(is_exported(available_pin_id)==true);
  unexport_pin(available_pin_id);
  CHECK(is_exported(available_pin_id)==false);
}

TEST_CASE( "System_tests/003/sysfs/unexporting an unexported pin OK"
         , "Unexporting a pin that is not exported OK - use is_exported to"
           " check if this is not what is required"
         )
{
  REQUIRE(is_exported(available_pin_id)==false);
  CHECK(unexport_pin(available_pin_id)==true);
  CHECK(is_exported(available_pin_id)==false);
}
