// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_parameters_unittests.cpp 
/// @brief Unit tests for clock_parameters and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_parameters.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

TEST_CASE( "Unit-tests/frequency/0010/default construct"
         , ""
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
         , ""
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
         , ""
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
  
  i_kilohertz int_KHzFrom_flt{1.1f};  // Compile FAIL
  i_kilohertz int_KHzFrom_dbl{1.1};   // Compile FAIL

#endif
}

TEST_CASE( "Unit-tests/frequency/0030/conversions"
         , ""
         )
{
  CHECK(frequency_cast<hertz>(kilohertz(600)).count()==600000);
  CHECK(frequency_cast<kilohertz>(megahertz(25)).count()==25000);
  CHECK(frequency_cast<kilohertz>(f_megahertz(19.2)).count()==19200);
  CHECK(frequency_cast<hertz>(f_megahertz(800.123456)).count()==800123456);
}

TEST_CASE( "Unit-tests/frequency/0030/comparisons - same frequency type"
         , ""
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

TEST_CASE( "Unit-tests/fixed_oscillator_clock_source/0000/"
         , ""
         )
{
  fixed_oscillator_clock_source osc_19_3MHz(f_megahertz(19.3));
  CHECK(osc_19_3MHz.source()==clock_source::oscillator);
  CHECK(osc_19_3MHz.frequency().count()==19300000);
}

TEST_CASE( "Unit-tests/clock_frequency/0000/create with default filter"
         , ""
         )
{
  clock_frequency clk_600KHz(kilohertz(600));
  CHECK(clk_600KHz.filter()==clock_filter::none);
  CHECK(clk_600KHz.average_frequency().count()==600000);
}

TEST_CASE( "Unit-tests/clock_frequency/0000/create with explicit filter"
         , ""
         )
{
  clock_frequency clk_600KHz(f_kilohertz(18.33), clock_filter::medium);
  CHECK(clk_600KHz.filter()==clock_filter::medium);
  CHECK(clk_600KHz.average_frequency().count()==18330);
}

TEST_CASE( "Unit-tests/clock_parameters/0000/create no filter"
         , ""
         )
{
  fixed_oscillator_clock_source src{megahertz{650}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}}
                     };
  CHECK(cp.frequency_min().count()==18571428U);
  CHECK(cp.frequency_avg().count()==18571428U);
  CHECK(cp.frequency_max().count()==18571428U);
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::integer_division);
  CHECK(cp.clk_divi()==35U);
  CHECK(cp.clk_divf()==0U);
}

TEST_CASE( "Unit-tests/clock_parameters/0010/create minimum filter"
         , ""
         )
{
  fixed_oscillator_clock_source src{megahertz{650}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}, clock_filter::minimum}
                     };
  CHECK(cp.frequency_min().count()==18055555U);
  CHECK(cp.frequency_avg().count()==18319938U);
  CHECK(cp.frequency_max().count()==18571428U);
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::mash_1_stage);
  CHECK(cp.clk_divi()==35U);
  CHECK(cp.clk_divf()==492U);
}

TEST_CASE( "Unit-tests/clock_parameters/0020/create medium filter"
         , ""
         )
{
  fixed_oscillator_clock_source src{megahertz{650}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}, clock_filter::medium}
                     };
  CHECK(cp.frequency_min().count()==17567567U);
  CHECK(cp.frequency_avg().count()==18319938U);
  CHECK(cp.frequency_max().count()==19117647U);
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::mash_2_stage);
  CHECK(cp.clk_divi()==35U);
  CHECK(cp.clk_divf()==492U);
}

TEST_CASE( "Unit-tests/clock_parameters/0030/create maximum filter"
         , ""
         )
{
  fixed_oscillator_clock_source src{megahertz{650}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}, clock_filter::maximum}
                     };
  CHECK(cp.frequency_min().count()==16666666U);
  CHECK(cp.frequency_avg().count()==18319938U);
  CHECK(cp.frequency_max().count()==20312500U);
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::mash_3_stage);
  CHECK(cp.clk_divi()==35U);
  CHECK(cp.clk_divf()==492U);
}

TEST_CASE( "Unit-tests/clock_parameters/0030/create max freq too large "
         , "Check filter MASH stages reduced until max freq in range"
         )
{
  fixed_oscillator_clock_source src{megahertz{200}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}, clock_filter::maximum}
                     };
