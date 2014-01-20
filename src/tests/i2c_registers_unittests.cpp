// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_registers_unittests.cpp 
/// @brief Unit tests for low-level I2C/BSC master control registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 3 BSC
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "i2c_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals::internal;

typedef uint32_t RegisterType;
typedef unsigned char Byte;

// Register byte offsets, see BCM2835 peripherals manual BSC Address Map table
// in section 3.2 Register View
enum RegisterOffsets 
{    C_OFFSET=0x00,  S_OFFSET=0x04, DLEN_OFFSET=0x08,    A_OFFSET=0x0c
, FIFO_OFFSET=0x10, DIV_OFFSET=0x14, DEL_OFFSET=0x18, CLKT_OFFSET=0x1c
};

TEST_CASE( "Unit-tests/i2c_registers/0000/field offsets"
         , "I2C/BSC interface registers should have the expected offsets"
         )
{
  i2c_registers i2c_regs;
// initially start with all bytes of i2c_regs set to 0xFF:
  std::memset(&i2c_regs, 0xFF, sizeof(i2c_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&i2c_regs));
 
// Set each 32-bit register to the value of its offset:
  i2c_regs.control = C_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[C_OFFSET])==C_OFFSET );
  i2c_regs.status = S_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[S_OFFSET])==S_OFFSET );
  i2c_regs.data_length = DLEN_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DLEN_OFFSET])==DLEN_OFFSET );
  i2c_regs.slave_addrs = A_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[A_OFFSET])==A_OFFSET );
  i2c_regs.fifo = FIFO_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[FIFO_OFFSET])==FIFO_OFFSET );
  i2c_regs.clk_div = DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DIV_OFFSET])==DIV_OFFSET );
  i2c_regs.data_delay = DEL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DEL_OFFSET])==DEL_OFFSET );
  i2c_regs.clk_stretch = CLKT_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[CLKT_OFFSET])==CLKT_OFFSET );
}

TEST_CASE( "Unit-tests/i2c_registers/0010/get_transfer_type"
         , "get_transfer_type returns the expected value for the C "
           "register READ field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_transfer_type()==i2c_transfer_type::write);
  i2c_regs.control=1U;
  CHECK(i2c_regs.get_transfer_type()==i2c_transfer_type::read);
  i2c_regs.control=~1U;
  CHECK(i2c_regs.get_transfer_type()==i2c_transfer_type::write);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_transfer_type()==i2c_transfer_type::read);
}

