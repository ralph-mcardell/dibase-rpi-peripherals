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
         , "get_transfer_type returns the expected value for the set C "
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
         , "get_interrupt_on_done returns the expected value for the set C "
           "register INTD field value"
         )
{
  i2c_registers i2c_regs;
  std::memset(&i2c_regs, 0x0, sizeof(i2c_regs));
  CHECK(i2c_regs.get_interrupt_on_done()==false);
  i2c_regs.control=1U<<7;
  CHECK(i2c_regs.get_interrupt_on_done()==true);
  i2c_regs.control=~(1U<<7);
  CHECK(i2c_regs.get_interrupt_on_done()==false);
  i2c_regs.control=~0U;
  CHECK(i2c_regs.get_interrupt_on_done()==true);
}
