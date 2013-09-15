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
    CHECK_FALSE(sp.has_conversation());
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
    CHECK_FALSE(sp.has_conversation());
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


TEST_CASE( "Platform-tests/spi0_conversation/0000/create & destroy good"
         , "Creating spi0_conversation from a good set of parameters leaves "
           "object in the expected state"
         )
{
  {
    spi0_conversation sc(spi0_slave::chip0, rpi_apb_core_frequency);
  }
  {
    hertz min_freq((rpi_apb_core_frequency.count()/65536) + 1);
    spi0_conversation sc(spi0_slave::chip1, min_freq);
  }
}

TEST_CASE( "Platform-tests/spi0_conversation/0010/create bad: force bad chip"
         , "Creating spi0_conversation from a bad chip value throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_conversation(static_cast<spi0_slave>(3),megahertz(1)))
                   , std::invalid_argument);
}

TEST_CASE( "Platform-tests/spi0_conversation/0020/create bad: frequency too high"
         , "Creating spi0_conversation from too high frequency value "
           "throws exception"
         )
{
  hertz high_freq(rpi_apb_core_frequency.count() + 1);
  REQUIRE_THROWS_AS((spi0_conversation(spi0_slave::chip1, high_freq))
                   , std::out_of_range);
}

TEST_CASE( "Platform-tests/spi0_conversation/0030/create bad: frequency too low"
         , "Creating spi0_conversation from too low frequency value "
           "throws exception"
         )
{
  hertz low_freq((rpi_apb_core_frequency.count()/65536) - 1);
  REQUIRE_THROWS_AS((spi0_conversation(spi0_slave::chip1, low_freq))
                   , std::out_of_range);
}

TEST_CASE( "Platform-tests/spi0_conversation/0040/create bad: ltoh too low"
         , "Creating spi0_conversation from too low ltoh value "
           "throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_conversation( spi0_slave::chip1
                                      , megahertz(1)
                                      , spi0_mode::standard
                                      , spi0_clk_polarity::low
                                      , spi0_clk_phase::middle
                                      , 0U)) 
                   , std::out_of_range
                   );
}

TEST_CASE( "Platform-tests/spi0_conversation/0050/create bad: ltoh too high"
         , "Creating spi0_conversation from too high ltoh value "
           "throws exception"
         )
{
  REQUIRE_THROWS_AS((spi0_conversation( spi0_slave::chip1
                                      , megahertz(1)
                                      , spi0_mode::standard
                                      , spi0_clk_polarity::low
                                      , spi0_clk_phase::middle
                                      , 16U)) 
                   , std::out_of_range
                   );
}

TEST_CASE( "Platform-tests/spi0_conversation/0100/open/close changes states OK"
         , "Opening spi0_conversation object changes state to open and "
           "associated spi_pins object to has_conversation, and inverse "
           "states after closing"
         )
{
  spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  REQUIRE_FALSE(sc.is_open());
  REQUIRE_FALSE(sp.has_conversation());
  REQUIRE_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
  sc.open(sp);
  CHECK(sc.is_open());
  CHECK(sp.has_conversation());
  CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  sc.close();
  CHECK_FALSE(sc.is_open());
  CHECK_FALSE(sp.has_conversation());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_conversation/0110/open: close when destroyed"
         , "Destroying an open spi0_conversation object closes it"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  REQUIRE_FALSE(sp.has_conversation());
  {
    spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
    REQUIRE_FALSE(sc.is_open());
    sc.open(sp);
    CHECK(sc.is_open());
    CHECK(sp.has_conversation());
    CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  }
  CHECK_FALSE(sp.has_conversation());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_conversation/0120/open: close when spi_pins destroyed"
         , "Destroying the spi0_pins object associated with an open "
           "spi0_conversation object closes it"
         )
{
  spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
  REQUIRE_FALSE(sc.is_open());
  {
    spi0_pins sp(rpi_p1_spi0_full_pin_set);  
    REQUIRE_FALSE(sp.has_conversation());
    sc.open(sp);
    CHECK(sc.is_open());
    CHECK(sp.has_conversation());
    CHECK(spi0_ctrl::instance().regs->get_transfer_active());
  }
  CHECK_FALSE(sc.is_open());
  CHECK_FALSE(spi0_ctrl::instance().regs->get_transfer_active());
}

TEST_CASE( "Platform-tests/spi0_conversation/0130/bad: two open objects is an error"
         , "Opening a spi0_conversation object when another is already open "
           "throws an exception"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
  sc.open(sp);
  spi0_conversation sc2(spi0_slave::chip1, kilohertz(25));
  REQUIRE_THROWS_AS(sc2.open(sp), peripheral_in_use);
}

TEST_CASE( "Platform-tests/spi0_conversation/0140/good: two open objects one after other"
         , "Opening a spi0_conversation object after closing another is OK"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);  
  spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
  sc.open(sp);
  CHECK(sc.is_open());
  sc.close();
  CHECK_FALSE(sc.is_open());
  spi0_conversation sc2(spi0_slave::chip1, kilohertz(25));
  sc2.open(sp);
  CHECK(sc2.is_open());
}

