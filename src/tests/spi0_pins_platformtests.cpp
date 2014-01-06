// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins_platformtests.cpp 
/// @brief Platform tests for spi0_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "spi0_pins.h"
#include "spi0_ctrl.h"
#include "gpio_ctrl.h"
#include "periexcept.h"

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

TEST_CASE( "Platform-tests/spi0_slave_context/0000/create & destroy good"
         , "Creating spi0_slave_context from a good set of parameters leaves "
           "object in the expected state"
         )
{
  {
    spi0_slave_context sc(spi0_slave::chip0, hertz(rpi_apb_core_frequency.count()/2));
  }
  {
    hertz min_freq((rpi_apb_core_frequency.count()/65536) + 1);
    spi0_slave_context sc(spi0_slave::chip1, min_freq);
  }
}

TEST_CASE( "Platform-tests/spi0_slave_context/0010/create bad: force bad chip"
         , "Creating spi0_slave_context from a bad chip value throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_slave_context(static_cast<spi0_slave>(3),megahertz(1)))
                   , std::invalid_argument);
}

TEST_CASE( "Platform-tests/spi0_slave_context/0020/create bad: frequency too high"
         , "Creating spi0_slave_context from too high frequency value "
           "throws exception"
         )
{
  hertz high_freq(rpi_apb_core_frequency.count() + 1);
  REQUIRE_THROWS_AS((spi0_slave_context(spi0_slave::chip1, high_freq))
                   , std::out_of_range);
}

TEST_CASE( "Platform-tests/spi0_slave_context/0030/create bad: frequency too low"
         , "Creating spi0_slave_context from too low frequency value "
           "throws exception"
         )
{
  hertz low_freq((rpi_apb_core_frequency.count()/65536) - 1);
  REQUIRE_THROWS_AS((spi0_slave_context(spi0_slave::chip1, low_freq))
                   , std::out_of_range);
}

TEST_CASE( "Platform-tests/spi0_slave_context/0040/create bad: ltoh too low"
         , "Creating spi0_slave_context from too low ltoh value "
           "throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_slave_context( spi0_slave::chip1
                                      , megahertz(1)
                                      , spi0_mode::standard
                                      , spi0_clk_polarity::low
                                      , spi0_clk_phase::middle
                                      , 0U)) 
                   , std::out_of_range
                   );
}

TEST_CASE( "Platform-tests/spi0_slave_context/0050/create bad: ltoh too high"
         , "Creating spi0_slave_context from too high ltoh value "
           "throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_slave_context( spi0_slave::chip1
                                      , megahertz(1)
                                      , spi0_mode::standard
                                      , spi0_clk_polarity::low
                                      , spi0_clk_phase::middle
                                      , 16U)) 
                   , std::out_of_range
                   );
}


TEST_CASE( "Platform-tests/spi0_pins/0000/create & destroy with good full pin sets"
         , "Creating spi0_pins from a set of full 3-wire SPI0 pin set is in "
           "the expected state, destroys and checks pin & SPI0 peripheral free"
         )
{
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
    CHECK(spi0_ctrl::instance().allocated);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    CHECK(sp.has_std_mode_support());
    CHECK_FALSE(sp.is_conversing());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  CHECK_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0010/create & destroy with good 2 wire pin sets"
         , "Creating spi0_pins from a set of 2-wire only SPI0 pin set is in "
           "the expected state, destroys and checks pin & SPI0 peripheral free"
         )
{
  {
    spi0_pins sp(rpi_p1_spi0_2_wire_only_pin_set);
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
    CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
    CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
    CHECK(spi0_ctrl::instance().allocated);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    CHECK_FALSE(sp.has_std_mode_support());
    CHECK_FALSE(sp.is_conversing());
  }
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  CHECK_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0020/create good, non-default CSPOLn values"
         , "Creating spi0_pins using non-default cspol0,cspol1 parameter "
           "values has the expected result on the SPI0 CS register value"
         )
{
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set, spi0_cs_polarity::high);
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
  {
    spi0_pins sp( rpi_p1_spi0_full_pin_set
                , spi0_cs_polarity::high, spi0_cs_polarity::high);
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
  {
    spi0_pins sp( rpi_p1_spi0_full_pin_set
                , spi0_cs_polarity::low, spi0_cs_polarity::high);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
  {
    spi0_pins sp( rpi_p1_spi0_full_pin_set);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
}

TEST_CASE( "Platform-tests/spi0_pins/0030/create bad: no expected alt-fn"
         , "Creating spi0_pins from an SPI0 pin set with a pin that does not "
           "support the stated special function throws an exception"
         )
{
  spi0_pin_set<0U,7U,11U,10U,9U> full_bad_ce0;
  spi0_pin_set<8U,0U,11U,10U,9U> full_bad_ce1;
  spi0_pin_set<8U,7U,0U,10U,9U> full_bad_sclk;
  spi0_pin_set<8U,7U,11U,0U,9U> full_bad_mosi;
  spi0_pin_set<8U,7U,11U,10,0U> full_bad_miso;
  REQUIRE_THROWS_AS((spi0_pins(full_bad_ce0 )), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  REQUIRE_THROWS_AS((spi0_pins(full_bad_ce1 )), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_sclk)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_mosi)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(full_bad_miso)), std::invalid_argument);

  spi0_pin_set<0U,7U,11U,10U> two_wire_bad_ce0;
  spi0_pin_set<8U,0U,11U,10U> two_wire_bad_ce1;
  spi0_pin_set<8U,7U,0U,10U>  two_wire_bad_sclk;
  spi0_pin_set<8U,7U,11U,0U>  two_wire_bad_mosi;
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_ce0 )), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_ce1 )), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_sclk)), std::invalid_argument);
  REQUIRE_THROWS_AS((spi0_pins(two_wire_bad_mosi)), std::invalid_argument);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
}

