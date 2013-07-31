// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_registers_unittests.cpp 
/// @brief Unit tests for low-level clock manager control registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 6 General Purpose I/O (GPIO) 
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "clock_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals;

typedef uint32_t RegisterType;
typedef unsigned char Byte;

TEST_CASE( "Unit-tests/clock_record/0000/is_busy"
         , "Clock control BUSY bit read and returned correctly by is_busy"
         )
{
  clock_record cr{128U,0}; // BUSY flag is control register bit 7
  CHECK(cr.is_busy());
  cr.control &= ~128U;
  CHECK_FALSE(cr.is_busy());
}

TEST_CASE( "Unit-tests/clock_record/0010/get_enable"
         , "Clock control ENAB bit read and returned correctly by get_enable"
         )
{
  clock_record cr{16U,0}; // ENAB bit is control register bit 4
  CHECK(cr.get_enable());
  cr.control &= ~16U;
  CHECK_FALSE(cr.get_enable());
}

TEST_CASE( "Unit-tests/clock_record/0020/get_kill"
         , "Clock control KILL bit read and returned correctly by get_kill"
         )
{
  clock_record cr{32U,0}; // KILL bit is control register bit 5
  CHECK(cr.get_kill());
  cr.control &= ~32U;
  CHECK_FALSE(cr.get_kill());
}

TEST_CASE( "Unit-tests/clock_record/0030/get_flip"
         , "Clock control FLIP bit read and returned correctly by get_flip"
         )
{
  clock_record cr{256U,0}; // FLIP bit is control register bit 8
  CHECK(cr.get_flip());
  cr.control &= ~256U;
  CHECK_FALSE(cr.get_flip());
}

TEST_CASE( "Unit-tests/clock_record/0040/set_enable"
         , "Clock control ENAB bit written correctly by set_enable"
         )
{
  clock_record cr{0,0};
  CHECK(cr.set_enable(true));
  CHECK(cr.get_enable());
  CHECK(cr.control==0x5a000010U);//0x5a000000 is write password; 0x10 is ENAB bit
  CHECK(cr.set_enable(false, busy_override::yes));
  CHECK_FALSE(cr.get_enable());
  CHECK(cr.control==0x5a000000U);//0x5a000000 is write password; ENAB bit is off
}

TEST_CASE( "Unit-tests/clock_record/0050/set_kill"
         , "Clock control KILL bit written correctly by set_kill"
         )
{
  clock_record cr{0,0};
  cr.set_kill(true);
  CHECK(cr.get_kill());
  CHECK(cr.control==0x5a000020U);//0x5a000000 is write password; 0x20 is KILL bit
  cr.set_kill(false);
  CHECK_FALSE(cr.get_kill());
  CHECK(cr.control==0x5a000000U);//0x5a000000 is write password; KILL bit is off
}

TEST_CASE( "Unit-tests/clock_record/0060/set_flip"
         , "Clock control FLIP bit written correctly by set_flip"
         )
{
  clock_record cr{0,0};
  CHECK(cr.set_flip(true));
  CHECK(cr.get_flip());
  CHECK(cr.control==0x5a000100U);//0x5a000000 is write password; 0x100 is FLIP bit
  CHECK(cr.set_flip(false, busy_override::yes));
  CHECK_FALSE(cr.get_flip());
  CHECK(cr.control==0x5a000000U);//0x5a000000 is write password; FLIP bit is off
}

TEST_CASE( "Unit-tests/clock_record/0060/set_enable for busy clocks"
         , "Check set_anable behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0}; // control bit 7 is BUSY flag.
  CHECK_FALSE(cr.set_enable(true));
  CHECK_FALSE(cr.get_enable());
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_enable(false));
  CHECK_FALSE(cr.get_enable());
  CHECK(cr.control==128U);// BUSY and not forced: no write occured

  CHECK(cr.set_enable(true, busy_override::yes));
  CHECK(cr.get_enable());
  CHECK(cr.control==0x5a000090U);//0x5a000000=write password; 0x90=ENAB|BUSY
  CHECK(cr.set_enable(false, busy_override::yes));
  CHECK_FALSE(cr.get_enable());
  CHECK(cr.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, ENAB off
}