TEST_CASE( "Platform-tests/spi0_conversation/0150/bad: std SPI mode with 2-wire only spi0_pins"
         , "Opening a spi0_conversation object for standard 3-wire SPI mode "
           "with a 2-wire only spi0_pins object throws"
         )
{
  spi0_pins sp(rpi_p1_spi0_2_wire_only_pin_set);
  {
    spi0_conversation sc_los(spi0_slave::chip0,kilohertz(25),spi0_mode::lossi);
    sc_los.open(sp);
  }
  {
    spi0_conversation sc_bid(spi0_slave::chip0,kilohertz(25),spi0_mode::bidirectional);
    sc_bid.open(sp);
  }
  {
    spi0_conversation sc_std(spi0_slave::chip0,kilohertz(25));
    REQUIRE_THROWS_AS(sc_std.open(sp), std::invalid_argument);
  }
}

TEST_CASE( "Platform-tests/spi0_conversation/0200/good: open sets clock divider"
         , "Opening spi0_conversation sets the conversation's clock divider"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  hertz test_frequency(megahertz(1));
  spi0_conversation sc(spi0_slave::chip0, test_frequency);
  std::uint32_t expected_cdiv{rpi_apb_core_frequency.count()/test_frequency.count()};
  spi0_ctrl::instance().regs->set_clock_divider(65536U);
  sc.open(sp);
  CHECK(sc.is_open());
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_cdiv);
  sc.close();
  CHECK(spi0_ctrl::instance().regs->get_clock_divider()==expected_cdiv);
}

TEST_CASE( "Platform-tests/spi0_conversation/0210/good: open sets ltoh in LoSSI mode"
         , "Opening spi0_conversation only sets the LTOH register to that of "
         "the conversation if using LoSSI mode"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);

  std::uint32_t updated_ltoh{10U};
  std::uint32_t original_ltoh{2U};
  spi0_ctrl::instance().regs->set_lossi_output_hold_delay(original_ltoh);
  {
    spi0_conversation sc_std( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::standard
                            , spi0_clk_polarity::low
                            , spi0_clk_phase::middle
                            , updated_ltoh
                            ); 
    sc_std.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==original_ltoh);
  }
  {
    spi0_conversation sc_bid( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::bidirectional
                            , spi0_clk_polarity::low
                            , spi0_clk_phase::middle
                            , updated_ltoh
                            ); 
    sc_bid.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==original_ltoh);
  }
  {
    spi0_conversation sc_los( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::lossi
                            , spi0_clk_polarity::low
                            , spi0_clk_phase::middle
                            , updated_ltoh
                            );
    sc_los.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_lossi_output_hold_delay()==updated_ltoh);
  }
}

TEST_CASE( "Platform-tests/spi0_conversation/0220/good: open leaves CSPOL0,1 alone"
         , "Opening spi0_conversation does not modify the CS register CSPOL0 "
           "and CSPOL1 field values"
         )
{
  spi0_conversation sc(spi0_slave::chip0, kilohertz(25));
  
  {
    spi0_pins sp_00(rpi_p1_spi0_full_pin_set);
    REQUIRE_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    REQUIRE_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    sc.open(sp_00);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK_FALSE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
  {
    spi0_pins sp_11(rpi_p1_spi0_full_pin_set
                , spi0_cs_polarity::high, spi0_cs_polarity::high);
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
    sc.open(sp_11);
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(0));
    CHECK(spi0_ctrl::instance().regs->get_chip_select_polarity(1));
  }
}

TEST_CASE( "Platform-tests/spi0_conversation/0230/good: open sets CS"
         , "Opening spi0_conversation sets the CS register CS field to the "
         "conversation slave chip enable line number"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  {
    spi0_ctrl::instance().regs->set_chip_select(2U);
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select()==2U);
    spi0_conversation sc_0( spi0_slave::chip0, kilohertz(25)); 
    sc_0.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_chip_select()==0U);
  }
  {
    spi0_ctrl::instance().regs->set_chip_select(2U);
    REQUIRE(spi0_ctrl::instance().regs->get_chip_select()==2U);
    spi0_conversation sc_1( spi0_slave::chip1, kilohertz(25)); 
    sc_1.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_chip_select()==1U);
  }
}

TEST_CASE( "Platform-tests/spi0_conversation/0240/good: open sets fields for mode"
         , "Opening spi0_conversation only sets fields relevant for the "
           "conversation's specified communication mode"
         )
{
  spi0_pins sp(rpi_p1_spi0_full_pin_set);
  {
    spi0_conversation sc_std( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::standard
                            ); 
    sc_std.open(sp);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_enable());
    CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  }
  {
    spi0_conversation sc_bid( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::bidirectional
                            ); 
    sc_bid.open(sp);
    CHECK_FALSE(spi0_ctrl::instance().regs->get_lossi_enable());

  // REN is used in bidirectional mode but start in write mode.
    CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  }
  {
    spi0_conversation sc_los( spi0_slave::chip0
                            , kilohertz(25)
                            , spi0_mode::lossi
                            );
    sc_los.open(sp);
    CHECK(spi0_ctrl::instance().regs->get_lossi_enable());
    CHECK_FALSE(spi0_ctrl::instance().regs->get_read_enable());
  }
}
