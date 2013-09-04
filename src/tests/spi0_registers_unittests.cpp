// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_registers_unittests.cpp 
/// @brief Unit tests for low-level SPI master control registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 10 SPI
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "spi0_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals::internal;

typedef uint32_t RegisterType;
typedef unsigned char Byte;

// Register byte offsets, see BCM2835 peripherals manual PWM Address Map table
// in section 9.6 Control and Status Registers
enum RegisterOffsets 
{   CS_OFFSET=0x00, FIFO_OFFSET=0x04, CLK_OFFSET=0x08
, DLEN_OFFSET=0x0C, LTOH_OFFSET=0x10,  DC_OFFSET=0x14
};

TEST_CASE( "Unit-tests/spi0_registers/0000/field offsets"
         , "SPI0 interface registers should have the expected offsets"
         )
{
  spi0_registers spi0_regs;
// initially start with all bytes of spi0_regs set to 0xFF:
  std::memset(&spi0_regs, 0xFF, sizeof(spi0_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&spi0_regs));
 
// Set each 32-bit register to the value of its offset:
  spi0_regs.control_and_status = CS_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[CS_OFFSET])==CS_OFFSET );
  spi0_regs.fifo = FIFO_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[FIFO_OFFSET])==FIFO_OFFSET );
  spi0_regs.clock = CLK_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[CLK_OFFSET])==CLK_OFFSET );
  spi0_regs.data_length = DLEN_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DLEN_OFFSET])==DLEN_OFFSET );
  spi0_regs.lossi_mode_toh = LTOH_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[LTOH_OFFSET])==LTOH_OFFSET );
  spi0_regs.dma_controls = DC_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DC_OFFSET])==DC_OFFSET );
}

TEST_CASE( "Unit-tests/spi0_registers/0010/get_chip_select"
         , "get_chip_select returns correct state for control_and_status "
           "register CS bits"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 1U;     // bits [1:0] == 01
  CHECK(spi0_regs.get_chip_select()==1U);
  spi0_regs.control_and_status = ~1U;    // bits [1:0] == 10, other bits 1
  CHECK(spi0_regs.get_chip_select()==2U);
  spi0_regs.control_and_status = ~3U;    // bits [1:0] == 00, other bits 1
  CHECK(spi0_regs.get_chip_select()==0U);
  spi0_regs.control_and_status = 3U;     // bits [1:0] == 11, other bits 0
  CHECK(spi0_regs.get_chip_select()==3U);// Reserved value, should not be set
}

TEST_CASE( "Unit-tests/spi0_registers/0020/get_clock_phase"
         , "get_clock_phase returns correct state for control_and_status "
           "register CPHA bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 4U;     // bit [2] == 1
  CHECK(spi0_regs.get_clock_phase());
  spi0_regs.control_and_status = ~4U;    // bit [2] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_clock_phase());
}

TEST_CASE( "Unit-tests/spi0_registers/0030/get_clock_polarity"
         , "get_clock_polarity returns correct state for control_and_status "
           "register CPOL bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 8U;     // bit [3] == 1
  CHECK(spi0_regs.get_clock_polarity());
  spi0_regs.control_and_status = ~8U;    // bit [3] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_clock_polarity());
}

TEST_CASE( "Unit-tests/spi0_registers/0040/get_chip_select_polarity"
         , "get_chip_select_polarity returns correct state for "
           "control_and_status register CPSOL bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x40U;  // bit [6] == 1
  CHECK(spi0_regs.get_chip_select_polarity());
  spi0_regs.control_and_status = ~0x40U; // bit [6] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_chip_select_polarity());
}

TEST_CASE( "Unit-tests/spi0_registers/0050/get_transfer_active"
         , "get_transfer_active returns correct state for "
           "control_and_status register TA bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x80U;  // bit [7] == 1
  CHECK(spi0_regs.get_transfer_active());
  spi0_regs.control_and_status = ~0x80U; // bit [7] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_transfer_active());
}

TEST_CASE( "Unit-tests/spi0_registers/0060/get_dma_enable"
         , "get_dma_enable returns correct state for "
           "control_and_status register DMAEN bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x100U;  // bit [8] == 1
  CHECK(spi0_regs.get_dma_enable());
  spi0_regs.control_and_status = ~0x100U; // bit [8] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_dma_enable());
}

TEST_CASE( "Unit-tests/spi0_registers/0070/get_interrupt_on_done"
         , "get_interrupt_on_done returns correct state for "
           "control_and_status register INTD bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x200U;  // bit [9] == 1
  CHECK(spi0_regs.get_interrupt_on_done());
  spi0_regs.control_and_status = ~0x200U; // bit [9] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_interrupt_on_done());
}

TEST_CASE( "Unit-tests/spi0_registers/0080/get_interrupt_on_rxr"
         , "get_interrupt_on_rxr returns correct state for "
           "control_and_status register INTR bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x400U;  // bit [10] == 1
  CHECK(spi0_regs.get_interrupt_on_rxr());
  spi0_regs.control_and_status = ~0x400U; // bit [10] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_interrupt_on_rxr());
}

