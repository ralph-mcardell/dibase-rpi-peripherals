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

TEST_CASE( "Unit-tests/one_bit_field_register/offsets"
         , "Registers with one-bit field per pin should have offsets 0 and 4"
         )
{
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&r, 0xFF, sizeof(r));
  Byte * reg_base_addr(reinterpret_cast<Byte *>(&r));

  r[0] = 1234567890U;
  r[1] = 2345678901U;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[0])==1234567890U );
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[4])==2345678901U );
}

TEST_CASE( "Unit-tests/one_bit_field_register/set_bit"
         , "Setting a bit changes the state of just that bit"
         )
{
  RegisterType const BitsPerRegister(32);
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&r, 0, sizeof(r));
  for (RegisterType bitnumber=MinPinId; bitnumber<=MaxPinId; ++bitnumber)
    {
      r.set_bit(bitnumber);
      RegisterType const BitRegIdx(bitnumber/BitsPerRegister);
    // All bits up to and including the just set bit should be set
    // This is 2**(bitposition+1) - 1. i.e. if bitposition were 2 then
    // bits 0,1,2 (==7) would be set which is 2**3 - 1.
      CHECK(r[BitRegIdx]==(1U<<(bitnumber%BitsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/one_bit_field_register/clear_bit"
         , "Clearing a bit changes the state of just that bit"
         )
{
  RegisterType const BitsPerRegister(32);
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&r, 0xFF, sizeof(r));
  for (RegisterType bitnumber=MinPinId; bitnumber<=MaxPinId; ++bitnumber)
    {
      r.clear_bit(bitnumber);
      RegisterType const BitRegIdx(bitnumber/BitsPerRegister);
    // All bits up to and including the just set bit should be cleared
    // This is ~(2**(bitposition+1) - 1). i.e. if bitposition were 2 then
    // bits 0,1,2 would be clear which is ~(2**3 - 1).
      CHECK(r[BitRegIdx]==~((1U<<(bitnumber%BitsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/one_bit_field_register/set_just_bit"
         , "Setting 1 bit sets only 1 bit in only one register word of a pair"
         )
{
  RegisterType const BitsPerRegister(32);
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0:
  std::memset(&r, 0, sizeof(r));
  for (RegisterType bitnumber=MinPinId; bitnumber<=MaxPinId; ++bitnumber)
    {
      r.set_just_bit(bitnumber);
      RegisterType const BitRegIdx(bitnumber/BitsPerRegister);
      CHECK(r[BitRegIdx]==1U<<(bitnumber%BitsPerRegister));
    }
}

TEST_CASE( "Unit-tests/one_bit_field_register/get_bit"
         , "Getting single bit returns 0 or non-zero value with single bit set"
         )
{
  RegisterType const BitsPerRegister(32);
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0:
  std::memset(&r, 0, sizeof(r));
  for (RegisterType bitnumber=MinPinId; bitnumber<=MaxPinId; ++bitnumber)
    {
      bool level_low(r.get_bit(bitnumber));
      CHECK(level_low==false);
      RegisterType const BitRegIdx(bitnumber/BitsPerRegister);
      r[BitRegIdx] |= 1U<<(bitnumber%BitsPerRegister);
      bool level_high(r.get_bit(bitnumber));
      CHECK(level_high==true);
      CHECK(r.get_bit(bitnumber)==1U<<(bitnumber%BitsPerRegister));
    }
}

TEST_CASE( "Unit-tests/one_bit_field_register/clear_all_bits"
         , "Clearing all bits should result in both registers being zero"
         )
{
  one_bit_field_register r;
  // initially start with all bytes of gpio_regs set to 0xFF:
  std::memset(&r, 0xFF, sizeof(r));
  CHECK( r[0] == ~((RegisterType)0) );
  CHECK( r[1] == ~((RegisterType)0) );
  r.clear_all_bits();
  CHECK( r[0] == 0 );
  CHECK( r[1] == 0 );
}

TEST_CASE( "Unit-tests/gpio_registers/register-offsets"
         , "Register member offsets should match the documented layout"
         )
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

  gpio_regs.gppud = static_cast<gpio_pud_mode>(GPPUD);
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUD])==GPPUD );

  gpio_regs.gppudclk[0] = GPPUDCLK0;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUDCLK0])==GPPUDCLK0 );
  gpio_regs.gppudclk[1] = GPPUDCLK1;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[GPPUDCLK1])==GPPUDCLK1 );

  gpio_regs.test = TEST;
  CHECK( reinterpret_cast<RegisterType&>(reg_base_addr[TEST])==TEST );
}

TEST_CASE( "Unit-tests/gpio_registers/set_pin_fn"
         , "Setting pin function sets associated 3 bits of gpfsel member"
         )
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
      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::output);
      RegisterType const FnRegIdx(pinid/FnsPerReg);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnOutput<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt0);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt0<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt1);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt1<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt2);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt2<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt3);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt3<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt4);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt4<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::alt5);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==(PinFnAlt5<<((pinid%FnsPerReg)*BitsPerFn)));

      gpio_regs.set_pin_function(pin_id(pinid), gpio_pin_fn::input);
      CHECK(gpio_regs.gpfsel[FnRegIdx]==0);
    }
  CHECK(gpio_regs.gpfsel[0]==0);
  CHECK(gpio_regs.gpfsel[1]==0);
  CHECK(gpio_regs.gpfsel[2]==0);
  CHECK(gpio_regs.gpfsel[3]==0);
  CHECK(gpio_regs.gpfsel[4]==0);
}