// 3 stage MASH would give 28.57MHz maximum frequency for these values which
// exceeds the 25MHz upper limit so the MASH level should reduce to 2 stage MASH
  CHECK(cp.frequency_min().count()==16666666U);
  CHECK(cp.frequency_avg().count()==18320064U);
  CHECK(cp.frequency_max().count()==22222222U); 
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::mash_2_stage); // reduced from 3 stage
  CHECK(cp.clk_divi()==10U);
  CHECK(cp.clk_divf()==939U);
}

TEST_CASE( "Unit-tests/clock_parameters/0040/create max freq too large 2 stages"
         , "Check filter MASH stages reduced until max freq in range"
         )
{
  fixed_oscillator_clock_source src{megahertz{90}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{f_megahertz{18.32}, clock_filter::maximum}
                     };
// 3 & 2 stage MASH would give maximum frequency for these values which
// exceeds the 25MHz upper limit so the MASH level should reduce to 1 stage MASH
  CHECK(cp.frequency_min().count()==18000000U);
  CHECK(cp.frequency_avg().count()==18318425U);
  CHECK(cp.frequency_max().count()==22500000U); 
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::mash_1_stage); // reduced from 3 stage
  CHECK(cp.clk_divi()==4U);
  CHECK(cp.clk_divf()==935U);
}

TEST_CASE( "Unit-tests/clock_parameters/0050/create max freq too high 3 stages"
         , "Check integer division (non) MASH mode selected if no filtering "
           "mode produces a valid range of filtered frequencies"
         )
{
  fixed_oscillator_clock_source src{megahertz{83}};
  clock_parameters cp{ src.source()
                     , src.frequency()
                     , clock_frequency{megahertz{24}, clock_filter::maximum}
                     };

// No MASH filtering would give maximum frequency for these values which does
// not exceed the 25MHz upper limit so the MASH level should reduce to
// integer_division
  CHECK(cp.frequency_min().count()==27666666U);
  CHECK(cp.frequency_avg().count()==27666666U);
  CHECK(cp.frequency_max().count()==27666666U); 
  CHECK(cp.clk_source()==clock_src::oscillator);
  CHECK(cp.clk_mash()==clock_mash_mode::integer_division);
  CHECK(cp.clk_divi()==3U);
  CHECK(cp.clk_divf()==0U);
}

TEST_CASE( "Unit-tests/clock_parameters/0060/create DIVI too low throws max filter"
         , "Check throws if no mode produces valid range of frequencies"
         )
{
  fixed_oscillator_clock_source src{megahertz{24}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(megahertz(6), clock_filter::maximum)
                      )
                   , std::range_error
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0070/create DIVI too low throws med filter"
         , "Check throws if no mode produces valid range of frequencies"
         )
{
  fixed_oscillator_clock_source src{megahertz{24}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(megahertz(10), clock_filter::medium)
                      )
                   , std::range_error
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0080/create DIVI too low throws min filter"
         , "Check throws if no mode produces valid range of frequencies"
         )
{
  fixed_oscillator_clock_source src{megahertz{24}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(megahertz(16), clock_filter::minimum)
                      )
                   , std::range_error
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0090/create clock freq. > max allowed, no filtered"
         , "Check throws if the requested average frequency is greater the "
           "maximum allowed (25MHz)"
         )
{
  fixed_oscillator_clock_source src{megahertz{383}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(f_megahertz(125.000001), clock_filter::none)
                      )
                   , std::invalid_argument
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0100/create clock freq. > max allowed, filtering"
         , "Check throws if the requested average frequency is greater the "
           "maximum allowed (25MHz)"
         )
{
  fixed_oscillator_clock_source src{megahertz{83}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(f_megahertz(25.000001), clock_filter::minimum)
                      )
                   , std::invalid_argument
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0110/create clock freq. > than source"
         , "Check throws if the requested average frequency is greater than "
           "that of the source"
         )
{
  fixed_oscillator_clock_source src{hertz{83}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(hertz(84), clock_filter::maximum)
                      )
                   , std::invalid_argument
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0120/create clock freq. too low"
         , "Check throws if the requested average frequency is to small a "
           "fraction of the source"
         )
{
  fixed_oscillator_clock_source src{hertz{0x1000}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(hertz(1), clock_filter::maximum)
                      )
                   , std::invalid_argument
                   );
}

TEST_CASE( "Unit-tests/clock_parameters/0130/create zero clock freq. "
         , "Check throws if the requested average frequency is to small a "
           "fraction of the source"
         )
{
  fixed_oscillator_clock_source src{hertz{0x1000}};
  REQUIRE_THROWS_AS( clock_parameters
                      ( src.source()
                      , src.frequency()
                      , clock_frequency(hertz(0), clock_filter::maximum)
                      )
                   , std::invalid_argument
                   );
}