TEST_CASE( "Platform-tests/spi0_pins/0040/create bad: SPI0 in use"
         , "Creating spi0_pins from a good SPI0 pin set when the SPI0 "
           "peripheral is marked as in use throws an exception"
         )
{
  spi0_ctrl::instance().allocated = true;
  REQUIRE(spi0_ctrl::instance().allocated);

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  spi0_ctrl::instance().allocated = false;
  REQUIRE_FALSE(spi0_ctrl::instance().allocated);
}

TEST_CASE( "Platform-tests/spi0_pins/0050/create bad: pins in use"
         , "Creating spi0_pins from a good SPI0 pin set when a pin in the set "
           "is marked as in use throws an exception"
         )
{
  REQUIRE_FALSE(spi0_ctrl::instance().allocated);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));


  gpio_ctrl::instance().alloc.allocate(spi_ce0_n);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_ce0_n);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));

  gpio_ctrl::instance().alloc.allocate(spi_ce1_n);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_ce1_n);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));

  gpio_ctrl::instance().alloc.allocate(spi_sclk);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_sclk);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));

  gpio_ctrl::instance().alloc.allocate(spi_mosi);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_mosi);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));

  gpio_ctrl::instance().alloc.allocate(spi_miso);
  REQUIRE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  REQUIRE_THROWS_AS((spi0_pins(rpi_p1_spi0_full_pin_set)),bad_peripheral_alloc);
  CHECK_FALSE(spi0_ctrl::instance().allocated);
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce0_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_ce1_n));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_sclk));
  CHECK_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_mosi));
  CHECK(gpio_ctrl::instance().alloc.is_in_use(spi_miso));

  gpio_ctrl::instance().alloc.deallocate(spi_miso);
  REQUIRE_FALSE(gpio_ctrl::instance().alloc.is_in_use(spi_miso));
}

TEST_CASE( "Platform-tests/spi0_pins/0100/write_fifo_is_empty() false if no conversation"
         , "Calling write_fifo_is_empty on a spi0_pins object returns false "
           "if is_conversing() returns false"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.write_fifo_is_empty());
}

TEST_CASE( "Platform-tests/spi0_pins/110/write_fifo_is_empty with conversation"
         , "Calling write_fifo_is_empty on a spi0_pins object when "
           "is_conversing() returns true returns true or false depending "
           "on TX FIFO state"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  REQUIRE(sp.is_conversing());
  CHECK(sp.write_fifo_is_empty());
  spi0_ctrl::instance().regs->transmit_fifo_write(97U);
  CHECK_FALSE(sp.write_fifo_is_empty());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/0120/write_fifo_has_space()"
         , "Calling write_fifo_has_space on a spi0_pins object returns false "
           "if the transmit FIFO if full, true otherwise"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  CHECK(sp.write_fifo_has_space());
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  REQUIRE(sp.is_conversing());
  CHECK(sp.write_fifo_has_space());
  bool false_returned_on_full{false};
  for (int i=0; i!=128; ++i)
    {
      spi0_ctrl::instance().regs->transmit_fifo_write(97U);
      if (!sp.write_fifo_has_space())
        {
          false_returned_on_full = true;
          break;
        }
    }
  CHECK(false_returned_on_full);
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  CHECK(sp.write_fifo_has_space());
}

