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

TEST_CASE( "Unit-tests/clock_record/0040/get_mash"
         , "Clock control MASH field read and returned correctly by get_mash"
         )
{
  clock_record cr{1024U|512U,0}; // MASH bits are control register bits 9 & 10
  CHECK(cr.get_mash()==clock_mash_mode::mash_3_stage);
  cr.control &= ~512U;
  CHECK(cr.get_mash()==clock_mash_mode::mash_2_stage);
  cr.control &= ~1024U;
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);
  cr.control = 512U;
  CHECK(cr.get_mash()==clock_mash_mode::mash_1_stage);
}

TEST_CASE( "Unit-tests/clock_record/0050/get_source"
         , "Clock control SRC field read and returned correctly by get_source"
         )
{
  clock_record cr{15U,0}; // SRC bits are control register bits 0..3
  for (;cr.control!=7;--cr.control)
    { /* SRC values 15...8 all map to clock_src::gnd */
      CHECK(cr.get_source()==clock_src::gnd);
    }
  CHECK(cr.get_source()==clock_src::hdmi_aux);
  --cr.control;
  CHECK(cr.get_source()==clock_src::plld);
  --cr.control;
  CHECK(cr.get_source()==clock_src::pllc);
  --cr.control;
  CHECK(cr.get_source()==clock_src::plla);
  --cr.control;
  CHECK(cr.get_source()==clock_src::testdebug1);
  --cr.control;
  CHECK(cr.get_source()==clock_src::testdebug0);
  --cr.control;
  CHECK(cr.get_source()==clock_src::oscillator);
  --cr.control;
  CHECK(cr.get_source()==clock_src::gnd);
}

TEST_CASE( "Unit-tests/clock_record/0060/get_divi"
         , "Clock divisor DIVI field read and returned correctly by get_divi"
         )
{
  clock_record cr{0x88123123U,0xDEDU};
  CHECK(cr.get_divi()==0); // Real clock DIVI value should never be 0
  cr.divisor = 0xFFF000U;
  CHECK(cr.get_divi()==0xFFFU);
  cr.divisor = 0x765000U;
  CHECK(cr.get_divi()==0x765U);
  cr.divisor = 0x55444333U;
  CHECK(cr.get_divi()==0x444U);
}

TEST_CASE( "Unit-tests/clock_record/0070/get_divf"
         , "Clock divisor DIVF field read and returned correctly by get_divf"
         )
{
  clock_record cr{0x88123123U,0xDED000U};
  CHECK(cr.get_divf()==0); // Real clock DIVI value should never be 0
  cr.divisor = 0xFFFU;
  CHECK(cr.get_divf()==0xFFFU);
  cr.divisor = 0x765U;
  CHECK(cr.get_divf()==0x765U);
  cr.divisor = 0x55444333U;
  CHECK(cr.get_divf()==0x333U);
}

