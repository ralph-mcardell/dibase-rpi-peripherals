// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_pins_platformtests.cpp 
/// @brief Platform tests for i2c_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "i2c_pins.h"
#include "i2c_ctrl.h"
#include "gpio_ctrl.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

// Using class static const/constexpr values directly with Catch CHECK/REQUIRE
// etc. causes them to need to be defined - which they are not so ld complains!
constexpr auto default_tout(i2c_pins::default_tout);
constexpr auto default_fedl(i2c_pins::default_fedl);
constexpr auto default_redl(i2c_pins::default_redl);

TEST_CASE( "Platform-tests/i2c_pins/0000/create & destroy good, implied BSC peripheral"
         , "Creating i2c_pins from a good set of parameters, with BSC0|1 "
           "implied by pid ids, leaves object in the expected state"
         )
{
  {
    i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
    CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
    CHECK(i2c_ctrl::instance().regs(0)->get_clock_stretch_timeout()==default_tout);
    CHECK(i2c_ctrl::instance().regs(0)->get_write_delay()==default_fedl);
    CHECK(i2c_ctrl::instance().regs(0)->get_read_delay()==default_redl);
    CHECK((hertz(rpi_apb_core_frequency).count()/hertz(i2c_pins_default_frequency).count())==2500U);
    CHECK(i2c_ctrl::instance().regs(0)->get_clock_divider()==2500U);
    CHECK(i2c_ctrl::instance().regs(0)->get_enable());
    CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_transfer_active());
    CHECK(i2c_ctrl::instance().regs(0)->get_tx_fifo_empty());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  {
    i2c_pins iic(pin_id(2),pin_id(3)); // SDA1, SCL1 => BSC1
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(2)));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(3)));
    CHECK(i2c_ctrl::instance().alloc.is_in_use(1));
    CHECK(i2c_ctrl::instance().regs(1)->get_clock_stretch_timeout()==default_tout);
    CHECK(i2c_ctrl::instance().regs(1)->get_write_delay()==default_fedl);
    CHECK(i2c_ctrl::instance().regs(1)->get_read_delay()==default_redl);
    CHECK((hertz(rpi_apb_core_frequency).count()/hertz(i2c_pins_default_frequency).count())==2500U);
    CHECK(i2c_ctrl::instance().regs(1)->get_clock_divider()==2500U);
    CHECK(i2c_ctrl::instance().regs(1)->get_enable());
    CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_transfer_active());
    CHECK(i2c_ctrl::instance().regs(1)->get_tx_fifo_empty());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(2)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(3)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(1));
  CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0010/create & destroy good, explicit BSC peripheral"
         , "Creating i2c_pins from a good set of parameters, with BSC0|1 "
           "explicitly passed leaves object in the expected state"
         )
{
  {
    i2c_pins iic(pin_id(0),pin_id(1),0); // SDA0,SCL0 -- explicitly specify BSC0
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
    CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
    CHECK(i2c_ctrl::instance().regs(0)->get_clock_stretch_timeout()==default_tout);
    CHECK(i2c_ctrl::instance().regs(0)->get_write_delay()==default_fedl);
    CHECK(i2c_ctrl::instance().regs(0)->get_read_delay()==default_redl);
    CHECK((hertz(rpi_apb_core_frequency).count()/hertz(i2c_pins_default_frequency).count())==2500U);
    CHECK(i2c_ctrl::instance().regs(0)->get_clock_divider()==2500U);
    CHECK(i2c_ctrl::instance().regs(0)->get_enable());
    CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_transfer_active());
    CHECK(i2c_ctrl::instance().regs(0)->get_tx_fifo_empty());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  {
    i2c_pins iic(pin_id(2),pin_id(3),1); // SDA1, SCL1 -- explicitly specify BSC1
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(2)));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(3)));
    CHECK(i2c_ctrl::instance().alloc.is_in_use(1));
    CHECK(i2c_ctrl::instance().regs(1)->get_clock_stretch_timeout()==default_tout);
    CHECK(i2c_ctrl::instance().regs(1)->get_write_delay()==default_fedl);
    CHECK(i2c_ctrl::instance().regs(1)->get_read_delay()==default_redl);
    CHECK((hertz(rpi_apb_core_frequency).count()/hertz(i2c_pins_default_frequency).count())==2500U);
    CHECK(i2c_ctrl::instance().regs(1)->get_clock_divider()==2500U);
    CHECK(i2c_ctrl::instance().regs(1)->get_enable());
    CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_transfer_active());
    CHECK(i2c_ctrl::instance().regs(1)->get_tx_fifo_empty());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(2)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(3)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(1));
  CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0020/create bad - no expected alt-fn"
         , "Creating i2c_pins from SDA/SCL pin parameter values that do not "
           "have the required alternative function fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins(pin_id(1),pin_id(1))), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(1),pin_id(1),0)), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(0))), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(0),0)), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0030/create bad - too many expected alt-fn"
         , "Creating i2c_pins from pin parameter values having more than one "
           "possible candidate for the required alternative function fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins(pin_id(44),pin_id(1))), std::range_error);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(44)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(45))), std::range_error);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(45)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0040/create bad - invalid explicit BSC number"
         , "Creating i2c_pins from a BSC number other than 0 or 1 fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1),2)), std::out_of_range);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0050/create bad - conflicting SDA/SCL BSC peripheral"
         , "Creating i2c_pins from pins supporting SDA and SCL functions for "
           "different BSC peripherals fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(3))), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(3)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(1));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(2),pin_id(1))), std::invalid_argument);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(2)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(1));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  CHECK_FALSE(i2c_ctrl::instance().regs(1)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0060/create bad - fedl out of range"
         , "Creating i2c_pins with a fedl parameter value that is more than "
           "half the computed CDIV(fc/f) value fails"
         )
{
  REQUIRE_THROWS_AS(  (i2c_pins ( pin_id(0),pin_id(1),i2c_pins_default_frequency
                                , default_tout
                                , 1251
                                )
                      )
                   , std::out_of_range
                   );
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0070/create bad - redl out of range"
         , "Creating i2c_pins with a redl parameter value that is more than "
           "half the computed CDIV(fc/f) value fails"
         )
{
  REQUIRE_THROWS_AS(  (i2c_pins ( pin_id(0),pin_id(1),i2c_pins_default_frequency
                                , default_tout, default_fedl
                                , 1251
                                )
                      )
                   , std::out_of_range
                   );
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0080/create bad - clock frequency too low"
         , "Creating i2c_pins with a f parameter value that is less than "
           "fc/32768 fails"
         )
{
  REQUIRE_THROWS_AS(  (i2c_pins ( pin_id(0),pin_id(1)
                                , (rpi_apb_core_frequency.count()/32768U)-1U
                                )
                      )
                   , std::out_of_range
                   );
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}
