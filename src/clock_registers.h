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
      { pwm_offset = 10///< PWM control & divisor registers' offset
      , gp_offset =  28///< General purpose control & divisor registers' offset
      , password = 0x5A000000 ///< Magic value: see BCM2835 peripherals manual tables 6-34 & 35.
      };
    public:
    /// @brief Physical address of start of BCM2835 clock control registers
      static const physical_address_t physical_address
                                          = peripheral_base_address + 0x101000;

       register_t reserved_do_not_use_0[pwm_offset];///< Reserved, currently unused
       register_t pwm_ctrl; ///< PWM clock control register
       register_t pwm_div;  ///< PWM clock divisor register

       register_t reserved_do_not_use_1[gp_offset-pwm_offset-2];///< Reserved, currently unused
       register_t gp0_ctrl; ///< General purpose clock 0 control register
       register_t gp0_div;  ///< General purpose clock 0 divisor register
       register_t gp1_ctrl; ///< General purpose clock 1 control register
       register_t gp1_div;  ///< General purpose clock 1 divisor register
       register_t gp2_ctrl; ///< General purpose clock 2 control register
       register_t gp2_div;  ///< General purpose clock 2 divisor register
    };
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H