TEST_CASE( "Unit-tests/clock_record/0200/set_enable"
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

TEST_CASE( "Unit-tests/clock_record/0210/set_kill"
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

TEST_CASE( "Unit-tests/clock_record/0220/set_flip"
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

TEST_CASE( "Unit-tests/clock_record/0230/set_mash"
         , "Clock control MASH mode value written correctly by set_mash"
         )
{
  clock_record cr{0,0};
  CHECK(cr.set_mash(clock_mash_mode::mash_3_stage));
  CHECK(cr.get_mash()==clock_mash_mode::mash_3_stage);
  CHECK(cr.control==0x5a000600U);//0x5a000000=write password; 0x600=MASH bits
  CHECK(cr.set_mash(clock_mash_mode::mash_2_stage, busy_override::yes));
  CHECK(cr.get_mash()==clock_mash_mode::mash_2_stage);
  CHECK(cr.control==0x5a000400U);//0x5a000000=write password; 0x400=MASH bits
  CHECK(cr.set_mash(clock_mash_mode::mash_1_stage));
  CHECK(cr.get_mash()==clock_mash_mode::mash_1_stage);
  CHECK(cr.control==0x5a000200U);//0x5a000000=write password; 0x200=MASH bits
  CHECK(cr.set_mash(clock_mash_mode::integer_division));
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);
  CHECK(cr.control==0x5a000000U);//0x5a000000=write password; 0x000=MASH bits
}

TEST_CASE( "Unit-tests/clock_record/0240/set_source"
         , "Clock control SRC value written correctly by set_source"
         )
{
  clock_record cr{0,0};
  CHECK(cr.set_source(clock_src::hdmi_aux));
  CHECK(cr.get_source()==clock_src::hdmi_aux);
  CHECK(cr.control==0x5a000007U);//0x5a000000=write password; 0x7=SRC (3) bits
  CHECK(cr.set_source(clock_src::plld));
  CHECK(cr.get_source()==clock_src::plld);
  CHECK(cr.control==0x5a000006U);
  CHECK(cr.set_source(clock_src::pllc));
  CHECK(cr.get_source()==clock_src::pllc);
  CHECK(cr.control==0x5a000005U);
  CHECK(cr.set_source(clock_src::plla));
  CHECK(cr.get_source()==clock_src::plla);
  CHECK(cr.control==0x5a000004U);
  CHECK(cr.set_source(clock_src::testdebug1));
  CHECK(cr.get_source()==clock_src::testdebug1);
  CHECK(cr.control==0x5a000003U);
  CHECK(cr.set_source(clock_src::testdebug0));
  CHECK(cr.get_source()==clock_src::testdebug0);
  CHECK(cr.control==0x5a000002U);
  CHECK(cr.set_source(clock_src::oscillator));
  CHECK(cr.get_source()==clock_src::oscillator);
  CHECK(cr.control==0x5a000001U);
  CHECK(cr.set_source(clock_src::gnd));
  CHECK(cr.get_source()==clock_src::gnd);
  CHECK(cr.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_record/0250/set_divi"
         , "Clock divisor DIVI value written correctly by set_divi"
         )
{
  clock_record cr{0,0};
  CHECK(cr.set_divi(1));
  CHECK(cr.get_divi()==1);
  CHECK(cr.divisor==0x5a001000U);//0x5a000000=write pswd; 0x001=DIVI (12) bits
  CHECK(cr.set_divi(0xfffU));
  CHECK(cr.get_divi()==0xfffU);
  CHECK(cr.divisor==0x5afff000U);
  cr.divisor = 0x123U; // Set non-divi related bits. set_divi should not modify
  CHECK(cr.set_divi(0x5a5U));
  CHECK(cr.get_divi()==0x5a5U);
  CHECK(cr.divisor==0x5a5a5123U);
  CHECK(cr.set_divi(0xa5aU));
  CHECK(cr.get_divi()==0xa5aU);
  CHECK(cr.divisor==0x5aa5a123U);
  cr.divisor = 0xffeeedddU;
  CHECK_FALSE(cr.set_divi(0U));
  CHECK(cr.get_divi()==0xeeeU); // Operation failed: no change
  CHECK(cr.divisor==0xffeeedddU);
  CHECK_FALSE(cr.set_divi(0x1000U));
  CHECK(cr.get_divi()==0xeeeU); // Operation failed: no change
  CHECK(cr.divisor==0xffeeedddU);
}

TEST_CASE( "Unit-tests/clock_record/0260/set_divf"
         , "Clock divisor DIVF value written correctly by set_divf"
         )
{
  clock_record cr{0,0xfff};
  CHECK(cr.set_divf(0));
  CHECK(cr.get_divf()==0);
  CHECK(cr.divisor==0x5a000000U);//0x5a000000=write pswd; 0x000=DIVF (12) bits
  CHECK(cr.set_divf(0xfffU));
  CHECK(cr.get_divf()==0xfffU);
  CHECK(cr.divisor==0x5a000fffU);
  cr.divisor = 0x123000U; // Set non-divf related bits. set_divf should not modify
  CHECK(cr.set_divf(0x5a5U));
  CHECK(cr.get_divf()==0x5a5U);
  CHECK(cr.divisor==0x5a1235a5U);
  CHECK(cr.set_divf(0xa5aU));
  CHECK(cr.get_divf()==0xa5aU);
  CHECK(cr.divisor==0x5a123a5aU);
  cr.divisor = 0xffeeedddU;
  CHECK_FALSE(cr.set_divf(0x1000U));
  CHECK(cr.get_divf()==0xdddU); // Operation failed: no change
  CHECK(cr.divisor==0xffeeedddU);
}

TEST_CASE( "Unit-tests/clock_record/0400/set_enable for busy clocks"
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

TEST_CASE( "Unit-tests/clock_record/0410/set_flip for busy clocks"
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

TEST_CASE( "Unit-tests/clock_record/0420/set_mash for busy clocks"
         , "Check set_mash field behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0};
  CHECK_FALSE(cr.set_mash(clock_mash_mode::mash_3_stage));
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);//integer_division=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_mash(clock_mash_mode::mash_2_stage));
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);//integer_division=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_mash(clock_mash_mode::mash_1_stage));
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);//integer_division=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  cr.control |= 512U; // set lower MASH field bit
  CHECK_FALSE(cr.set_mash(clock_mash_mode::integer_division));
  CHECK(cr.get_mash()==clock_mash_mode::mash_1_stage);
  CHECK(cr.control==(128U|512U));// BUSY and not forced: no write occured

  CHECK(cr.set_mash(clock_mash_mode::mash_3_stage, busy_override::yes));
  CHECK(cr.get_mash()==clock_mash_mode::mash_3_stage);
  CHECK(cr.control==0x5a000680U);//0x5a000000=write password; 0x600=MASH|BUSY bits
  CHECK(cr.set_mash(clock_mash_mode::mash_2_stage, busy_override::yes));
  CHECK(cr.get_mash()==clock_mash_mode::mash_2_stage);
  CHECK(cr.control==0x5a000480U);//0x5a000000=write password; 0x400=MASH|BUSY bits
  CHECK(cr.set_mash(clock_mash_mode::mash_1_stage, busy_override::yes));
  CHECK(cr.get_mash()==clock_mash_mode::mash_1_stage);
  CHECK(cr.control==0x5a000280U);//0x5a000000=write password; 0x200=MASH|BUSY bits
  CHECK(cr.set_mash(clock_mash_mode::integer_division, busy_override::yes));
  CHECK(cr.get_mash()==clock_mash_mode::integer_division);
  CHECK(cr.control==0x5a000080U);//0x5a000000=write password; 0x000=MASH|BUSY bits
}

TEST_CASE( "Unit-tests/clock_record/0430/set_source for busy clocks"
         , "Check set_source field behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0};
  CHECK_FALSE(cr.set_source(clock_src::hdmi_aux));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::plld));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::pllc));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::plla));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::testdebug1));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::testdebug0));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(cr.set_source(clock_src::oscillator));
  CHECK(cr.get_source()==clock_src::gnd); //gnd=0
  CHECK(cr.control==128U);// BUSY and not forced: no write occured
  cr.control |= 1U; // Set low bit of SRC field (oscillator)
  CHECK_FALSE(cr.set_source(clock_src::gnd));
  CHECK(cr.get_source()==clock_src::oscillator); //gnd=0
  CHECK(cr.control==(128U|1U));// BUSY and not forced: no write occured

  CHECK(cr.set_source(clock_src::hdmi_aux, busy_override::yes));
  CHECK(cr.get_source()==clock_src::hdmi_aux);
  CHECK(cr.control==0x5a000087U);//0x5a000000=write password; 0x87=BUSY|SRC bits
  CHECK(cr.set_source(clock_src::plld, busy_override::yes));
  CHECK(cr.get_source()==clock_src::plld);
  CHECK(cr.control==0x5a000086U);
  CHECK(cr.set_source(clock_src::pllc, busy_override::yes));
  CHECK(cr.get_source()==clock_src::pllc);
  CHECK(cr.control==0x5a000085U);
  CHECK(cr.set_source(clock_src::plla, busy_override::yes));
  CHECK(cr.get_source()==clock_src::plla);
  CHECK(cr.control==0x5a000084U);
  CHECK(cr.set_source(clock_src::testdebug1, busy_override::yes));
  CHECK(cr.get_source()==clock_src::testdebug1);
  CHECK(cr.control==0x5a000083U);
  CHECK(cr.set_source(clock_src::testdebug0, busy_override::yes));
  CHECK(cr.get_source()==clock_src::testdebug0);
  CHECK(cr.control==0x5a000082U);
  CHECK(cr.set_source(clock_src::oscillator, busy_override::yes));
  CHECK(cr.get_source()==clock_src::oscillator);
  CHECK(cr.control==0x5a000081U);
  CHECK(cr.set_source(clock_src::gnd, busy_override::yes));
  CHECK(cr.get_source()==clock_src::gnd);
  CHECK(cr.control==0x5a000080U);
}