TEST_CASE( "Unit-tests/i2c_registers/0020/get_interrupt_on_done"
         , "get_interrupt_on_done returns the expected value for the C "
           "register INTD field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_interrupt_on_done()==false);
  i2c_regs.control=1U<<8;
  CHECK(i2c_regs.get_interrupt_on_done()==true);
  i2c_regs.control=~(1U<<8);
  CHECK(i2c_regs.get_interrupt_on_done()==false);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_interrupt_on_done()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0030/get_interrupt_on_txw"
         , "get_interrupt_on_txw returns the expected value for the C "
           "register INTT field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_interrupt_on_txw()==false);
  i2c_regs.control=1U<<9;
  CHECK(i2c_regs.get_interrupt_on_txw()==true);
  i2c_regs.control=~(1U<<9);
  CHECK(i2c_regs.get_interrupt_on_txw()==false);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_interrupt_on_txw()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0040/get_interrupt_on_rxr"
         , "get_interrupt_on_rxr returns the expected value for the C "
           "register INTT field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_interrupt_on_rxr()==false);
  i2c_regs.control=1U<<10;
  CHECK(i2c_regs.get_interrupt_on_rxr()==true);
  i2c_regs.control=~(1U<<10);
  CHECK(i2c_regs.get_interrupt_on_rxr()==false);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_interrupt_on_rxr()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0050/get_enable"
         , "get_enable returns the expected value for the C "
           "register INTT field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_enable()==false);
  i2c_regs.control=1U<<15;
  CHECK(i2c_regs.get_enable()==true);
  i2c_regs.control=~(1U<<15);
  CHECK(i2c_regs.get_enable()==false);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_enable()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0110/set_transfer_type"
         , "set_transfer_type sets the expected value for the C "
           "register READ field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_read_mask==1U);
  i2c_regs.set_transfer_type(i2c_transfer_type::read);
  CHECK(i2c_regs.control==i2c_regs.c_read_mask);
  i2c_regs.control=~0U;
  i2c_regs.set_transfer_type(i2c_transfer_type::write);
  CHECK(i2c_regs.control==~i2c_regs.c_read_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0120/clear_fifo"
         , "clear_fifo writes the expected value to the C "
           "register CLEAR field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_clear_fifo==(3U<<4U));
  i2c_regs.clear_fifo();
  CHECK(i2c_regs.control==i2c_regs.c_clear_fifo);
}

TEST_CASE( "Unit-tests/i2c_registers/0130/start_transfer"
         , "start_transfer writes the expected value to the C "
           "register ST field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_start_transfer==(1U<<7U));
  i2c_regs.start_transfer();
  CHECK(i2c_regs.control==i2c_regs.c_start_transfer);
}

TEST_CASE( "Unit-tests/i2c_registers/0140/set_interrupt_on_done"
         , "set_interrupt_on_done sets the expected value for the C "
           "register INTD field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_int_on_done_mask==(1U<<8U));
  i2c_regs.set_interrupt_on_done(true);
  CHECK(i2c_regs.control==i2c_regs.c_int_on_done_mask);
  i2c_regs.control=~0U;
  i2c_regs.set_interrupt_on_done(false);
  CHECK(i2c_regs.control==~i2c_regs.c_int_on_done_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0150/set_interrupt_on_txw"
         , "set_interrupt_on_txw sets the expected value for the C "
           "register INTT field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_int_on_txw_mask==(1U<<9U));
  i2c_regs.set_interrupt_on_txw(true);
  CHECK(i2c_regs.control==i2c_regs.c_int_on_txw_mask);
  i2c_regs.control=~0U;
  i2c_regs.set_interrupt_on_txw(false);
  CHECK(i2c_regs.control==~i2c_regs.c_int_on_txw_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0160/set_interrupt_on_rxr"
         , "set_interrupt_on_rxr sets the expected value for the C "
           "register INTR field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_int_on_rxr_mask==(1U<<10U));
  i2c_regs.set_interrupt_on_rxr(true);
  CHECK(i2c_regs.control==i2c_regs.c_int_on_rxr_mask);
  i2c_regs.control=~0U;
  i2c_regs.set_interrupt_on_rxr(false);
  CHECK(i2c_regs.control==~i2c_regs.c_int_on_rxr_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0170/set_enable"
         , "set_enable sets the expected value for the C "
           "register I2CEN field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.c_enable_mask==(1U<<15U));
  i2c_regs.set_enable(true);
  CHECK(i2c_regs.control==i2c_regs.c_enable_mask);
  i2c_regs.control=~0U;
  i2c_regs.set_enable(false);
  CHECK(i2c_regs.control==~i2c_regs.c_enable_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0200/get_transfer_active"
         , "get_transfer_active returns the expected value for the S "
           "register TA field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_active_mask==1U); // S register bit 0
  CHECK(i2c_regs.get_transfer_active()==false);
  i2c_regs.status=i2c_regs.s_xfer_active_mask;
  CHECK(i2c_regs.get_transfer_active()==true);
  i2c_regs.status=~i2c_regs.s_xfer_active_mask;
  CHECK(i2c_regs.get_transfer_active()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_transfer_active()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0210/get_transfer_done"
         , "get_transfer_done returns the expected value for the S "
           "register DONE field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_done_mask==(1U<<1U)); // S register bit 1
  CHECK(i2c_regs.get_transfer_done()==false);
  i2c_regs.status=i2c_regs.s_xfer_done_mask;
  CHECK(i2c_regs.get_transfer_done()==true);
  i2c_regs.status=~i2c_regs.s_xfer_done_mask;
  CHECK(i2c_regs.get_transfer_done()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_transfer_done()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0220/get_tx_fifo_needs_writing"
         , "get_tx_fifo_needs_writing returns the expected value for the S "
           "register TXW field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_txw_mask==(1U<<2U)); // S register bit 2
  CHECK(i2c_regs.get_tx_fifo_needs_writing()==false);
  i2c_regs.status=i2c_regs.s_xfer_txw_mask;
  CHECK(i2c_regs.get_tx_fifo_needs_writing()==true);
  i2c_regs.status=~i2c_regs.s_xfer_txw_mask;
  CHECK(i2c_regs.get_tx_fifo_needs_writing()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_tx_fifo_needs_writing()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0230/get_rx_fifo_needs_reading"
         , "get_rx_fifo_needs_reading returns the expected value for the S "
           "register RXR field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_rxr_mask==(1U<<3U)); // S register bit 3
  CHECK(i2c_regs.get_rx_fifo_needs_reading()==false);
  i2c_regs.status=i2c_regs.s_xfer_rxr_mask;
  CHECK(i2c_regs.get_rx_fifo_needs_reading()==true);
  i2c_regs.status=~i2c_regs.s_xfer_rxr_mask;
  CHECK(i2c_regs.get_rx_fifo_needs_reading()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_rx_fifo_needs_reading()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0240/get_tx_fifo_not_full"
         , "get_tx_fifo_not_full returns the expected value for the S "
           "register TXD field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_txd_mask==(1U<<4U)); // S register bit 4
  CHECK(i2c_regs.get_tx_fifo_not_full()==false);
  i2c_regs.status=i2c_regs.s_xfer_txd_mask;
  CHECK(i2c_regs.get_tx_fifo_not_full()==true);
  i2c_regs.status=~i2c_regs.s_xfer_txd_mask;
  CHECK(i2c_regs.get_tx_fifo_not_full()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_tx_fifo_not_full()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0250/get_rx_fifo_not_empty"
         , "get_rx_fifo_not_empty returns the expected value for the S "
           "register RXD field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_rxd_mask==(1U<<5U)); // S register bit 5
  CHECK(i2c_regs.get_rx_fifo_not_empty()==false);
  i2c_regs.status=i2c_regs.s_xfer_rxd_mask;
  CHECK(i2c_regs.get_rx_fifo_not_empty()==true);
  i2c_regs.status=~i2c_regs.s_xfer_rxd_mask;
  CHECK(i2c_regs.get_rx_fifo_not_empty()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_rx_fifo_not_empty()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0260/get_tx_fifo_empty"
         , "get_tx_fifo_empty returns the expected value for the S "
           "register TXE field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_txe_mask==(1U<<6U)); // S register bit 6
  CHECK(i2c_regs.get_tx_fifo_empty()==false);
  i2c_regs.status=i2c_regs.s_xfer_txe_mask;
  CHECK(i2c_regs.get_tx_fifo_empty()==true);
  i2c_regs.status=~i2c_regs.s_xfer_txe_mask;
  CHECK(i2c_regs.get_tx_fifo_empty()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_tx_fifo_empty()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0270/get_rx_fifo_full"
         , "get_rx_fifo_full returns the expected value for the S "
           "register RXF field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_xfer_rxf_mask==(1U<<7U)); // S register bit 7
  CHECK(i2c_regs.get_rx_fifo_full()==false);
  i2c_regs.status=i2c_regs.s_xfer_rxf_mask;
  CHECK(i2c_regs.get_rx_fifo_full()==true);
  i2c_regs.status=~i2c_regs.s_xfer_rxf_mask;
  CHECK(i2c_regs.get_rx_fifo_full()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_rx_fifo_full()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0280/get_slave_ack_error"
         , "get_slave_ack_error returns the expected value for the S "
           "register ERR field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_ack_err_mask==(1U<<8U)); // S register bit 8
  CHECK(i2c_regs.get_slave_ack_error()==false);
  i2c_regs.status=i2c_regs.s_ack_err_mask;
  CHECK(i2c_regs.get_slave_ack_error()==true);
  i2c_regs.status=~i2c_regs.s_ack_err_mask;
  CHECK(i2c_regs.get_slave_ack_error()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_slave_ack_error()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0290/get_clock_timeout"
         , "get_clock_timeout returns the expected value for the S "
           "register CLKT field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.s_clk_timeout_mask==(1U<<9U)); // S register bit 9
  CHECK(i2c_regs.get_clock_timeout()==false);
  i2c_regs.status=i2c_regs.s_clk_timeout_mask;
  CHECK(i2c_regs.get_clock_timeout()==true);
  i2c_regs.status=~i2c_regs.s_clk_timeout_mask;
  CHECK(i2c_regs.get_clock_timeout()==false);
  i2c_regs.status=~0U;
  CHECK(i2c_regs.get_clock_timeout()==true);
}

TEST_CASE( "Unit-tests/i2c_registers/0300/clear_transfer_done"
         , "clear_transfer_done writes 1 only to S register DONE field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  i2c_regs.clear_transfer_done();
  CHECK(i2c_regs.status==i2c_regs.s_xfer_done_mask);
  i2c_regs.status=~i2c_regs.s_xfer_done_mask;
  i2c_regs.clear_transfer_done();
  CHECK(i2c_regs.status==i2c_regs.s_xfer_done_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0310/clear_slave_ack_error"
         , "clear_slave_ack_error writes 1 only to S register ERR field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  i2c_regs.clear_slave_ack_error();
  CHECK(i2c_regs.status==i2c_regs.s_ack_err_mask);
  i2c_regs.status=~i2c_regs.s_ack_err_mask;
  i2c_regs.clear_slave_ack_error();
  CHECK(i2c_regs.status==i2c_regs.s_ack_err_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0320/clear_clock_timeout"
         , "clear_clock_timeout writes 1 to only S register CLKT field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  i2c_regs.clear_clock_timeout();
  CHECK(i2c_regs.status==i2c_regs.s_clk_timeout_mask);
  i2c_regs.status=~i2c_regs.s_clk_timeout_mask;
  i2c_regs.clear_clock_timeout();
  CHECK(i2c_regs.status==i2c_regs.s_clk_timeout_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0400/get_data_length"
         , "get_data_length returns the DLEN register value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.dlen_mask==65535U); // lower 16 bits of register
  CHECK(i2c_regs.get_data_length()==0U);
  i2c_regs.data_length = ~0U;
  CHECK(i2c_regs.get_data_length()==i2c_regs.dlen_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0410/set_data_length"
         , "set_data_length sets the DLEN register value and returns true OR "
           "returns false"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.dlen_mask==65535U); // lower 16 bits of register
  CHECK(i2c_regs.set_data_length(i2c_regs.dlen_mask));
  CHECK(i2c_regs.data_length==i2c_regs.dlen_mask);
  CHECK(i2c_regs.set_data_length(0U));
  CHECK(i2c_regs.data_length==0U);
  CHECK_FALSE(i2c_regs.set_data_length(~0U));
  CHECK(i2c_regs.data_length==0U);
  CHECK_FALSE(i2c_regs.set_data_length(i2c_regs.dlen_mask+1));
  CHECK(i2c_regs.data_length==0U);
}

TEST_CASE( "Unit-tests/i2c_registers/0500/get_slave_address"
         , "get_slave_address returns the A register value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.a_mask==127U); // lower 7 bits of register
  CHECK(i2c_regs.get_slave_address()==0U);
  i2c_regs.slave_addrs = ~0U;
  CHECK(i2c_regs.get_slave_address()==i2c_regs.a_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0510/set_slave_address"
         , "set_slave_address sets the A register value and returns true OR "
           "returns false"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.a_mask==127U); // lower 7 bits of register
  CHECK(i2c_regs.set_slave_address(i2c_regs.a_mask));
  CHECK(i2c_regs.slave_addrs==i2c_regs.a_mask);
  CHECK(i2c_regs.set_slave_address(0U));
  CHECK(i2c_regs.slave_addrs==0U);
  CHECK_FALSE(i2c_regs.set_slave_address(~0U));
  CHECK(i2c_regs.slave_addrs==0U);
  CHECK_FALSE(i2c_regs.set_slave_address(i2c_regs.a_mask+1));
  CHECK(i2c_regs.slave_addrs==0U);
}

TEST_CASE( "Unit-tests/i2c_registers/0600/transmit_fifo_write"
         , "transmit_fifo_write set correct byte data to "
           "FIFO register DATA field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x00U, sizeof(i2c_regs));
  std::uint8_t expected{255U};
  i2c_regs.transmit_fifo_write(expected);
  CHECK(i2c_regs.fifo==expected);  
}

TEST_CASE( "Unit-tests/i2c_registers/0610/receive_fifo_read"
         , "receive_fifo_read obtains correct byte data from "
           "FIFO register DATA field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x00U, sizeof(i2c_regs));
  std::uint8_t expected{255U};
  i2c_regs.fifo = expected;
  CHECK(i2c_regs.receive_fifo_read()==expected);  
}

TEST_CASE( "Unit-tests/i2c_registers/0700/set_clock_divider"
         , "set_clock_divider sets correct value in the "
           "clock register CDIV field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x00U, sizeof(i2c_regs));
  dibase::rpi::peripherals::internal::register_t expected{2U};
  i2c_regs.set_clock_divider(expected);
  CHECK(i2c_regs.clk_div==expected);  
  i2c_regs.set_clock_divider(32768U);
  CHECK(i2c_regs.clk_div==0U);  
  CHECK_FALSE(i2c_regs.set_clock_divider(32769U));
  CHECK_FALSE(i2c_regs.set_clock_divider(1U));
}

TEST_CASE( "Unit-tests/i2c_registers/0710/get_clock_divider"
         , "get_clock_divider obtains correct value from the "
           "clock register CDIV field"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x00U, sizeof(i2c_regs));
  dibase::rpi::peripherals::internal::register_t expected{1U};
  i2c_regs.clk_div = expected;
  CHECK(i2c_regs.get_clock_divider()==expected);
  i2c_regs.clk_div = expected|0xffff8000U;
  CHECK(i2c_regs.get_clock_divider()==expected);
  i2c_regs.clk_div = 0U;
  CHECK(i2c_regs.get_clock_divider()==32768U);  
}

TEST_CASE( "Unit-tests/i2c_registers/0800/get_read_delay"
         , "get_read_delay returns the DEL register REDL field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.del_redl_mask==65535U); // lower 16 bits of register
  REQUIRE(i2c_regs.del_max==65535U); // All field values valid
  CHECK(i2c_regs.get_read_delay()==0U);
  i2c_regs.data_delay = ~0U;
  CHECK(i2c_regs.get_read_delay()==i2c_regs.del_max);
}

TEST_CASE( "Unit-tests/i2c_registers/0810/get_write_delay"
         , "get_write_delay returns the DEL register FEDL field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.del_fedl_bit==16U); 
  REQUIRE((i2c_regs.del_fedl_mask>>i2c_regs.del_fedl_bit)==65535U); 
  REQUIRE(i2c_regs.del_max==65535U); // All field values valid
  CHECK(i2c_regs.get_write_delay()==0U);
  i2c_regs.data_delay = ~0U;
  CHECK(i2c_regs.get_write_delay()==(i2c_regs.del_max));
}

TEST_CASE( "Unit-tests/i2c_registers/0820/set_read_delay"
         , "set_read_delay sets the DEL register REDL field value and returns "
           "true OR returns false"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.del_redl_mask==65535U); // lower 16 bits of register
  REQUIRE(i2c_regs.del_max==65535U); // All field values valid
  CHECK(i2c_regs.set_read_delay(i2c_regs.del_max));
  CHECK(i2c_regs.data_delay==i2c_regs.del_redl_mask);
  i2c_regs.data_delay = ~0U;
  CHECK(i2c_regs.set_read_delay(0U));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_redl_mask);
  CHECK_FALSE(i2c_regs.set_read_delay(~0U));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_redl_mask);
  CHECK_FALSE(i2c_regs.set_read_delay(i2c_regs.del_max+1));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_redl_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0830/set_write_delay"
         , "set_write_delay sets the DEL register FEDL field value and returns "
           "true OR returns false"
         )
{
  i2c_registers i2c_regs; 
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.del_fedl_bit==16U); 
  REQUIRE((i2c_regs.del_fedl_mask>>i2c_regs.del_fedl_bit)==65535U); 
  REQUIRE(i2c_regs.del_max==65535U); // All field values valid
  CHECK(i2c_regs.set_write_delay(i2c_regs.del_max));
  CHECK(i2c_regs.data_delay==i2c_regs.del_fedl_mask);
  i2c_regs.data_delay = ~0U;
  CHECK(i2c_regs.set_write_delay(0U));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_fedl_mask);
  CHECK_FALSE(i2c_regs.set_write_delay(~0U));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_fedl_mask);
  CHECK_FALSE(i2c_regs.set_write_delay(i2c_regs.del_max+1));
  CHECK(i2c_regs.data_delay==~i2c_regs.del_fedl_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0900/get_clock_stretch_timeout"
         , "get_clock_stretch_timeout returns the CLKT register TOUT field "
           "value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.clkt_tout_mask==65535U); // lower 16 bits of register
  CHECK(i2c_regs.get_clock_stretch_timeout()==0U);
  i2c_regs.clk_stretch = ~0U;
  CHECK(i2c_regs.get_clock_stretch_timeout()==i2c_regs.clkt_tout_mask);
}

TEST_CASE( "Unit-tests/i2c_registers/0910/set_clock_stretch_timeout"
         , "set_clock_stretch_timeout sets the CLKT register TOUT field value "
           "and returns true OR returns false"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  REQUIRE(i2c_regs.clkt_tout_mask==65535U); // lower 16 bits of register
  CHECK(i2c_regs.set_clock_stretch_timeout(i2c_regs.clkt_tout_mask));
  CHECK(i2c_regs.clk_stretch==i2c_regs.clkt_tout_mask);
  CHECK(i2c_regs.set_clock_stretch_timeout(0U));
  CHECK(i2c_regs.clk_stretch==0U);
  CHECK_FALSE(i2c_regs.set_clock_stretch_timeout(~0U));
  CHECK(i2c_regs.clk_stretch==0U);
  CHECK_FALSE(i2c_regs.set_clock_stretch_timeout(i2c_regs.clkt_tout_mask+1));
  CHECK(i2c_regs.clk_stretch==0U);
}
