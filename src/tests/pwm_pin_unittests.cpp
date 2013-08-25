// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin_unittests.cpp 
/// @brief Unit tests for pwm_pin and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "pwm_pin.h"
//#include "pin.h"
//#include "periexcept.h"

using namespace dibase::rpi::peripherals;

TEST_CASE( "Unit-tests/pwm_pin/0000/create pwm_ratio specialisation instance"
         , "Creates pwm_ratio specialisation and checks its count, num & den "
           "are as expected"
         )
{
  pwm_ratio<unsigned, std::ratio<3,1201>> pr{12};
  CHECK(pr.count()==12);
  auto numerator(pr.num);
  CHECK(numerator==3);
  auto denominator(pr.den);
  CHECK(denominator==1201);
 }

TEST_CASE( "Unit-tests/pwm_pin/0010/create pwm_tenths instance"
         , "Creates pwm_tenths and checks its count, num & den are as expected"
         )
{
  pwm_tenths pt{8};
  CHECK(pt.count()==8);
  auto numerator(pt.num);
  CHECK(numerator==1);
  auto denominator(pt.den);
  CHECK(denominator==10);
}

TEST_CASE( "Unit-tests/pwm_pin/0020/create pwm_hundredths instance"
         , "Creates pwm_hundredths and checks its count, num & den are as "
           "expected"
         )
{
  pwm_hundredths ph{8};
  CHECK(ph.count()==8);
  auto numerator(ph.num);
  CHECK(numerator==1);
  auto denominator(ph.den);
  CHECK(denominator==100);
}

TEST_CASE( "Unit-tests/pwm_pin/0030/create pwm_thousandths instance"
         , "Creates pwm_thousandths and checks its count, num & den are as "
           "expected"
         )
{
  pwm_thousandths pt{8};
  CHECK(pt.count()==8);
  auto numerator(pt.num);
  CHECK(numerator==1);
  auto denominator(pt.den);
  CHECK(denominator==1000);
}

TEST_CASE( "Unit-tests/pwm_pin/0040/create millionths instance"
         , "Creates pwm_millionths and checks its count, num & den are as "
           "expected"
         )
{
  pwm_millionths pm{8};
  CHECK(pm.count()==8);
  auto numerator(pm.num);
  CHECK(numerator==1);
  auto denominator(pm.den);
  CHECK(denominator==1000000);
}