TEST_CASE( "Unit-tests/clock_record/0440/set_divi for busy clocks"
         , "Check set_divi behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0};
  CHECK_FALSE(cr.set_divi(1));
  CHECK(cr.get_divi()==0);
  CHECK(cr.divisor==0);
  CHECK_FALSE(cr.set_divi(0xfffU));
  CHECK(cr.get_divi()==0);
  CHECK(cr.divisor==0);

  CHECK(cr.set_divi(1, busy_override::yes));
  CHECK(cr.get_divi()==1);
  CHECK(cr.divisor==0x5a001000U);//0x5a000000=write pswd; 0x001=DIVI (12) bits
  CHECK(cr.set_divi(0xfffU, busy_override::yes));
  CHECK(cr.get_divi()==0xfffU);
  CHECK(cr.divisor==0x5afff000U);
}

TEST_CASE( "Unit-tests/clock_record/0450/set_divf for busy clocks"
         , "Check set_divf behaviour for busy clocks is as expected"
         )
{
  clock_record cr{128U,0};
  CHECK_FALSE(cr.set_divf(1));
  CHECK(cr.get_divf()==0);
  CHECK(cr.divisor==0);
  CHECK_FALSE(cr.set_divf(0xfffU));
  CHECK(cr.get_divf()==0);
  CHECK(cr.divisor==0);

  CHECK(cr.set_divf(1, busy_override::yes));
  CHECK(cr.get_divf()==1);
  CHECK(cr.divisor==0x5a000001U);//0x5a000000=write pswd; 0x001=DIVF (12) bits
  CHECK(cr.set_divf(0xfffU, busy_override::yes));
  CHECK(cr.get_divf()==0xfffU);
  CHECK(cr.divisor==0x5a000fffU);
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
// initially start with all bytes of clk_regs set to 0xFF:
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
// initially start with all bytes of clk_regs set to 0xFF:
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
// initially start with all bytes of clk_regs set to 0x00:
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
// initially start with all bytes of clk_regs set to 0x00:
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
// initially start with all bytes of clk_regs set to 0x00:
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
// initially start with all bytes of clk_regs set to 0x00:
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

TEST_CASE( "Unit-tests/clock_registers/0060/get_mash"
         , "Control MASH field read and returned OK by get_mash for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  clk_regs.pwm_clk.control |=1024U|512U; // MASH bits are bits 9 & 10
  CHECK(clk_regs.get_mash(pwm_clk_id)==clock_mash_mode::mash_3_stage);
  clk_regs.pwm_clk.control &= ~512U;
  CHECK(clk_regs.get_mash(pwm_clk_id)==clock_mash_mode::mash_2_stage);

  clk_regs.gp1_clk.control = 512U;
  CHECK(clk_regs.get_mash(gp1_clk_id)==clock_mash_mode::mash_1_stage);
  clk_regs.gp1_clk.control &= ~512U;
  CHECK(clk_regs.get_mash(gp1_clk_id)==clock_mash_mode::integer_division);

  clk_regs.gp0_clk.control |=1024U|512U;
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_3_stage);
  clk_regs.gp0_clk.control &= ~512U;
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_2_stage);
  
  clk_regs.gp2_clk.control = 512U;
  CHECK(clk_regs.get_mash(gp2_clk_id)==clock_mash_mode::mash_1_stage);
  clk_regs.gp2_clk.control &= ~512U;
  CHECK(clk_regs.get_mash(gp2_clk_id)==clock_mash_mode::integer_division);
}