TEST_CASE( "Platform-tests/spi0_pins/0200/read_fifo_is_full() false "
         , "Calling read_fifo_is_full on a spi0_pins object returns false "
           "(cannot check otherwise without hardware-assistance)"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_is_full());
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  REQUIRE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_is_full()); 
}

TEST_CASE( "Platform-tests/spi0_pins/0210/read_fifo_has_data() false "
         , "Calling read_fifo_has_data on a spi0_pins object returns false "
           "(cannot check otherwise without hardware-assistance)"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_has_data());
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  REQUIRE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_has_data()); 
}

TEST_CASE( "Platform-tests/spi0_pins/0220/read_fifo_needs_reading() false "
         , "Calling read_fifo_needs_reading on a spi0_pins object returns false "
           "(cannot check otherwise without hardware-assistance)"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_needs_reading());
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  REQUIRE(sp.is_conversing());
  CHECK_FALSE(sp.read_fifo_needs_reading()); 
}

// spi0_pins with spi0_slave_context
TEST_CASE( "Platform-tests/spi0_pins/1000/start/stop conversing"
         , "Start conversing with slave device changes spi_pins object state "
           "to is_conversing, and inverse states after stopping conversing"
         )
{
  spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  REQUIRE_FALSE(sp.is_conversing());
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
  sp.start_conversing(sc);
  CHECK(sp.is_conversing());
  CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  sp.stop_conversing();
  CHECK_FALSE(sp.is_conversing());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_pins/1010/conversing continues if ctx destroyed"
         , "Destroying spi0_slave_context object does not affect  spi0_pins "
           "object conversing state"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  REQUIRE_FALSE(sp.is_conversing());
  {
    spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
    sp.start_conversing(sc);
    CHECK(sp.is_conversing());
    CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  }
  CHECK(sp.is_conversing());
  CHECK(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_slave_context/1020/stop_conversing de-asserts CS TA"
         , "stop_conversing stops data transfers (CS register TA field 0)"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  CHECK(sp.is_conversing());  
  sp.stop_conversing();
  CHECK_FALSE(sp.is_conversing());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_pins/1030/Conversing stops when destroyed"
         , "Destroying the spi0_pins object stops any conversing"
         )
{
  spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);  
    REQUIRE_FALSE(sp.is_conversing());
    sp.start_conversing(sc);
    CHECK(sp.is_conversing());
    CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  }
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_pins/1040/good: different sequential conversations"
         , "Start conversing with one slave context followed by another is OK"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
  sp.start_conversing(sc);
  CHECK(sp.is_conversing());  
  spi0_slave_context sc2(spi0_slave::chip1, kilohertz(25));
  CHECK(sp.is_conversing());
}

TEST_CASE( "Platform-tests/spi0_pins/1050/bad: std SPI mode ctx with 2-wire only spi0_pins"
         , "Start conversing with a spi0_slave_context object for standard "
           "3-wire SPI mode with a 2-wire only spi0_pins object throws"
         )
{
  spi0_pins sp(rpi_p1_spi0_2_wire_only_pin_set);
  spi0_slave_context sc_los(spi0_slave::chip0,kilohertz(25),spi0_mode::lossi);
  sp.start_conversing(sc_los);
  spi0_slave_context sc_bid(spi0_slave::chip0,kilohertz(25),spi0_mode::bidirectional);
  sp.start_conversing(sc_bid);
  spi0_slave_context sc_std(spi0_slave::chip0,kilohertz(25));
  REQUIRE_THROWS_AS(sp.start_conversing(sc_std), std::invalid_argument);
}

