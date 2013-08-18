// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin_unittests.cpp 
/// @brief Unit tests for clock_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_pin.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Unit-tests/frequency/0010/default construct"
         , "Default constructed frequency objects possible & have zero counts"
         )
{
  CHECK((frequency<float, std::mega>{}.count())==0.0f);
  CHECK((frequency<unsigned short, std::kilo>{}.count())==0U);
  CHECK((megahertz{}.count())==0U);
  CHECK((f_megahertz{}.count())==0.0);
  CHECK((kilohertz{}.count())==0U);
  CHECK((f_kilohertz{}.count())==0.0);
  CHECK((hertz{}.count())==0U);
}

TEST_CASE( "Unit-tests/frequency/0010/create_misc_scale_and_rep_frequencies"
         , "Objects of various frequency types can be created from count value"
         )
{
  frequency<float, std::mega> NineteenPointTwoMhz{19.2};
  CHECK(NineteenPointTwoMhz.count()==19.2f);
  frequency<unsigned short, std::kilo> SixHundredKHz{600};
  CHECK(SixHundredKHz.count()==600U);
  frequency<unsigned> OneTwoThreeFourFiveSixSevenHz{1234567U};
  CHECK(OneTwoThreeFourFiveSixSevenHz.count()==1234567U);
  i_hertz EightHundredMillionHz{800000000};
  CHECK(EightHundredMillionHz.count()==800000000U);
  i_megahertz EightHundredMHz{800};
  CHECK(EightHundredMHz.count()==800U);
  i_kilohertz EightHundredThousandKhz{800000};
  CHECK(EightHundredThousandKhz.count()==800000U);
  f_megahertz EighteenPointThreeFourMHz{18.34f};
  CHECK(EighteenPointThreeFourMHz.count()==18.34f);
  f_kilohertz SixtySixPointSizSevenKhz{66.67};
  CHECK(SixtySixPointSizSevenKhz.count()==66.67);
  hertz NineHundredMillionHz{900000000};
  CHECK(NineHundredMillionHz.count()==900000000U);
  megahertz NineHundredMHz{900};
  CHECK(NineHundredMHz.count()==900U);
  kilohertz NineHundredThousandKhz{900000};
  CHECK(NineHundredThousandKhz.count()==900000U);
}

TEST_CASE( "Unit-tests/frequency/0020/create fails for inappropriate ctor values"
         , "Compile will fail if built with COMPILE_FAIL_TESTS #defined for "
           "two objects constructed from inappropriate values"
         )
{
  f_kilohertz flt_KHzFrom_flt{1.1f};  // Compiles OK
  f_kilohertz flt_KHzFrom_dbl{1.1};   // Compiles OK
  f_kilohertz flt_KHzFrom_int{100};   // Compiles OK
  f_kilohertz flt_KHzFrom_uint{10U};  // Compiles OK
  f_kilohertz flt_KHzFrom_lng{100L};  // Compiles OK
  f_kilohertz flt_KHzFrom_ulng{5UL};  // Compiles OK
  f_kilohertz flt_KHzFrom_llng{10LL}; // Compiles OK
  f_kilohertz flt_KHzFrom_ullng{5ULL};// Compiles OK

  i_kilohertz int_KHzFrom_int{100};   // Compiles OK
  i_kilohertz int_KHzFrom_uint{10U};  // Compiles OK
  i_kilohertz int_KHzFrom_lng{100L};  // Compiles OK
  i_kilohertz int_KHzFrom_ulng{5UL};  // Compiles OK
  i_kilohertz int_KHzFrom_llng{10LL}; // Compiles OK
  i_kilohertz int_KHzFrom_ullng{5ULL};// Compiles OK

#ifdef COMPILE_FAIL_TESTS
  
  i_kilohertz int_KHzFrom_flt = 1.1f;  // Compile FAIL
  i_kilohertz int_KHzFrom_dbl = 1.1;   // Compile FAIL

#endif
}

TEST_CASE( "Unit-tests/frequency/0030/conversions"
         , "frequency_cast can convert between frequencies of different types"
         )
{
  CHECK(frequency_cast<hertz>(kilohertz(600)).count()==600000);
  CHECK(frequency_cast<kilohertz>(megahertz(25)).count()==25000);
  CHECK(frequency_cast<kilohertz>(f_megahertz(19.2)).count()==19200);
  CHECK(frequency_cast<hertz>(f_megahertz(800.123456)).count()==800123456);
}

TEST_CASE( "Unit-tests/frequency/0030/comparisons - same frequency type"
         , "Can compare frequencies of SAME type with == != < > <= >= "
         )
{
  CHECK( hertz(123)==hertz(123) );
  CHECK( kilohertz(123)<kilohertz(124) );
  CHECK( hertz(123)!=hertz(124) );
  CHECK( hertz(123)<=hertz(123) );
  CHECK( hertz(123)<=hertz(124) );
  CHECK( hertz(123)>hertz(122) );
  CHECK( hertz(123)>=hertz(122) );
  CHECK( hertz(123)>=hertz(123) );

  CHECK_FALSE( megahertz(123)==megahertz(122) );
  CHECK_FALSE( hertz(123)<hertz(122) );
  CHECK_FALSE( hertz(123)<hertz(123) );
  CHECK_FALSE( hertz(123)!=hertz(123) );
  CHECK_FALSE( hertz(123)<=hertz(122) );
  CHECK_FALSE( hertz(123)>hertz(124) );
  CHECK_FALSE( hertz(123)>hertz(123) );
  CHECK_FALSE( hertz(123)>=hertz(124) );
}

TEST_CASE( "Unit-tests/fixed_oscillator_clock_source/0000/create"
         , "fixed_oscillator_clock_source can be created from a frequency"
         )
{
  fixed_oscillator_clock_source osc_19_3MHz(f_megahertz(19.3));
  CHECK(osc_19_3MHz.source()==clock_source::oscillator);
  CHECK(osc_19_3MHz.frequency().count()==19300000);
}

TEST_CASE( "Unit-tests/clock_frequency/0000/create with default filter"
         , "clock_frequency can be created from just a frequency with the "
           "filter type defaulting to none."
         )
{
  clock_frequency clk_600KHz(kilohertz(600));
  CHECK(clk_600KHz.filter()==clock_filter::none);
  CHECK(clk_600KHz.average_frequency().count()==600000);
}

TEST_CASE( "Unit-tests/clock_frequency/0010/create with explicit filter"
         , "clock_frequency can be created from frequency & an explicit "
           "filter type"
         )
{
  clock_frequency clk_600KHz(f_kilohertz(18.33), clock_filter::medium);
  CHECK(clk_600KHz.filter()==clock_filter::medium);
  CHECK(clk_600KHz.average_frequency().count()==18330);
}
