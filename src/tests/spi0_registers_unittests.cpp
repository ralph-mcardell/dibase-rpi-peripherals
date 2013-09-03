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
