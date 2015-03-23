// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_revision_unittests.cpp
/// @brief Unit tests for Raspberry Pi revision class.
///
/// @copyright Copyright (c) Dibase Limited 2012-2015
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "rpi_revision.h"

using namespace dibase::rpi;

TEST_CASE( "Unit_tests/rpi_revision/construct-from-parts"
         , "Construct a Raspberry Pi revision value from new scheme field values"
         )
{
  rpi_revision rpr( rpi_model::b
                  , rpi_processor::bcm2835
                  , rpi_ram::mb512
                  , rpi_maker::egoman
                  , 2
                  );
  CHECK(rpr.model()==rpi_model::b);
  CHECK(rpr.processor()==rpi_processor::bcm2835);
  CHECK(rpr.ram()==rpi_ram::mb512);
  CHECK(rpr.ram_MB()==512);
  CHECK(rpr.ram_B()==536870912);
  CHECK(rpr.maker()==rpi_maker::egoman);
  CHECK(rpr.version()==2);
  CHECK_FALSE(rpr.turbo());
  CHECK_FALSE(rpr.warranty_void());
  CHECK(rpr.raw_value()
        ==
        (2 + (1<<4) + (0<<12) + (1<<16) + (1<<20) + (1<<23) + (0<<24) + (0<<25))
     // ver   model  processor   maker      ram      scheme    turbo   warranty
       );
}

TEST_CASE( "Unit_tests/rpi_revision/construct-from-new-scheme-revision-value"
         , "Construct a Raspberry Pi revision value from a new scheme value"
         )
{
  std::size_t 
    rev{1 + (4<<4) + (1<<12) + (0<<16) + (2<<20) + (1<<23) + (1<<24) + (0<<25)};
    // ver   model  processor   maker      ram      scheme    turbo   warranty
  rpi_revision rpr(rev);
  CHECK(rpr.model()==rpi_model::pi_2_b);
  CHECK(rpr.processor()==rpi_processor::bcm2836);
  CHECK(rpr.ram()==rpi_ram::mb1024);
  CHECK(rpr.ram_MB()==1024);
  CHECK(rpr.ram_B()==1073741824);
  CHECK(rpr.maker()==rpi_maker::sony);
  CHECK(rpr.version()==1);
  CHECK(rpr.turbo());
  CHECK_FALSE(rpr.warranty_void());
  CHECK(rpr.raw_value() == rev);
}

TEST_CASE( "Unit_tests/rpi_revision/construct-from-old-scheme-revision-value"
         , "Construct a Raspberry Pi revision value from an old scheme value"
         )
{
  std::size_t 
    rev{ 0x10 + (1<<24)};//B+ v1, BCM2835, 512MB, made by Sony, warranty bit set
    //        turbo+warranty
  rpi_revision rpr(rev);
  CHECK(rpr.model()==rpi_model::b_plus);
  CHECK(rpr.processor()==rpi_processor::bcm2835);
  CHECK(rpr.ram()==rpi_ram::mb512);
  CHECK(rpr.ram_MB()==512);
  CHECK(rpr.ram_B()==536870912);
  CHECK(rpr.maker()==rpi_maker::sony);
  CHECK(rpr.version()==1);
  CHECK(rpr.turbo());
  CHECK(rpr.warranty_void());
  CHECK(rpr.raw_value()
        ==
        (1 + (3<<4) + (0<<12) + (0<<16) + (1<<20) + (1<<23) + (1<<24) + (1<<25))
     // ver   model  processor   maker      ram      scheme    turbo   warranty
       );
}