TEST_CASE( "Unit-tests/clock_registers/0070/get_source"
         , "Control SRC field read & returned OK by get_source for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.pwm_clk.control = 255U; // 255: All SRC bits plus some others
  clk_regs.gp0_clk.control = 255U;
  clk_regs.gp1_clk.control = 255U;
  clk_regs.gp2_clk.control = 255U;
  for (;(clk_regs.pwm_clk.control&15U)!=7U
      ; --clk_regs.pwm_clk.control, --clk_regs.gp0_clk.control
      , --clk_regs.gp1_clk.control, --clk_regs.gp2_clk.control
      )
    { /* Clock SRC values 15...8 all map to clock_src::gnd */
      CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::gnd);
      CHECK(clk_regs.get_source(gp1_clk_id)==clock_src::gnd);
      CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::gnd);
      CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::gnd);
    }
  CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::hdmi_aux);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::plld);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp1_clk_id)==clock_src::pllc);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::plla);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::testdebug1);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::testdebug0);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::oscillator);
  --clk_regs.pwm_clk.control; --clk_regs.gp0_clk.control;
  --clk_regs.gp1_clk.control; --clk_regs.gp2_clk.control;
  CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::gnd);
  CHECK(clk_regs.get_source(gp1_clk_id)==clock_src::gnd);
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::gnd);
  CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::gnd);
}