TEST_CASE( "Unit-tests/clock_record/0060/set_flip for busy clocks"
         , "Check set_flip bit behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0};
  CHECK_FALSE(cr.set_flip(true));
  CHECK_FALSE(cr.get_flip());
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_flip(false));
  CHECK_FALSE(cr.get_flip());
  CHECK(cr.control==128U);// BUSY and not forced: no write occured

  CHECK(cr.set_flip(true, busy_override::yes));
  CHECK(cr.get_flip());
  CHECK(cr.control==0x5a000180U);//0x5a000000=write password; 0x180=FLIP|BUSY
  CHECK(cr.set_flip(false, busy_override::yes));
  CHECK_FALSE(cr.get_flip());
  CHECK(cr.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, FLIP off
}

enum RegisterOffsets // Byte offsets
{ PWM_CTRL_OFFSET=40*4, PWM_DIV_OFFSET=41*4  // NB: NOT HEX values!!
, GP0_CTRL_OFFSET=0x70, GP0_DIV_OFFSET=0x74
, GP1_CTRL_OFFSET=0x78, GP1_DIV_OFFSET=0x7c
, GP2_CTRL_OFFSET=0x80, GP2_DIV_OFFSET=0x84
};

TEST_CASE( "Unit-tests/clock_registers/0000/field offsets"
         , "Clock control and divisor fields should have the expected offsets"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&clk_regs, 0xFF, sizeof(clk_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&clk_regs));
 
// Set each 32-bit register to the value of its offset:
  clk_regs.pwm_clk.control = PWM_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_CTRL_OFFSET])==PWM_CTRL_OFFSET );
  clk_regs.pwm_clk.divisor = PWM_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_DIV_OFFSET])==PWM_DIV_OFFSET );

  clk_regs.gp0_clk.control = GP0_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_CTRL_OFFSET])==GP0_CTRL_OFFSET );
  clk_regs.gp0_clk.divisor = GP0_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_DIV_OFFSET])==GP0_DIV_OFFSET );

  clk_regs.gp1_clk.control = GP1_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_CTRL_OFFSET])==GP1_CTRL_OFFSET );
  clk_regs.gp1_clk.divisor = GP1_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_DIV_OFFSET])==GP1_DIV_OFFSET );

  clk_regs.gp2_clk.control = GP2_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_CTRL_OFFSET])==GP2_CTRL_OFFSET );
  clk_regs.gp2_clk.divisor = GP2_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_DIV_OFFSET])==GP2_DIV_OFFSET );
 }
 
TEST_CASE( "Unit-tests/clock_registers/0010/pre-defined clock_id constants correct"
         , "Defined clock id member pointers should refer to the expected fields"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&clk_regs, 0xFF, sizeof(clk_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&clk_regs));
 