TEST_CASE( "Platform-tests/spi0_pins/1100/good: Start conversing sets clock divider"
         , "Starting conversing sets the clock divider specified by the "
           "spi0_slave_context's "
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
//1MHz should yield a CDIV of 250: even but not a power of 2
  hertz test_freq1Mhz(megahertz(1U));
  spi0_slave_context sc1Mhz(spi0_slave::chip0, test_freq1Mhz);
  std::uint32_t expected_1MHz_cdiv{rpi_apb_core_frequency.count()/test_freq1Mhz.count()};
  spi0_ctrl::instance().regs->set_clock_divider(65536U);
  sp.start_conversing(sc1Mhz);
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_1MHz_cdiv);
  sp.stop_conversing();
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_1MHz_cdiv);
// Odd CDIV values allowed, 2MHz should yield a divider of 125:
// BUT bit 0 is ignored and acted on as if it were 0, so a CDIV
// value of 125 acts as a CDIV value of 124.
  hertz test_frequ2Mhz(megahertz(2U));
  spi0_slave_context sc2Mhz(spi0_slave::chip0, test_frequ2Mhz);
  std::uint32_t expected_2Mhz_cdiv{rpi_apb_core_frequency.count()/test_frequ2Mhz.count()};
  spi0_ctrl::instance().regs->set_clock_divider(65536U);
  sp.start_conversing(sc2Mhz);
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_2Mhz_cdiv);
  sp.stop_conversing();
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_2Mhz_cdiv);
// Is a CDIV of 2 OK?
  spi0_slave_context sc_max_freq(spi0_slave::chip0, hertz(rpi_apb_core_frequency.count()/2));
  std::uint32_t expected_min_cdiv{2U};
  spi0_ctrl::instance().regs->set_clock_divider(65535U);
  sp.start_conversing(sc_max_freq);
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_min_cdiv);
  sp.stop_conversing();
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_min_cdiv);
  spi0_ctrl::instance().regs->set_clock_divider(65535U);
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==65535U);
}

TEST_CASE( "Platform-tests/spi0_pins/1110/good: start_conversing sets ltoh in LoSSI mode"
         , "start_conversing only sets the LTOH register to that of the "
           "spi0_slave_context if using LoSSI mode"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);

  std::uint32_t updated_ltoh{10U};
  std::uint32_t original_ltoh{2U};
  spi0_ctrl::instance().regs->set_lossi_output_hold_delay(original_ltoh);
  spi0_slave_context sc_std( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          , spi0_clk_polarity::low
                          , spi0_clk_phase::middle
                          , updated_ltoh
                          ); 
  sp.start_conversing(sc_std);
  CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==original_ltoh);
  spi0_slave_context sc_bid( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::bidirectional
                          , spi0_clk_polarity::low
                          , spi0_clk_phase::middle
                          , updated_ltoh
                          ); 
  sp.start_conversing(sc_bid);
  CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==original_ltoh);
  spi0_slave_context sc_los( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::lossi
                          , spi0_clk_polarity::low
                          , spi0_clk_phase::middle
                          , updated_ltoh
                          );
  sp.start_conversing(sc_los);
  CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==updated_ltoh);
 }

TEST_CASE( "Platform-tests/spi0_pins/1120/good: start_conversing leaves CSPOL0,1 alone"
         , "Opening spi0_slave_context does not modify the CS register CSPOL0 "
           "and CSPOL1 field values"
         )
{
  spi0_slave_context sc(spi0_slave::chip0, kilohertz(25));
  
  {
    spi0_pins sp_00(rpi_p1_spi0_full_pin_set);
    REQUIRE_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    REQUIRE_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    sp_00.start_conversing(sc);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
  {
    spi0_pins sp_11( rpi_p1_spi0_full_pin_set
                   , spi0_cs_polarity::high, spi0_cs_polarity::high);
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    sp_11.start_conversing(sc);
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
}

TEST_CASE( "Platform-tests/spi0_pins/1130/good: start_conversing sets CS"
         , "start_conversing sets the CS register CS field to the "
           "spi0_slave_context slave chip enable line number"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_ctrl::instance().regs->set_chip_select(2U);
  REQUIRE(spi0_ctrl::instance().regs->get_chip_select()==2U);
  spi0_slave_context sc_0( spi0_slave::chip0, kilohertz(25)); 
  sp.start_conversing(sc_0);
  CHECK(spi0_ctrl::instance().regs->get_chip_select()==0U);
  spi0_ctrl::instance().regs->set_chip_select(2U);
  REQUIRE(spi0_ctrl::instance().regs->get_chip_select()==2U);
  spi0_slave_context sc_1( spi0_slave::chip1, kilohertz(25)); 
  sp.start_conversing(sc_1);
  CHECK(spi0_ctrl::instance().regs->get_chip_select()==1U);
}

TEST_CASE( "Platform-tests/spi0_pins/1140/good: start_conversing sets fields for mode"
         , "start_conversing only sets fields relevant for the "
           "spi0_slave_context's specified communication mode"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc_std( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          ); 
  sp.start_conversing(sc_std);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  spi0_slave_context sc_bid( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::bidirectional
                          ); 
  sp.start_conversing(sc_bid);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_enable());

// REN is used in bidirectional mode but starts in write mode.
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
   spi0_slave_context sc_los( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::lossi
                          );
  sp.start_conversing(sc_los);
  CHECK(spi0_ctrl::instance().regs->get_lossi_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
}

TEST_CASE( "Platform-tests/spi0_pins/1150/good: start_conversing sets CPOL"
         , "start_conversing only sets the CS CPOL register to that of "
           "the spi0_slave_context"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_ctrl::instance().regs->set_clock_polarity(true);
  spi0_slave_context sc_std( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          , spi0_clk_polarity::low
                          ); 
  sp.start_conversing(sc_std);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_clock_polarity());
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_clock_polarity());
  spi0_slave_context sc_bid( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          , spi0_clk_polarity::high
                          ); 
  sp.start_conversing(sc_bid);
  CHECK(spi0_ctrl::instance().regs->get_clock_polarity());
}