TEST_CASE( "Unit-tests/clock_registers/0080/get_divi"
         , "Divisor DIVI field read & returned OK by get_divi for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to some non-zero:
  std::memset(&clk_regs, 0xBA, sizeof(clk_regs));
  clk_regs.pwm_clk.divisor = 0xBAD;
  CHECK(clk_regs.get_divi(pwm_clk_id)==0); // Real clock DIVI value should never be 0
  clk_regs.gp0_clk.divisor = 0xFFF000U;
  CHECK(clk_regs.get_divi(gp0_clk_id)==0xFFFU);
  clk_regs.gp1_clk.divisor = 0x8FE000U;
  CHECK(clk_regs.get_divi(gp1_clk_id)==0x8FEU);
  clk_regs.gp2_clk.divisor = 0xAABBBCCCU;
  CHECK(clk_regs.get_divi(gp2_clk_id)==0xBBBU);
}

TEST_CASE( "Unit-tests/clock_registers/0090/get_divf"
         , "Divisor DIVF field read & returned OK by get_divf for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to some non-zero:
  std::memset(&clk_regs, 0xBA, sizeof(clk_regs));
  clk_regs.pwm_clk.divisor = 0xBAD000;
  CHECK(clk_regs.get_divf(pwm_clk_id)==0); // Real clock DIVI value should never be 0
  clk_regs.gp0_clk.divisor = 0xFFFU;
  CHECK(clk_regs.get_divf(gp0_clk_id)==0xFFFU);
  clk_regs.gp1_clk.divisor = 0x8FEU;
  CHECK(clk_regs.get_divf(gp1_clk_id)==0x8FEU);
  clk_regs.gp2_clk.divisor = 0xAABBBCCCU;
  CHECK(clk_regs.get_divf(gp2_clk_id)==0xCCCU);
}
TEST_CASE( "Unit-tests/clock_registers/0200/set_enable"
         , "Control ENAB bit modified OK by set_enable for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
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

TEST_CASE( "Unit-tests/clock_registers/0210/set_kill"
         , "Control KILL bit modified OK by set_kill for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
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

TEST_CASE( "Unit-tests/clock_registers/0220/set_flip"
         , "Control FLIP bit modified OK by set_flip for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  CHECK(clk_regs.set_flip(pwm_clk_id, true));
  CHECK(clk_regs.get_flip(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000100U);//0x5a000000 is write passwd; 0x100 is FLIP bit
  CHECK(clk_regs.set_flip(pwm_clk_id, false));
  CHECK_FALSE(clk_regs.get_flip(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000000U);//0x5a000000 is write passwd; FLIP bit off
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

TEST_CASE( "Unit-tests/clock_registers/0230/set_mash"
         , "Control MASH field modified OK by set_mash for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));

  CHECK(clk_regs.set_mash(pwm_clk_id, clock_mash_mode::mash_3_stage));
  CHECK(clk_regs.get_mash(pwm_clk_id)==clock_mash_mode::mash_3_stage);
  CHECK(clk_regs.pwm_clk.control==0x5a000600U);//0x5a000000=write passwd; 0x600=MASH bits
  CHECK(clk_regs.set_mash(pwm_clk_id, clock_mash_mode::mash_2_stage));
  CHECK(clk_regs.get_mash(pwm_clk_id)==clock_mash_mode::mash_2_stage);
  CHECK(clk_regs.pwm_clk.control==0x5a000400U);
  clk_regs.pwm_clk.control=0x12345678U;

  CHECK(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::mash_1_stage, busy_override::yes));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_1_stage);
  CHECK(clk_regs.gp0_clk.control==0x5a000200U);
  CHECK(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::integer_division, busy_override::yes));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::integer_division);
  CHECK(clk_regs.gp0_clk.control==0x5a000000U);
  clk_regs.gp0_clk.control=0x12345678U;

  clk_regs.set_mash(gp1_clk_id, clock_mash_mode::mash_2_stage);
  CHECK(clk_regs.get_mash(gp1_clk_id)==clock_mash_mode::mash_2_stage);
  CHECK(clk_regs.gp1_clk.control==0x5a000400U);
  clk_regs.set_mash(gp1_clk_id, clock_mash_mode::mash_1_stage);
  CHECK(clk_regs.get_mash(gp1_clk_id)==clock_mash_mode::mash_1_stage);
  CHECK(clk_regs.gp1_clk.control==0x5a000200U);
  clk_regs.gp1_clk.control=0x12345678U;

  clk_regs.set_mash(gp2_clk_id, clock_mash_mode::mash_3_stage);
  CHECK(clk_regs.get_mash(gp2_clk_id)==clock_mash_mode::mash_3_stage);
  CHECK(clk_regs.gp2_clk.control==0x5a000600U);
  clk_regs.set_mash(gp2_clk_id, clock_mash_mode::integer_division);
  CHECK(clk_regs.get_mash(gp2_clk_id)==clock_mash_mode::integer_division);
  CHECK(clk_regs.gp2_clk.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_registers/0240/set_source"
         , "Control SRC value written correctly by set_source for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  CHECK(clk_regs.set_source(pwm_clk_id, clock_src::hdmi_aux));
  CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::hdmi_aux);
  CHECK(clk_regs.pwm_clk.control==0x5a000007U);//0x5a000000=write password; 0x7=SRC (3) bits
  CHECK(clk_regs.set_source(pwm_clk_id,clock_src::plld));
  CHECK(clk_regs.get_source(pwm_clk_id)==clock_src::plld);
  CHECK(clk_regs.pwm_clk.control==0x5a000006U);
 
  CHECK(clk_regs.set_source(gp0_clk_id, clock_src::pllc));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::pllc);
  CHECK(clk_regs.gp0_clk.control==0x5a000005U);
  CHECK(clk_regs.set_source(gp0_clk_id, clock_src::plla));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::plla);
  CHECK(clk_regs.gp0_clk.control==0x5a000004U);

  CHECK(clk_regs.set_source(gp1_clk_id, clock_src::testdebug1));
  CHECK(clk_regs.get_source(gp1_clk_id)==clock_src::testdebug1);
  CHECK(clk_regs.gp1_clk.control==0x5a000003U);
  CHECK(clk_regs.set_source(gp1_clk_id, clock_src::testdebug0));
  CHECK(clk_regs.get_source(gp1_clk_id)==clock_src::testdebug0);
  CHECK(clk_regs.gp1_clk.control==0x5a000002U);

  CHECK(clk_regs.set_source(gp2_clk_id, clock_src::oscillator));
  CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::oscillator);
  CHECK(clk_regs.gp2_clk.control==0x5a000001U);
  CHECK(clk_regs.set_source(gp2_clk_id, clock_src::gnd));
  CHECK(clk_regs.get_source(gp2_clk_id)==clock_src::gnd);
  CHECK(clk_regs.gp2_clk.control==0x5a000000U);
}

