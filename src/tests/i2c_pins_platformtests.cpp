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
    CHECK(i2c_ctrl::instance().regs(0)->get_transfer_type()==i2c_transfer_type::write);
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
    CHECK(i2c_ctrl::instance().regs(0)->get_transfer_type()==i2c_transfer_type::write);
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

TEST_CASE( "Platform-tests/i2c_pins/0020/create good - fedl maximum value"
         , "Creating i2c_pins with a fedl parameter value that is exactly "
           "half the computed CDIV(fc/f) value is OK"
         )
{
  i2c_pins iis(pin_id(0),pin_id(1),i2c_pins_default_frequency,default_tout,1250);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0030/create good - redl maximum value"
         , "Creating i2c_pins with a redl parameter value that is exactly "
           "half the computed CDIV(fc/f) value is OK"
         )
{
  i2c_pins iis( pin_id(0), pin_id(1)
              , i2c_pins_default_frequency, default_tout, default_fedl, 1250);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0040/create good - clock frequency minimum"
         , "Creating i2c_pins with a f parameter value that is exactly "
           "fc/32768 is OK"
         )
{
// Note: +1 to round up as 250,000,000 / 32768 is not a whole number.
  i2c_pins iis(pin_id(0),pin_id(1),hertz((rpi_apb_core_frequency.count()/32768)+1));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0050/create good - clock frequency maximum"
         , "Creating i2c_pins with a f parameter value that is exactly "
           "fc/32768 is OK"
         )
{
  i2c_pins iis(pin_id(0),pin_id(1),hertz(rpi_apb_core_frequency.count()/2),0,0,0);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0100/create bad - no expected alt-fn"
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

TEST_CASE( "Platform-tests/i2c_pins/0110/create bad - too many expected alt-fn"
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

TEST_CASE( "Platform-tests/i2c_pins/0120/create bad - invalid explicit BSC number"
         , "Creating i2c_pins from a BSC number other than 0 or 1 fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1),2)), std::out_of_range);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0130/create bad - conflicting SDA/SCL BSC peripheral"
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

TEST_CASE( "Platform-tests/i2c_pins/0140/create bad - fedl out of range"
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

TEST_CASE( "Platform-tests/i2c_pins/0150/create bad - redl out of range"
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

TEST_CASE( "Platform-tests/i2c_pins/0160/create bad - clock frequency too low"
         , "Creating i2c_pins with a f parameter value that is less than "
           "fc/32768 fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins( pin_id(0),pin_id(1)
                             , hertz((rpi_apb_core_frequency.count()/32768U)-1U)
                             )
                      )
                   , std::out_of_range
                   );
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0170/create bad - clock frequency too high"
         , "Creating i2c_pins with a f parameter value that is less than "
           "fc/32768 fails"
         )
{
  REQUIRE_THROWS_AS((i2c_pins( pin_id(0),pin_id(1)
                             , hertz((rpi_apb_core_frequency.count()/2)+1U)
                             )
                      )
                   , std::out_of_range
                   );
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
}

TEST_CASE( "Platform-tests/i2c_pins/0180/create bad - GPIO pins in use"
         , "Creating i2c_pins from GPIO pins that are in use fails"
         )
{
  gpio_ctrl::instance().alloc.allocate(pin_id(0));
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));

  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1))), bad_peripheral_alloc);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1),0)), bad_peripheral_alloc);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());

  gpio_ctrl::instance().alloc.deallocate(pin_id(0));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  gpio_ctrl::instance().alloc.allocate(pin_id(1));
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));

  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1))), bad_peripheral_alloc);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1),0)), bad_peripheral_alloc);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());

  gpio_ctrl::instance().alloc.deallocate(pin_id(1));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
}

TEST_CASE( "Platform-tests/i2c_pins/0190/create bad - I2C/BSC peripheral in use"
         , "Creating i2c_pins for a I2C/BSC peripheral that is in use fails"
         )
{
  i2c_ctrl::instance().alloc.allocate(0);
  REQUIRE(i2c_ctrl::instance().alloc.is_in_use(0));

  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1))), bad_peripheral_alloc);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());
  REQUIRE_THROWS_AS((i2c_pins(pin_id(0),pin_id(1),0)), bad_peripheral_alloc);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(0)));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(pin_id(1)));
  CHECK(i2c_ctrl::instance().alloc.is_in_use(0));
  CHECK_FALSE(i2c_ctrl::instance().regs(0)->get_enable());

  i2c_ctrl::instance().alloc.deallocate(0);
  REQUIRE_FALSE(i2c_ctrl::instance().alloc.is_in_use(0));
}

TEST_CASE( "Platform-tests/i2c_pins/0200/Created not busy"
         , "A just created i2c_pins object is not busy"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  CHECK_FALSE( iic.is_busy() );
}

TEST_CASE( "Platform-tests/i2c_pins/0210/Created write empty"
         , "A just created i2c_pins object BSC FIFO is empty"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  CHECK( iic.write_fifo_is_empty() );
  CHECK( iic.write_fifo_has_space() );
  CHECK_FALSE( iic.write_fifo_needs_writing() ); // only set during transaction
}

TEST_CASE( "Platform-tests/i2c_pins/0220/FIFO write not empty"
         , "Writing data to a BSC FIFO, i2c_pins object reports FIFO not empty"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  i2c_ctrl::instance().regs(0)->transmit_fifo_write('x');
  CHECK_FALSE( iic.write_fifo_is_empty() );
  CHECK( iic.write_fifo_has_space() );
}

TEST_CASE( "Platform-tests/i2c_pins/0230/FIFO write and fill"
         , "Writing data to fill a BSC FIFO, i2c_pins object reports FIFO not "
           "empty and has no space"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  i2c_ctrl::instance().regs(0)->transmit_fifo_write('x');
  CHECK_FALSE( iic.write_fifo_is_empty() );
  for (int i=0; i<15; ++i)
    {
      i2c_ctrl::instance().regs(0)->transmit_fifo_write('x');    
    }
  CHECK_FALSE( iic.write_fifo_is_empty() );
  CHECK_FALSE( iic.write_fifo_has_space() );
}

TEST_CASE( "Platform-tests/i2c_pins/0240/Created: read not full, no data"
         , "A just created i2c_pins object BSC FIFO has no read data so is "
           "reported as not full, has no data and does not need reading"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  CHECK_FALSE( iic.read_fifo_has_data() );
  CHECK_FALSE( iic.read_fifo_is_full() );
  CHECK_FALSE( iic.read_fifo_needs_reading() ); 
}

TEST_CASE( "Platform-tests/i2c_pins/0240/Set for read: read not full, no data"
         , "A just created i2c_pins object, with C register READ field set "
           "with no read data reports FIFO as not full, having no data and "
           "not need reading"
         )
{
  i2c_pins iic(pin_id(0),pin_id(1)); // SDA0, SCL0 => BSC0
  i2c_ctrl::instance().regs(0)->set_transfer_type(i2c_transfer_type::read);
  REQUIRE(i2c_ctrl::instance().regs(0)->get_transfer_type()==i2c_transfer_type::read);

  CHECK_FALSE( iic.read_fifo_has_data() );
  CHECK_FALSE( iic.read_fifo_is_full() );
  CHECK_FALSE( iic.read_fifo_needs_reading() );

  i2c_ctrl::instance().regs(0)->set_transfer_type(i2c_transfer_type::write);
  CHECK(i2c_ctrl::instance().regs(0)->get_transfer_type()==i2c_transfer_type::write);
}