TEST_CASE( "Platform-tests/spi0_pins/1160/good: start_conversing sets CPHA"
         , "start_conversing only sets the CS CPHA register to that of "
           "the spi0_slave_context"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_ctrl::instance().regs->set_clock_phase(true);
  spi0_slave_context sc_std( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          , spi0_clk_polarity::low
                          , spi0_clk_phase::middle
                          ); 
  sp.start_conversing(sc_std);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_clock_phase());
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_clock_phase());
  spi0_slave_context sc_bid( spi0_slave::chip0
                          , kilohertz(25)
                          , spi0_mode::standard
                          , spi0_clk_polarity::low
                          , spi0_clk_phase::start
                          ); 
  sp.start_conversing(sc_bid);
  CHECK(spi0_ctrl::instance().regs->get_clock_phase());
}

TEST_CASE( "Platform-tests/spi0_pins/1170/good: start_conversing sets common CS state"
         , "start_conversing sets the CS state that is common to all "
           "spi0_slave_context instances"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc);

// Not using DMA
  CHECK_FALSE(spi0_ctrl::instance().regs->get_dma_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_dma_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_long_word());

// Not using interrupts
  CHECK_FALSE(spi0_ctrl::instance().regs->get_interrupt_on_done());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_interrupt_on_rxr());

// Not de-asserting CS when done
  CHECK_FALSE(spi0_ctrl::instance().regs->get_auto_deassert_chip_select());

// Start in write mode - have to write command in bidirectional mode 1st!
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());

// Don't really care about CSPOL and CSPOL2 state - should be zero
  CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(2U));
  spi0_slave_context sc_bid( spi0_slave::chip1
                          , kilohertz(25)
                          , spi0_mode::lossi
                          , spi0_clk_polarity::high
                          , spi0_clk_phase::start
                          ); 
  sp.start_conversing(sc_bid);

  CHECK_FALSE(spi0_ctrl::instance().regs->get_dma_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_dma_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_long_word());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_interrupt_on_done());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_interrupt_on_rxr());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_auto_deassert_chip_select());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(2U));
}

TEST_CASE( "Platform-tests/spi0_pins/1180/good: start_conversing clears FIFOs"
         , "start_conversing clears the transmit (and receive) FIFOs"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_ctrl::instance().regs->set_transfer_active(true);
  REQUIRE(spi0_ctrl::instance().regs->get_transfer_active());
  while(spi0_ctrl::instance().regs->get_tx_fifo_not_full())
    {
      spi0_ctrl::instance().regs->transmit_fifo_write(65U);
    }
  spi0_ctrl::instance().regs->set_transfer_active(false);
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_tx_fifo_not_full());
  spi0_slave_context sc0( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc0);
  CHECK(spi0_ctrl::instance().regs->get_tx_fifo_not_full());
  REQUIRE(spi0_ctrl::instance().regs->get_transfer_active());
  while(spi0_ctrl::instance().regs->get_tx_fifo_not_full())
    {
      spi0_ctrl::instance().regs->transmit_fifo_write(97U);
    }
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_tx_fifo_not_full());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_tx_fifo_not_full());
  spi0_slave_context sc1( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc1);
  CHECK(spi0_ctrl::instance().regs->get_tx_fifo_not_full());
}