TEST_CASE( "Unit-tests/clock_registers/0250/set_divi"
         , "Divisor DIVI value written correctly by set_divi for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  CHECK(clk_regs.set_divi(pwm_clk_id, 1));
  CHECK(clk_regs.get_divi(pwm_clk_id)==1);
  CHECK(clk_regs.pwm_clk.divisor==0x5a001000U);//0x5a000000=write pswd; 0x001=DIVI (12) bits
  CHECK(clk_regs.set_divi(gp0_clk_id, 0xfffU));
  CHECK(clk_regs.get_divi(gp0_clk_id)==0xfffU);
  CHECK(clk_regs.gp0_clk.divisor==0x5afff000U);
  clk_regs.gp1_clk.divisor = 0x123U; // Set non-divi related bits. set_divi should not modify
  CHECK(clk_regs.set_divi(gp1_clk_id,0x5a5U));
  CHECK(clk_regs.get_divi(gp1_clk_id)==0x5a5U);
  CHECK(clk_regs.gp1_clk.divisor==0x5a5a5123U);
  CHECK(clk_regs.set_divi(gp1_clk_id,0xa5aU));
  CHECK(clk_regs.get_divi(gp1_clk_id)==0xa5aU);
  CHECK(clk_regs.gp1_clk.divisor==0x5aa5a123U);
  clk_regs.gp2_clk.divisor = 0xffeeedddU;
  CHECK_FALSE(clk_regs.set_divi(gp2_clk_id, 0U));
  CHECK(clk_regs.get_divi(gp2_clk_id)==0xeeeU); // Operation failed: no change
  CHECK(clk_regs.gp2_clk.divisor==0xffeeedddU);
  CHECK_FALSE(clk_regs.set_divi(gp2_clk_id, 0x1000U));
  CHECK(clk_regs.get_divi(gp2_clk_id)==0xeeeU); // Operation failed: no change
  CHECK(clk_regs.gp2_clk.divisor==0xffeeedddU);
}

