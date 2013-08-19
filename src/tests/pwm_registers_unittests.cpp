// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_registers_unittests.cpp 
/// @brief Unit tests for low-level PWM controller registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 9 Pulse Width Modifier
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "pwm_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals;

typedef uint32_t RegisterType;
typedef unsigned char Byte;

// Register byte offsets, see BCM2835 peripherals manual PWM Address Map table
// in section 9.6 Control and Status Registers
enum RegisterOffsets 
{  CTL_OFFSET=0x00,  STA_OFFSET=0x04, DMAC_OFFSET=0x08
, RNG1_OFFSET=0x10, DAT1_OFFSET=0x14, FIF1_OFFSET=0x18
, RNG2_OFFSET=0x20, DAT2_OFFSET=0x24
};

TEST_CASE( "Unit-tests/pwm_registers/0000/field offsets"
         , "PWM controller registers should have the expected offsets"
         )
{
  pwm_registers pwm_regs;
// initially start with all bytes of pwm_regs set to 0xFF:
  std::memset(&pwm_regs, 0xFF, sizeof(pwm_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&pwm_regs));
 
// Set each 32-bit register to the value of its offset:
  pwm_regs.control = CTL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[CTL_OFFSET])==CTL_OFFSET );
  pwm_regs.status = STA_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[STA_OFFSET])==STA_OFFSET );
  pwm_regs.dma_config = DMAC_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DMAC_OFFSET])==DMAC_OFFSET );

  pwm_regs.range1 = RNG1_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[RNG1_OFFSET])==RNG1_OFFSET );
  pwm_regs.data1 = DAT1_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DAT1_OFFSET])==DAT1_OFFSET );
  pwm_regs.fifo_in = FIF1_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[FIF1_OFFSET])==FIF1_OFFSET );

  pwm_regs.range2 = RNG2_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[RNG2_OFFSET])==RNG2_OFFSET );
  pwm_regs.data2 = DAT2_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[DAT2_OFFSET])==DAT2_OFFSET );
 }