TEST_CASE( "Unit-tests/spi0_registers/0090/get_auto_deassert_chip_select"
         , "get_auto_deassert_chip_select returns correct state for "
           "control_and_status register ADCS bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x800U;  // bit [11] == 1
  CHECK(spi0_regs.get_auto_deassert_chip_select());
  spi0_regs.control_and_status = ~0x800U; // bit [11] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_auto_deassert_chip_select());
}

TEST_CASE( "Unit-tests/spi0_registers/0100/get_read_enable"
         , "get_read_enable returns correct state for "
           "control_and_status register REN bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x1000U;  // bit [12] == 1
  CHECK(spi0_regs.get_read_enable());
  spi0_regs.control_and_status = ~0x1000U; // bit [12] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_read_enable());
}

TEST_CASE( "Unit-tests/spi0_registers/0110/get_lossi_enable"
         , "get_lossi_enable returns correct state for "
           "control_and_status register LEN bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x2000U;  // bit [13] == 1
  CHECK(spi0_regs.get_lossi_enable());
  spi0_regs.control_and_status = ~0x2000U; // bit [13] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_lossi_enable());
}

TEST_CASE( "Unit-tests/spi0_registers/0120/get_transfer_done"
         , "get_transfer_done returns correct state for "
           "control_and_status register LEN bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x10000U;  // bit [16] == 1
  CHECK(spi0_regs.get_transfer_done());
  spi0_regs.control_and_status = ~0x10000U; // bit [13] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_transfer_done());
}

TEST_CASE( "Unit-tests/spi0_registers/0130/get_rx_fifo_not_empty"
         , "get_rx_fifo_not_empty returns correct state for "
           "control_and_status register RXD bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x20000U;  // bit [17] == 1
  CHECK(spi0_regs.get_rx_fifo_not_empty());
  spi0_regs.control_and_status = ~0x20000U; // bit [17] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_rx_fifo_not_empty());
}

TEST_CASE( "Unit-tests/spi0_registers/0140/get_tx_fifo_not_full"
         , "get_tx_fifo_not_full returns correct state for "
           "control_and_status register TXD bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x40000U;  // bit [18] == 1
  CHECK(spi0_regs.get_tx_fifo_not_full());
  spi0_regs.control_and_status = ~0x40000U; // bit [18] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_tx_fifo_not_full());
}

TEST_CASE( "Unit-tests/spi0_registers/0150/get_rx_fifo_needs_reading"
         , "get_rx_fifo_needs_reading returns correct state for "
           "control_and_status register RXR bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x80000U;  // bit [19] == 1
  CHECK(spi0_regs.get_rx_fifo_needs_reading());
  spi0_regs.control_and_status = ~0x80000U; // bit [19] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_rx_fifo_needs_reading());
}

TEST_CASE( "Unit-tests/spi0_registers/0160/get_rx_fifo_full"
         , "get_rx_fifo_full returns correct state for "
           "control_and_status register RXF bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x100000U;  // bit [20] == 1
  CHECK(spi0_regs.get_rx_fifo_full());
  spi0_regs.control_and_status = ~0x100000U; // bit [20] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_rx_fifo_full());
}

TEST_CASE( "Unit-tests/spi0_registers/0170/get_lossi_dma_enable"
         , "get_lossi_dma_enable returns correct state for "
           "control_and_status register DMA_LEN bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x1000000U;  // bit [24] == 1
  CHECK(spi0_regs.get_lossi_dma_enable());
  spi0_regs.control_and_status = ~0x1000000U; // bit [24] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_lossi_dma_enable());
}

TEST_CASE( "Unit-tests/spi0_registers/0180/get_lossi_long_word"
         , "get_lossi_long_word returns correct state for "
           "control_and_status register LEN_LONG bit"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x2000000U;  // bit [25] == 1
  CHECK(spi0_regs.get_lossi_long_word());
  spi0_regs.control_and_status = ~0x2000000U; // bit [25] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_lossi_long_word());
}

TEST_CASE( "Unit-tests/spi0_registers/0190/get_chip_select_polarity"
         , "get_chip_select_polarity returns correct state for "
           "control_and_status register CSPOL[0,1,2] bits"
         )
{
  spi0_registers spi0_regs;
  std::memset(&spi0_regs, 0x00U, sizeof(spi0_regs));
  spi0_regs.control_and_status = 0x200000U;  // bit [21] == 1 (CSPOL0)
  CHECK(spi0_regs.get_chip_select_polarity(0));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(1));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(2));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(3));
  spi0_regs.control_and_status = 0x400000U;  // bit [22] == 1 (CSPOL1)
  CHECK(spi0_regs.get_chip_select_polarity(1));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(0));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(2));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(3));
  spi0_regs.control_and_status = 0x800000U;  // bit [23] == 1 (CSPOL2)
  CHECK(spi0_regs.get_chip_select_polarity(2));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(0));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(1));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(3));
  spi0_regs.control_and_status = ~0xe00000U; // bits [21:23] == 0, other bits 1
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(2));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(0));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(1));
  CHECK_FALSE(spi0_regs.get_chip_select_polarity(3));
}