TEST_CASE( "Unit-tests/gpio_registers/set_pin"
         , "Setting pin sets associated gpset member bit high"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.set_pin(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gpset[PinRegIdx]==(1U<<(pinid%PinsPerRegister)));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/clear_pin"
         , "Clearing pin sets associated gpclr member bit high"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.clear_pin(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gpclr[PinRegIdx]==(1U<<(pinid%PinsPerRegister)));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_level"
         , "Requesting a pin's level returns that pin's level"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      bool level_low(gpio_regs.pin_level(pin_id(pinid)));
      CHECK(level_low==false);
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      gpio_regs.gplev[PinRegIdx] |= 1U<<(pinid%PinsPerRegister);
      bool level_high(gpio_regs.pin_level(pin_id(pinid)));
      CHECK(level_high==true);
      CHECK(gpio_regs.pin_level(pin_id(pinid))==1U<<(pinid%PinsPerRegister));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_event"
         , "Requesting a pin's event status returns that pin's event status"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      bool no_event(gpio_regs.pin_event(pin_id(pinid)));
      CHECK(no_event==false);
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      gpio_regs.gpeds[PinRegIdx] |= 1U<<(pinid%PinsPerRegister);
      bool event_waiting(gpio_regs.pin_event(pin_id(pinid)));
      CHECK(event_waiting==true);
      CHECK(gpio_regs.pin_event(pin_id(pinid))==1U<<(pinid%PinsPerRegister));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/clear_pin_event"
         , "Clearing a pin's event sets associated gpeds member bit high"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.clear_pin_event(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gpeds[PinRegIdx]==(1U<<(pinid%PinsPerRegister)));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_rising_edge_detect_enable"
         , "Enabling rising edge detect for pin sets appropriate bit in gpren"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_rising_edge_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpren[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_rising_edge_detect_disable"
         , "Disabling rising edge detect for pin clears appropriate bit in gpren"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_rising_edge_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpren[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_falling_edge_detect_enable"
         , "Enabling falling edge detect for pin sets appropriate bit in gpfen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_falling_edge_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpfen[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_falling_edge_detect_disable"
         , "Disabling falling edge detect for pin clears appropriate bit in gpfen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_falling_edge_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpfen[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_high_detect_enable"
         , "Enabling high detect for pin sets appropriate bit in gphen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_high_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gphen[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_high_detect_disable"
         , "Disabling high detect for pin clears appropriate bit in gphen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_high_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gphen[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_low_detect_enable"
         , "Enabling low detect for pin sets appropriate bit in gplen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_low_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gplen[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_low_detect_disable"
         , "Disabling low detect for pin clears appropriate bit in gplen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_low_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gplen[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_async_rising_edge_detect_enable"
         , "Enabling async rising edge detect for pin sets assoc. bit in gparen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_async_rising_edge_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gparen[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_async_rising_edge_detect_disable"
         , "Disabling async rising edge detect for pin clears assoc. bit in gparen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_async_rising_edge_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gparen[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_async_falling_edge_detect_enable"
         , "Enabling async falling edge detect for pin sets assoc. bit in gpafen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_async_falling_edge_detect_enable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpafen[PinRegIdx]==(1U<<(pinid%PinsPerRegister+1))-1);
    }
}

TEST_CASE( "Unit-tests/gpio_registers/pin_async_falling_edge_detect_disable"
         , "Disabling async falling edge detect for pin clears assoc. bit in gpafen"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.pin_async_falling_edge_detect_disable(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
    // See comments in TEST_CASE "Unit-tests/one_bit_field_register/set_bit"
      CHECK(gpio_regs.gpafen[PinRegIdx]==~((1U<<(pinid%PinsPerRegister+1))-1));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/set_pull_up_down_mode"
         , "Setting pull up/down mode sets the correct value in gppud"
         )
{
  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  gpio_regs.set_pull_up_down_mode( gpio_pud_mode::enable_pull_down_control );
  CHECK(static_cast<RegisterType>(gpio_regs.gppud)==1);
  gpio_regs.set_pull_up_down_mode( gpio_pud_mode::enable_pull_up_control );
  CHECK(static_cast<RegisterType>(gpio_regs.gppud)==2);
  gpio_regs.set_pull_up_down_mode( gpio_pud_mode::off );
  CHECK(static_cast<RegisterType>(gpio_regs.gppud)==0);
}

TEST_CASE( "Unit-tests/gpio_registers/assert_pin_pull_up_down_clock"
         , "Asserting pin's pull up/down clock sets the correct bit in gppudclk"
         )
{
  RegisterType const BitsPerRegister(32);
  RegisterType const PinsPerRegister(BitsPerRegister); // 1 bit per pin

  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0:
  std::memset(&gpio_regs, 0, sizeof(gpio_regs));
  for (RegisterType pinid=MinPinId; pinid<=MaxPinId; ++pinid)
    {
      gpio_regs.assert_pin_pull_up_down_clock(pin_id(pinid));
      RegisterType const PinRegIdx(pinid/PinsPerRegister);
      CHECK(gpio_regs.gppudclk[PinRegIdx]==1U<<(pinid%PinsPerRegister));
    }
}

TEST_CASE( "Unit-tests/gpio_registers/remove_all_pin_pull_up_down_clocks"
         , "Removing all pin's pull up/down clocks clears both gppudclk registers"
         )
{
  gpio_registers gpio_regs;
// start with all bytes of gpio_regs set to 0xFF:
  std::memset(&gpio_regs, 0xFF, sizeof(gpio_regs));
  gpio_regs.remove_all_pin_pull_up_down_clocks();
  CHECK(gpio_regs.gppudclk[0]==0);
  CHECK(gpio_regs.gppudclk[1]==0);
}