// Set each 32-bit register to the value of its offset:
  (clk_regs.*pwm_clk_id).control = PWM_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_CTRL_OFFSET])==PWM_CTRL_OFFSET );
  (clk_regs.*pwm_clk_id).divisor = PWM_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[PWM_DIV_OFFSET])==PWM_DIV_OFFSET );

  (clk_regs.*gp0_clk_id).control = GP0_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_CTRL_OFFSET])==GP0_CTRL_OFFSET );
  (clk_regs.*gp0_clk_id).divisor = GP0_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP0_DIV_OFFSET])==GP0_DIV_OFFSET );

  (clk_regs.*gp1_clk_id).control = GP1_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_CTRL_OFFSET])==GP1_CTRL_OFFSET );
  (clk_regs.*gp1_clk_id).divisor = GP1_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP1_DIV_OFFSET])==GP1_DIV_OFFSET );

  (clk_regs.*gp2_clk_id).control = GP2_CTRL_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_CTRL_OFFSET])==GP2_CTRL_OFFSET );
  (clk_regs.*gp2_clk_id).divisor = GP2_DIV_OFFSET;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GP2_DIV_OFFSET])==GP2_DIV_OFFSET );
 }
 
 TEST_CASE( "Unit-tests/clock_registers/0020/is_busy"
         , "Control BUSY bit read and returned OK by is_busy for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.pwm_clk.control |= 128U;// BUSY flag is control register bit 7
  CHECK(clk_regs.is_busy(pwm_clk_id));
  clk_regs.pwm_clk.control &= ~128U;
  CHECK_FALSE(clk_regs.is_busy(pwm_clk_id));

  clk_regs.gp0_clk.control |= 128U;
  CHECK(clk_regs.is_busy(gp0_clk_id));
  clk_regs.gp0_clk.control &= ~128U;
  CHECK_FALSE(clk_regs.is_busy(gp0_clk_id));

  clk_regs.gp1_clk.control |= 128U;
  CHECK(clk_regs.is_busy(gp1_clk_id));
  clk_regs.gp1_clk.control &= ~128U;
  CHECK_FALSE(clk_regs.is_busy(gp1_clk_id));

  clk_regs.gp2_clk.control |= 128U;
  CHECK(clk_regs.is_busy(gp2_clk_id));
  clk_regs.gp2_clk.control &= ~128U;
  CHECK_FALSE(clk_regs.is_busy(gp2_clk_id));
}

 TEST_CASE( "Unit-tests/clock_registers/0030/get_enable"
         , "Control ENAB bit read and returned OK by get_enable for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.pwm_clk.control |= 16U;// ENAB bit is control register bit 4
  CHECK(clk_regs.get_enable(pwm_clk_id));
  clk_regs.pwm_clk.control &= ~16U;
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));

  clk_regs.gp0_clk.control |= 16U;
  CHECK(clk_regs.get_enable(gp0_clk_id));
  clk_regs.gp0_clk.control &= ~16U;
  CHECK_FALSE(clk_regs.get_enable(gp0_clk_id));

  clk_regs.gp1_clk.control |= 16U;
  CHECK(clk_regs.get_enable(gp1_clk_id));
  clk_regs.gp1_clk.control &= ~16U;
  CHECK_FALSE(clk_regs.get_enable(gp1_clk_id));

  clk_regs.gp2_clk.control |= 16U;
  CHECK(clk_regs.get_enable(gp2_clk_id));
  clk_regs.gp2_clk.control &= ~16U;
  CHECK_FALSE(clk_regs.get_enable(gp2_clk_id));
}

 TEST_CASE( "Unit-tests/clock_registers/0040/get_kill"
         , "Control KILL bit read and returned OK by get_kill for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.pwm_clk.control |= 32U;// KILL bit is control register bit 5
  CHECK(clk_regs.get_kill(pwm_clk_id));
  clk_regs.pwm_clk.control &= ~32U;
  CHECK_FALSE(clk_regs.get_kill(pwm_clk_id));

  clk_regs.gp0_clk.control |= 32U;
  CHECK(clk_regs.get_kill(gp0_clk_id));
  clk_regs.gp0_clk.control &= ~32U;
  CHECK_FALSE(clk_regs.get_kill(gp0_clk_id));
 
  clk_regs.gp1_clk.control |= 32U;
  CHECK(clk_regs.get_kill(gp1_clk_id));
  clk_regs.gp1_clk.control &= ~32U;
  CHECK_FALSE(clk_regs.get_kill(gp1_clk_id));

  clk_regs.gp2_clk.control |= 32U;
  CHECK(clk_regs.get_kill(gp2_clk_id));
  clk_regs.gp2_clk.control &= ~32U;
  CHECK_FALSE(clk_regs.get_kill(gp2_clk_id));
}

 TEST_CASE( "Unit-tests/clock_registers/0050/get_flip"
         , "Control FLIP bit read and returned OK by get_flip for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.pwm_clk.control |= 256U;// FLIP bit is control register bit 8
  CHECK(clk_regs.get_flip(pwm_clk_id));
  clk_regs.pwm_clk.control &= ~256U;
  CHECK_FALSE(clk_regs.get_flip(pwm_clk_id));

  clk_regs.gp0_clk.control |= 256U;
  CHECK(clk_regs.get_flip(gp0_clk_id));
  clk_regs.gp0_clk.control &= ~256U;
  CHECK_FALSE(clk_regs.get_flip(gp0_clk_id));

  clk_regs.gp1_clk.control |= 256U;
  CHECK(clk_regs.get_flip(gp1_clk_id));
  clk_regs.gp1_clk.control &= ~256U;
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));

  clk_regs.gp2_clk.control |= 256U;
  CHECK(clk_regs.get_flip(gp2_clk_id));
  clk_regs.gp2_clk.control &= ~256U;
  CHECK_FALSE(clk_regs.get_flip(gp2_clk_id));
}

