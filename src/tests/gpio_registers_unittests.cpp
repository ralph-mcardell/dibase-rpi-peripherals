// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_registers_unittests.cpp 
/// @brief Unit tests for low-level GPIO control registers type.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 6 General Purpose I/O (GPIO) 
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "gpio_registers.h"
#include <cstring>
#include <cstdint>

using namespace dibase::rpi::peripherals;

typedef uint32_t RegisterType;
typedef unsigned char Byte;
RegisterType const MinPinId(0);
RegisterType const MaxPinId(53);
RegisterType const NumPinIds(MaxPinId-MinPinId+1);

TEST_CASE("Unit-tests/gpio_registers/register-offsets", "Register member offsets should match the documented layout")
{
  enum RegisterOffsets
    { GPFSEL0=0x00, GPFSEL1=0x04, GPFSEL2  =0x08, GPFSEL3  =0x0C, GPFSEL4=0x10
    , GPFSEL5=0x14, GPSET0 =0x1C, GPSET1   =0x20, GPCLR0   =0x28, GPCLR1 =0x2C
    , GPLEV0 =0x34, GPLEV1 =0x38, GPEDS0   =0x40, GPEDS1   =0x44, GPREN0 =0x4C
    , GPREN1 =0x50, GPFEN0 =0x58, GPFEN1   =0x5C, GPHEN0   =0x64, GPHEN1 =0x68
    , GPLEN0 =0x70, GPLEN1 =0x74, GPAREN0  =0x7C, GPAREN1  =0x80, GPAFEN0=0x88
    , GPAFEN1=0x8C, GPPUD  =0x94, GPPUDCLK0=0x98, GPPUDCLK1=0x9C, TEST   =0xB0
    };
  gpio_registers gpio_regs;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&gpio_regs));
  
  // Set each 32-bit register to the value of its offset:
  gpio_regs.gpfsel[0] = GPFSEL0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL0])==GPFSEL0 );
  gpio_regs.gpfsel[1] = GPFSEL1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL1])==GPFSEL1 );
  gpio_regs.gpfsel[2] = GPFSEL2;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL2])==GPFSEL2 );
  gpio_regs.gpfsel[3] = GPFSEL3;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL3])==GPFSEL3 );
  gpio_regs.gpfsel[4] = GPFSEL4;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL4])==GPFSEL4 );
  gpio_regs.gpfsel[5] = GPFSEL5;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFSEL5])==GPFSEL5 );

  gpio_regs.gpset[0] = GPSET0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPSET0])==GPSET0 );
  gpio_regs.gpset[1] = GPSET1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPSET1])==GPSET1 );

  gpio_regs.gpclr[0] = GPCLR0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPCLR0])==GPCLR0 );
  gpio_regs.gpclr[1] = GPCLR1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPCLR1])==GPCLR1 );

  gpio_regs.gplev[0] = GPLEV0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPLEV0])==GPLEV0 );
  gpio_regs.gplev[1] = GPLEV1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPLEV1])==GPLEV1 );

  gpio_regs.gpeds[0] = GPEDS0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPEDS0])==GPEDS0 );
  gpio_regs.gpeds[1] = GPEDS1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPEDS1])==GPEDS1 );

  gpio_regs.gpren[0] = GPREN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPREN0])==GPREN0 );
  gpio_regs.gpren[1] = GPREN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPREN1])==GPREN1 );

  gpio_regs.gpfen[0] = GPFEN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFEN0])==GPFEN0 );
  gpio_regs.gpfen[1] = GPFEN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPFEN1])==GPFEN1 );

  gpio_regs.gphen[0] = GPHEN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPHEN0])==GPHEN0 );
  gpio_regs.gphen[1] = GPHEN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPHEN1])==GPHEN1 );

  gpio_regs.gplen[0] = GPLEN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPLEN0])==GPLEN0 );
  gpio_regs.gplen[1] = GPLEN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPLEN1])==GPLEN1 );

  gpio_regs.gparen[0] = GPAREN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPAREN0])==GPAREN0 );
  gpio_regs.gparen[1] = GPAREN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPAREN1])==GPAREN1 );

  gpio_regs.gpafen[0] = GPAFEN0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPAFEN0])==GPAFEN0 );
  gpio_regs.gpafen[1] = GPAFEN1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPAFEN1])==GPAFEN1 );

  gpio_regs.gppud = GPPUD;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUD])==GPPUD );

  gpio_regs.gppudclk[0] = GPPUDCLK0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUDCLK0])==GPPUDCLK0 );
  gpio_regs.gppudclk[1] = GPPUDCLK1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUDCLK1])==GPPUDCLK1 );

  gpio_regs.test = TEST;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[TEST])==TEST );
}

TEST_CASE("Unit-tests/gpio_registers/set_pin_fn", "Setting pin function sets associated 3 bits of gpfsel member")
{
// See BCM2835 Peripherals datasheet document, Tables 6-2..6-7
//  RegisterType const PinFnInput(0); // Unused
  RegisterType const PinFnOutput(1);
  RegisterType const PinFnAlt0(4);
  RegisterType const PinFnAlt1(5);
  RegisterType const PinFnAlt2(6);
  RegisterType const PinFnAlt3(7);
  RegisterType const PinFnAlt4(3);
  RegisterType const PinFnAlt5(2);

  RegisterType const BitsPerFn(3);
  RegisterType const BitsPerRegister(32);
  RegisterType const FnsPerReg(BitsPerRegister/BitsPerFn);

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.set_pin_function(pinid, gpio_pin_fn::output);
      RegisterType const FnRegIdx(pinid/FnsPerReg);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnOutput<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt0);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt0<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt1);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt1<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt2);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt2<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt3);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt3<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt4);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt4<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::alt5);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt5<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pinid, gpio_pin_fn::input);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==0);
    }
  CHECK(gpio_regs.gpfsel[0]==0);
  CHECK(gpio_regs.gpfsel[1]==0);
  CHECK(gpio_regs.gpfsel[2]==0);
  CHECK(gpio_regs.gpfsel[3]==0);
  CHECK(gpio_regs.gpfsel[4]==0);
}

TEST_CASE("Unit-tests/gpio_registers/set_pin", "Setting pin sets associated gpset member bit high")
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.set_pin(pinid);
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gpset[PinRegIdx]==(1U<<(pinid%PinsPerRegister)));
    }
}

TEST_CASE("Unit-tests/gpio_registers/clear_pin", "Clearing pin sets associated gpclr member bit high")
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.clear_pin(pinid);
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gpclr[PinRegIdx]==(1U<<(pinid%PinsPerRegister)));
    }
}