TEST_CASE( "Platform-tests/spi0_pins/1400/good: std: write 1 byte to open conversation"
         , "Writing one byte when conversing in standard mode conversation succeeds"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc);
  CHECK(sp.write(65U));
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  std::uint8_t data{66U}; 
  CHECK(sp.write(&data, 1)==1U);
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/1410/bad: write 1 byte. not conversing"
         , "Writing one byte when not conversing fails"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.write(65U));
  std::uint8_t data{66U}; 
  CHECK(sp.write(&data, 1)==0U);
}

TEST_CASE( "Platform-tests/spi0_pins/1420/bad: std: write when full fails"
         , "Writing one byte conversing in standard mode returns "
           "false if write attempted when transmit FIFO is full"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc);
  REQUIRE(spi0_ctrl::instance().regs->get_transfer_active());
  for (;;)
    {
      spi0_ctrl::instance().regs->transmit_fifo_write(97U);
      if (!spi0_ctrl::instance().regs->get_tx_fifo_not_full())
        {
          CHECK_FALSE(sp.write(65U));
          std::uint8_t data{66U}; 
          CHECK(sp.write(&data, 1)==0U);
          break;
        }
    }
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/1430/good: bidir: write 1 byte to conversation"
         , "Writing one byte to a bidirectional mode conversation "
           "succeeds and sets CS REN bit to 0."
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0
                      , kilohertz(25)
                      , spi0_mode::bidirectional
                      );
  sp.start_conversing(sc);
  spi0_ctrl::instance().regs->set_read_enable(true);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  CHECK(sp.write(65U));
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  spi0_ctrl::instance().regs->set_read_enable(true);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  std::uint8_t data{66U}; 
  CHECK(sp.write(&data, 1)==1U);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/1440/good: std: write does not modify REN"
         , "Writing to a standard mode conversation does not modify the "
           "CS REN bit."
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) );
  sp.start_conversing(sc);
  spi0_ctrl::instance().regs->set_read_enable(true);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  CHECK(sp.write(65U));
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  spi0_ctrl::instance().regs->set_read_enable(true);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  std::uint8_t data{66U}; 
  CHECK(sp.write(&data, 1)==1U);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/1450/good: lossi: write data byte to conversation"
         , "Writing parameter data byte to a LoSSI mode conversation succeeds"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0
                      , kilohertz(25)
                      , spi0_mode::lossi
                      );
  sp.start_conversing(sc);
  CHECK(sp.write(65U));
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
  std::uint8_t data{66U}; 
  CHECK(sp.write(&data, 1)==1U);
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}

TEST_CASE( "Platform-tests/spi0_pins/1460/good: lossi: write cmd byte to conversation"
         , "Writing command byte to a LoSSI mode conversation succeeds"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0
                      , kilohertz(25)
                      , spi0_mode::lossi
                      );
  sp.start_conversing(sc);
  CHECK(sp.write(65U, spi0_lossi_write::command));
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
// Note: not supported by multi-byte write from buffer write overload.
}

TEST_CASE( "Platform-tests/spi0_pins/1600/ bad: read 1 byte, not conversing"
         , "Reading a byte when not conversing fails"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) ); 
  std::uint8_t data{0U};
  REQUIRE_FALSE(sp.is_conversing());
  CHECK_FALSE(sp.read(data));
  CHECK(sp.read(&data,1)==0U);
}

TEST_CASE( "Platform-tests/spi0_pins/1610/bad: std: read when empty fails"
         , "Reading one byte from a standard mode conversation returns "
           "false if read attempted when receive FIFO is empty"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0, kilohertz(25) ); 
  sp.start_conversing(sc);
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_rx_fifo_not_empty());
  std::uint8_t data{0U};
  CHECK_FALSE(sp.read(data));
  CHECK(sp.read(&data,1)==0U);
}

TEST_CASE( "Platform-tests/spi0_pins/1620/good: bidir: read byte from conversation"
         , "Reading one byte from an open bidirectional mode conversation "
           "fails but sets CS REN bit to 1."
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  spi0_slave_context sc( spi0_slave::chip0
                      , kilohertz(25)
                      , spi0_mode::bidirectional
                      );
  sp.start_conversing(sc);
  spi0_ctrl::instance().regs->set_read_enable(false);
  CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  std::uint8_t data{0U};
  CHECK_FALSE(sp.read(data));
  std::size_t pending_reads{0U};
  CHECK(sp.read(&data,1,&pending_reads)==0U);
  CHECK(pending_reads==1U);
  CHECK(spi0_ctrl::instance().regs->get_read_enable());
  spi0_ctrl::instance().regs->clear_fifo(spi0_fifo_clear_action::clear_tx);
}