TEST_CASE( "Unit-tests/clock_registers/0060/set_enable"
         , "Control ENAB bit modified OK by set_enable for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  CHECK(clk_regs.set_enable(pwm_clk_id, true));
  CHECK(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000010U);//0x5a000000 is write passwd; 0x10 is ENAB bit
  CHECK(clk_regs.set_enable(pwm_clk_id, false));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000000U);//0x5a000000 is write passwd; ENAB bit off
  clk_regs.pwm_clk.control=0x12345678U;

  CHECK(clk_regs.set_enable(gp0_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_enable(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000010U);
  CHECK(clk_regs.set_enable(gp0_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_enable(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000000U);
  clk_regs.gp0_clk.control=0x12345678U;

  clk_regs.set_enable(gp1_clk_id, true);
  CHECK(clk_regs.get_enable(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000010U);
  clk_regs.set_enable(gp1_clk_id, false);
  CHECK_FALSE(clk_regs.get_enable(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000000U);
  clk_regs.gp1_clk.control=0x12345678U;

  clk_regs.set_enable(gp2_clk_id, true);
  CHECK(clk_regs.get_enable(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000010U);
  clk_regs.set_enable(gp2_clk_id, false);
  CHECK_FALSE(clk_regs.get_enable(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_registers/0070/set_kill"
         , "Control KILL bit modified OK by set_kill for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.set_kill(pwm_clk_id, true);
  CHECK(clk_regs.get_kill(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000020U);//0x5a000000 is write passwd; 0x20 is KILL bit
  clk_regs.set_kill(pwm_clk_id, false);
  CHECK_FALSE(clk_regs.get_kill(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000000U);//0x5a000000 is write passwd; KILL bit off
  clk_regs.pwm_clk.control=0x12345678U;

  clk_regs.set_kill(gp0_clk_id, true);
  CHECK(clk_regs.get_kill(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000020U);
  clk_regs.set_kill(gp0_clk_id, false);
  CHECK_FALSE(clk_regs.get_kill(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000000U);
  clk_regs.gp0_clk.control=0x12345678U;

  clk_regs.set_kill(gp1_clk_id, true);
  CHECK(clk_regs.get_kill(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000020U);
  clk_regs.set_kill(gp1_clk_id, false);
  CHECK_FALSE(clk_regs.get_kill(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000000U);
  clk_regs.gp1_clk.control=0x12345678U;

  clk_regs.set_kill(gp2_clk_id, true);
  CHECK(clk_regs.get_kill(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000020U);
  clk_regs.set_kill(gp2_clk_id, false);
  CHECK_FALSE(clk_regs.get_kill(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_registers/0080/set_flip"
         , "Control FLIP bit modified OK by set_flip for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  CHECK(clk_regs.set_flip(pwm_clk_id, true));
  CHECK(clk_regs.get_flip(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000100U);//0x5a000000 is write passwd; 0x100 is FLIP bit
  CHECK(clk_regs.set_flip(pwm_clk_id, false));
  CHECK_FALSE(clk_regs.get_flip(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000000U);//0x5a000000 is write passwd; ENAB bit off
  clk_regs.pwm_clk.control=0x12345678U;

  CHECK(clk_regs.set_flip(gp0_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_flip(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000100U);
  CHECK(clk_regs.set_flip(gp0_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_flip(gp0_clk_id));
  CHECK(clk_regs.gp0_clk.control==0x5a000000U);
  clk_regs.gp0_clk.control=0x12345678U;

  clk_regs.set_flip(gp1_clk_id, true);
  CHECK(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000100U);
  clk_regs.set_flip(gp1_clk_id, false);
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000000U);
  clk_regs.gp1_clk.control=0x12345678U;

  clk_regs.set_flip(gp2_clk_id, true);
  CHECK(clk_regs.get_flip(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000100U);
  clk_regs.set_flip(gp2_clk_id, false);
  CHECK_FALSE(clk_regs.get_flip(gp2_clk_id));
  CHECK(clk_regs.gp2_clk.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_registers/0090/set_enable for busy clock"
         , "Check set_flip bit behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.pwm_clk.control = 128; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_enable(pwm_clk_id, true));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==128);// BUSY and not forced: no write occured
  CHECK_FALSE(clk_regs.set_enable(pwm_clk_id, false));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==128);// BUSY and not forced: no write occured

  CHECK(clk_regs.set_enable(pwm_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000090U);//0x5a000000=write password; 0x90=ENAB|BUSY
  CHECK(clk_regs.set_enable(pwm_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, ENAB off
}

TEST_CASE( "Unit-tests/clock_registers/0100/set_flip for busy clock"
         , "Check set_flip bit behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of gpio_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp1_clk.control = 128; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_flip(gp1_clk_id, true));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==128);// BUSY and not forced: no write occured
  CHECK_FALSE(clk_regs.set_flip(gp1_clk_id, false));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==128);// BUSY and not forced: no write occured

  CHECK(clk_regs.set_flip(gp1_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000180U);//0x5a000000=write password; 0x180=FLIP|BUSY
  CHECK(clk_regs.set_flip(gp1_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, FLIP off
}
