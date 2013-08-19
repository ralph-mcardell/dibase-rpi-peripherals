// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_registers.h 
/// @brief Low-level (GPIO) PWM control registers type definition.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 9 Pulse Width
/// Modifier for details along with additional information on PWM found in 
/// wiringpi and Gertboard source code..
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PWM_REGISTERS_H
# define DIBASE_RPI_PERIPHERALS_PWM_REGISTERS_H

# include "peridef.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Strong enumeration type for PWM channels
  /// There are two channels known, confusingly, either as GPIO special
  /// functions PWM0, PWM1 or PWM channel 1, PWM channel 2 in the PWM
  /// controller registers descriptions.
    enum class pwm_channel : register_t
    { gpio_pwm0 = 0   ///< GPIO function nomenclature for first PWM channel
    , gpio_pwm1 = 1   ///< GPIO function nomenclature for second PWM channel
    , pwm_ch1 = 0     ///< PWM channel nomenclature for first PWM channel
    , pwm_ch2 = 1     ///< PWM channel nomenclature for second PWM channel
    };
    
  /// @brief Represents layout of PWM control registers with operations.
  ///
  /// Permits access to BCM2835 PWM controller registers when an 
  /// instance is mapped to the correct physical memory location.
  ///
  /// See the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 9 for published
  /// details. PWN clock control and divisor register offsets gleaned from
  /// Gertboard gb_pwm.h source code.
  ///
  /// Note that there is some confusion with regard to PWM channel numbering.
  /// While the GPIO pins special functions refer to PWM channels 0 and 1 (PWM0,
  /// PWM1), the PWM control registers use PWM channels 1 and 2 - go figure. 
  /// The obvious assumption is that PWM0 maps onto PWM channel 1 and PWM1 to
  /// channel 2, and is backed up by values used by the Gertboard software PWM 
  /// source code (pwm.h, pwm.c).
  ///
  /// Member function operations are provided to query and set the various
  /// fields and flags for PWM channel control.
    struct pwm_registers
    {
    private:
      enum
      { ctl_enable  = 0x1   ///< Control register: PWM channel enable
      , ctl_mode_ser= 0x2   ///< Control register: serialiser mode
      , ctl_rptl    = 0x4   ///< Control register: repeat last data
      , ctl_sbit    = 0x8   ///< Control register: silence state 1s
      , ctl_pola    = 0x10  ///< Control register: reverse polarity
      , ctl_usef    = 0x20  ///< Control register: Use FIFO
      , ctl_clrf    = 0x40  ///< Control register: Clear FIFO
      , ctl_msen    = 0x80  ///< Control register: Use M/S algorithm
      , ctl_ch2_shift = 8   ///< Shift for control bits for PWM1 / channel 2
      };

    public:
    /// @brief Physical address of start of BCM2835 PWM control registers
      constexpr static physical_address_t 
                            physical_address = peripheral_base_address+0x20C000;
      register_t control;     ///< PWM control register, CTL
      register_t status;      ///< PWM status register, STA
      register_t dma_config;  ///< PWM DMA configuration register, DMAC
      register_t reserved_do_not_use_0; ///< Reserved, currently unused
      register_t range1;      ///< PWM0 (channel 1) range register, RNG1
      register_t data1;       ///< PWM0 (channel 1) data register, DAT1
      register_t fifo_in;     ///< PWM FIFO input register FIF1
      register_t reserved_do_not_use_1; ///< Reserved, currently unused
      register_t range2;      ///< PWM1 (channel 2) range register, RNG2
      register_t data2;       ///< PWM1 (channel 2) data register, DAT2
    };
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_PWM_REGISTERS_H
