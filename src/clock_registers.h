// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_registers.h 
/// @brief Low-level (GPIO) clock manager control registers type definition.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
/// I/O (GPIO) for details along with additional information on clocks for
/// PWM found in wiringpi and Gertboard source code.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H
 #define DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H

 #include "peridef.h"
 #include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Clock manager control registers record for a single clock
  ///
  /// Each clock has two control registers: 
  ///   - a main control register, XX_CTL
  ///   - a frequency divisor reqgister, XX_DIV
  /// These are grouped together in this structure as the members control and
  /// divisor respectfully.
    struct clock_record
    {
      register_t control; ///< Clock control (XX_CTL) register
      register_t divisor; ///< Clock fequency divisor (XX_DIV) register
    };

    struct clock_registers;
    typedef clock_record clock_registers::*   clock_id;

  /// @brief Represents layout of clock control registers with operations.
  ///
  /// Permits access to BCM2835 (GPIO) clock mamanger control registers 
  /// when an instance is mapped to the correct physical memory location.
  ///
  /// See the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 6 for published
  /// details. PWN clock control and divisor register offsets gleaned from
  /// Gertboard gb_pwm.h source code.
  ///
    struct clock_registers
    {
    private:
      enum
      { gp_offset = 28///< General purpose control & divisor registers' offset
      , pwm_offset=40 ///< PWM control & divisor registers' offset
      , regs_per_clk=2///< Number of 32-bit (4-byte) registers for each clock
      , num_gp_clks=3 ///< Number of general purpose clocks
      
    /// @brief 32-bit register gap between GP clocks end & PWN clocks start
      , gp_pwm_gap=pwm_offset-gp_offset-(num_gp_clks*regs_per_clk)

      /// @brief Magic value: see BCM2835 peripherals manual tables 6-34 & 35.
      , password = 0x5A000000
      };
    public:
    /// @brief Physical address of start of BCM2835 clock control registers
      static const physical_address_t physical_address
                                          = peripheral_base_address + 0x101000;

      register_t reserved_do_not_use_0[gp_offset];///< Reserved, currently unused
      clock_record gp0_clk; ///< General purpose clock 0
      clock_record gp1_clk; ///< General purpose clock 1
      clock_record gp2_clk; ///< General purpose clock 2

      register_t reserved_do_not_use_1[gp_pwm_gap];///< Reserved, currently unused
      clock_record pwm_clk; ///< PWM clock
    };

    clock_id const gp0_clk_id{&clock_registers::gp0_clk};
    clock_id const gp1_clk_id{&clock_registers::gp1_clk};
    clock_id const gp2_clk_id{&clock_registers::gp2_clk};
    clock_id const pwm_clk_id{&clock_registers::pwm_clk};
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H