TEST_CASE( "Unit-tests/clock_registers/0260/set_divf"
         , "Divisor DIVF value written correctly by set_divf for given clock"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.pwm_clk.divisor = 0xfff;
  CHECK(clk_regs.set_divf(pwm_clk_id, 0));
  CHECK(clk_regs.get_divf(pwm_clk_id)==0);
  CHECK(clk_regs.pwm_clk.divisor==0x5a000000U);//0x5a000000=write pswd; 0x000=DIVF (12) bits
  CHECK(clk_regs.set_divf(gp0_clk_id, 0xfffU));
  CHECK(clk_regs.get_divf(gp0_clk_id)==0xfffU);
  CHECK(clk_regs.gp0_clk.divisor==0x5a000fffU);
  clk_regs.gp1_clk.divisor = 0x123000U; // Set non-divf related bits. set_divf should not modify
  CHECK(clk_regs.set_divf(gp1_clk_id,0x5a5U));
  CHECK(clk_regs.get_divf(gp1_clk_id)==0x5a5U);
  CHECK(clk_regs.gp1_clk.divisor==0x5a1235a5U);
  CHECK(clk_regs.set_divf(gp1_clk_id,0xa5aU));
  CHECK(clk_regs.get_divf(gp1_clk_id)==0xa5aU);
  CHECK(clk_regs.gp1_clk.divisor==0x5a123a5aU);
  clk_regs.gp2_clk.divisor = 0xffeeedddU;
  CHECK_FALSE(clk_regs.set_divf(gp2_clk_id, 0x1000U));
  CHECK(clk_regs.get_divf(gp2_clk_id)==0xdddU); // Operation failed: no change
  CHECK(clk_regs.gp2_clk.divisor==0xffeeedddU);
}
TEST_CASE( "Unit-tests/clock_registers/0400/set_enable for busy clock"
         , "Check set_enable bit behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.pwm_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_enable(pwm_clk_id, true));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(clk_regs.set_enable(pwm_clk_id, false));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==128U);// BUSY and not forced: no write occured

  CHECK(clk_regs.set_enable(pwm_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000090U);//0x5a000000=write password; 0x90=ENAB|BUSY
  CHECK(clk_regs.set_enable(pwm_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_enable(pwm_clk_id));
  CHECK(clk_regs.pwm_clk.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, ENAB off
}

TEST_CASE( "Unit-tests/clock_registers/0410/set_flip for busy clock"
         , "Check set_flip bit behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp1_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_flip(gp1_clk_id, true));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(clk_regs.set_flip(gp1_clk_id, false));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==128U);// BUSY and not forced: no write occured

  CHECK(clk_regs.set_flip(gp1_clk_id, true, busy_override::yes));
  CHECK(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000180U);//0x5a000000=write password; 0x180=FLIP|BUSY
  CHECK(clk_regs.set_flip(gp1_clk_id, false, busy_override::yes));
  CHECK_FALSE(clk_regs.get_flip(gp1_clk_id));
  CHECK(clk_regs.gp1_clk.control==0x5a000080U);//0x5a000000=write password; 0x80=BUSY, FLIP off
}

TEST_CASE( "Unit-tests/clock_registers/0420/set_mash for busy clock"
         , "Check set_mash field behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp0_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::mash_1_stage));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::integer_division);
  CHECK(clk_regs.gp0_clk.control==128U);// BUSY and not forced: no write occured
  clk_regs.gp0_clk.control |= 512U; // set MASH field lower bit
  CHECK_FALSE(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::integer_division));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_1_stage);
  CHECK(clk_regs.gp0_clk.control==(128U|512U));// BUSY and not forced: no write occured

  CHECK(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::mash_3_stage, busy_override::yes));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_3_stage);
  CHECK(clk_regs.gp0_clk.control==0x5a000680U);//0x5a000000=write password; 0x680=MASH|BUSY
  CHECK(clk_regs.set_mash(gp0_clk_id, clock_mash_mode::mash_2_stage, busy_override::yes));
  CHECK(clk_regs.get_mash(gp0_clk_id)==clock_mash_mode::mash_2_stage);
  CHECK(clk_regs.gp0_clk.control==0x5a000480U);//0x5a000000=write password; 0x480==MASH|BUSY
}

