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

TEST_CASE( "Unit-tests/pwm_registers/0010/get_enable"
         , "get_enable returns correct state for control register PWENi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 1U;       // bit 0==1: PWEN1 true
  CHECK(pwm_regs.get_enable(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~1U;      // bit 0==0: PWEN1 false
  CHECK_FALSE(pwm_regs.get_enable(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x100U;   // bit 8==1: PWEN2 true
  CHECK(pwm_regs.get_enable(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x100U;  // bit 8==0: PWEN2 false
  CHECK_FALSE(pwm_regs.get_enable(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0020/get_mode"
         , "get_mode returns correct state for control register MODEi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 2U;       // bit 1==1: MODE1 true
  CHECK(pwm_regs.get_mode(pwm_channel::gpio_pwm0)==pwm_mode::serialiser);
  pwm_regs.control = ~2U;      // bit 1==0: MODE1 false
  CHECK(pwm_regs.get_mode(pwm_channel::pwm_ch1)==pwm_mode::pwm);

  pwm_regs.control = 0x200U;   // bit 9==1: MODE2 true
  CHECK(pwm_regs.get_mode(pwm_channel::gpio_pwm1)==pwm_mode::serialiser);
  pwm_regs.control = ~0x200U;  // bit 9==0: MODE2 false
  CHECK(pwm_regs.get_mode(pwm_channel::pwm_ch2)==pwm_mode::pwm);
}

TEST_CASE( "Unit-tests/pwm_registers/0030/get_repeat_last_data"
         , "get_repeat_last_data returns correct state for control register "
           "RPTLi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 4U;       // bit 2==1: RPTL1 true
  CHECK(pwm_regs.get_repeat_last_data(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~4U;      // bit 2==0: RPTL1 false
  CHECK_FALSE(pwm_regs.get_repeat_last_data(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x400U;   // bit 10==1: RPTL2 true
  CHECK(pwm_regs.get_repeat_last_data(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x400U;  // bit 10==0: RPTL2 false
  CHECK_FALSE(pwm_regs.get_repeat_last_data(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0040/get_silence"
         , "get_silence returns correct state for control register "
           "SBITi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 8U;       // bit 3==1: SBIT1 true
  CHECK(pwm_regs.get_silence(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~8U;      // bit 3==0: SBIT1 false
  CHECK_FALSE(pwm_regs.get_silence(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x800U;   // bit 11==1: SBIT2 true
  CHECK(pwm_regs.get_silence(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x800U;  // bit 11==0: SBIT2 false
  CHECK_FALSE(pwm_regs.get_silence(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0050/get_polarity_inverted"
         , "get_polarity_inverted returns correct state for control register "
           "POLAi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 0x10U;    // bit 4==1: POLA1 true
  CHECK(pwm_regs.get_polarity_inverted(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~0x10U;   // bit 4==0: POLA1 false
  CHECK_FALSE(pwm_regs.get_polarity_inverted(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x1000U;  // bit 12==1: POLA2 true
  CHECK(pwm_regs.get_polarity_inverted(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x1000U; // bit 12==0: POLA2 false
  CHECK_FALSE(pwm_regs.get_polarity_inverted(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0050/get_use_fifo"
         , "get_use_fifo returns correct state for control register "
           "USEFi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 0x20U;    // bit 5==1: USEF1 true
  CHECK(pwm_regs.get_use_fifo(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~0x20U;   // bit 5==0: USEF1 false
  CHECK_FALSE(pwm_regs.get_use_fifo(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x2000U;  // bit 13==1: USEF2 true
  CHECK(pwm_regs.get_use_fifo(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x2000U; // bit 13==0: USEF2 false
  CHECK_FALSE(pwm_regs.get_use_fifo(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0060/get_ms_enabled"
         , "get_ms_enabled returns correct state for control register "
           "MSENi bits"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.control = 0x80U;    // bit 7==1: MSEN1 true
  CHECK(pwm_regs.get_ms_enabled(pwm_channel::gpio_pwm0));
  pwm_regs.control = ~0x80U;   // bit 7==0: MSEN1 false
  CHECK_FALSE(pwm_regs.get_ms_enabled(pwm_channel::pwm_ch1));

  pwm_regs.control = 0x8000U;  // bit 15==1: MSEN2 true
  CHECK(pwm_regs.get_ms_enabled(pwm_channel::gpio_pwm1));
  pwm_regs.control = ~0x8000U; // bit 15==0: MSEN2 false
  CHECK_FALSE(pwm_regs.get_ms_enabled(pwm_channel::pwm_ch2));
}

TEST_CASE( "Unit-tests/pwm_registers/0070/clear_fifo"
         , "clear_fifo should set the CLRF1 bit (bit 6) of the control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));
  pwm_regs.clear_fifo();
  CHECK(pwm_regs.control==0x40);
}

TEST_CASE( "Unit-tests/pwm_registers/0080/set_enable"
         , "set_enable sets correct state in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_enable(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x1U);
  pwm_regs.set_enable(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_enable(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x100U);
  pwm_regs.set_enable(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0090/set_mode"
         , "set_mode sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_mode(pwm_channel::gpio_pwm0, pwm_mode::serialiser);
  CHECK(pwm_regs.control==0x2U);
  pwm_regs.set_mode(pwm_channel::pwm_ch1, pwm_mode::pwm);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_mode(pwm_channel::gpio_pwm1, pwm_mode::serialiser);
  CHECK(pwm_regs.control==0x200U);
  pwm_regs.set_mode(pwm_channel::pwm_ch2, pwm_mode::pwm);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0100/set_repeat_last_data"
         , "set_repeat_last_data sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_repeat_last_data(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x4U);
  pwm_regs.set_repeat_last_data(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_repeat_last_data(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x400U);
  pwm_regs.set_repeat_last_data(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0110/set_silence"
         , "set_silence sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_silence(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x8U);
  pwm_regs.set_silence(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_silence(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x800U);
  pwm_regs.set_silence(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0120/set_polarity_inverted"
         , "set_polarity_inverted sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_polarity_inverted(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x10U);
  pwm_regs.set_polarity_inverted(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_polarity_inverted(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x1000U);
  pwm_regs.set_polarity_inverted(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0130/set_use_fifo"
         , "set_use_fifo sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_use_fifo(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x20U);
  pwm_regs.set_use_fifo(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_use_fifo(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x2000U);
  pwm_regs.set_use_fifo(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}

TEST_CASE( "Unit-tests/pwm_registers/0140/set_ms_enabled"
         , "set_ms_enabled sets correct bits in control register"
         )
{
  pwm_registers pwm_regs;
  std::memset(&pwm_regs, 0x00U, sizeof(pwm_regs));

  pwm_regs.set_ms_enabled(pwm_channel::gpio_pwm0, true);
  CHECK(pwm_regs.control==0x80U);
  pwm_regs.set_ms_enabled(pwm_channel::pwm_ch1, false);
  CHECK(pwm_regs.control==0U);

  pwm_regs.set_ms_enabled(pwm_channel::gpio_pwm1, true);
  CHECK(pwm_regs.control==0x8000U);
  pwm_regs.set_ms_enabled(pwm_channel::pwm_ch2, false);
  CHECK(pwm_regs.control==0U);
}