TEST_CASE( "Unit-tests/clock_registers/0430/set_source for busy clock"
         , "Check set_source field behaviour for busy clock is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp0_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_source(gp0_clk_id, clock_src::hdmi_aux));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::gnd); // gnd = 0
  CHECK(clk_regs.gp0_clk.control==128U);// BUSY and not forced: no write occured
  CHECK_FALSE(clk_regs.set_source(gp0_clk_id,clock_src::plld));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::gnd);
  CHECK(clk_regs.gp0_clk.control==128U);

  CHECK(clk_regs.set_source(gp0_clk_id, clock_src::testdebug1, busy_override::yes));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::testdebug1);
  CHECK(clk_regs.gp0_clk.control==0x5a000083U);
  CHECK(clk_regs.set_source(gp0_clk_id, clock_src::gnd, busy_override::yes));
  CHECK(clk_regs.get_source(gp0_clk_id)==clock_src::gnd);
  CHECK(clk_regs.gp0_clk.control==0x5a000080U);
}

TEST_CASE( "Unit-tests/clock_registers/0440/set_divi for busy clocks"
         , "Check set_divi behaviour for busy clocks is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp1_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_divi(gp1_clk_id, 1));
  CHECK(clk_regs.get_divi(gp1_clk_id)==0);
  CHECK(clk_regs.gp1_clk.divisor==0);
  CHECK_FALSE(clk_regs.set_divi(gp1_clk_id, 0xfffU));
  CHECK(clk_regs.get_divi(gp1_clk_id)==0);
  CHECK(clk_regs.gp1_clk.divisor==0);

  CHECK(clk_regs.set_divi(gp1_clk_id, 1, busy_override::yes));
  CHECK(clk_regs.get_divi(gp1_clk_id)==1);
  CHECK(clk_regs.gp1_clk.divisor==0x5a001000U);//0x5a000000=write pswd; 0x001=DIVI (12) bits
  CHECK(clk_regs.set_divi(gp1_clk_id, 0xfffU, busy_override::yes));
  CHECK(clk_regs.get_divi(gp1_clk_id)==0xfffU);
  CHECK(clk_regs.gp1_clk.divisor==0x5afff000U);
}

TEST_CASE( "Unit-tests/clock_registers/0450/set_divf for busy clocks"
         , "Check set_divf behaviour for busy clocks is as expected"
         )
{
  clock_registers clk_regs;
// initially start with all bytes of clk_regs set to 0x00:
  std::memset(&clk_regs, 0x00, sizeof(clk_regs));
  clk_regs.gp1_clk.control = 128U; // BUSY flag is bit 7
  CHECK_FALSE(clk_regs.set_divf(gp1_clk_id, 1));
  CHECK(clk_regs.get_divf(gp1_clk_id)==0);
  CHECK(clk_regs.gp1_clk.divisor==0);
  CHECK_FALSE(clk_regs.set_divf(gp1_clk_id, 0xfffU));
  CHECK(clk_regs.get_divf(gp1_clk_id)==0);
  CHECK(clk_regs.gp1_clk.divisor==0);

  CHECK(clk_regs.set_divf(gp1_clk_id, 1, busy_override::yes));
  CHECK(clk_regs.get_divf(gp1_clk_id)==1);
  CHECK(clk_regs.gp1_clk.divisor==0x5a000001U);//0x5a000000=write pswd; 0x001=DIVF (12) bits
  CHECK(clk_regs.set_divf(gp1_clk_id, 0xfffU, busy_override::yes));
  CHECK(clk_regs.get_divf(gp1_clk_id)==0xfffU);
  CHECK(clk_regs.gp1_clk.divisor==0x5a000fffU);
}
