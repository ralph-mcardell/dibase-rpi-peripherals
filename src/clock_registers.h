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
  /// @brief Override clock busy enumerations
  /// Many operations are best not performed if a clock is busy and 
  /// interface functions usually do not allow such operations to proceed
  /// is a clock is busy by default but allow forcing the issue if the
  /// caller specifies busy_override::yes.
    enum class busy_override
    { no      ///< Do not override do-not-do-if-clock-busy advise
    , yes     ///< Override do-not-do-if-clock-busy advise
    };

  /// @brief Clock manager control registers record for a single clock
  ///
  /// Each clock has two control registers: 
  ///   - a main control register, XX_CTL
  ///   - a frequency divisor reqgister, XX_DIV
  /// These are grouped together in this structure as the members control and
  /// divisor respectfully.
    struct clock_record
    {
    private:
      enum
      { ctrl_enab_mask = (1U<<4)
      , ctrl_kill_mask = (1U<<5)
      , ctrl_busy_mask = (1U<<7)
      , ctrl_flip_mask = (1U<<8)

      /// @brief Magic value: see BCM2835 peripherals manual tables 6-34 & 35.
      , password = 0x5A000000U
      };

    public:
      register_t control; ///< Clock control (XX_CTL) register
      register_t divisor; ///< Clock fequency divisor (XX_DIV) register

    /// @brief Return ststus of control register BUSY flag.
    /// @returns true if control register BUSY bit set, false if not.
      bool is_busy() volatile { return control&ctrl_busy_mask; }

    /// @brief Returns value of ENAB control register bit
    /// @returns true if ENAB control bit set or false if it is not.
      bool get_enable() volatile { return control&ctrl_enab_mask; }

    /// @brief Returns value of KILL control register bit
    /// @returns true if KILL control bit set or false if it is not.
      bool get_kill() volatile { return control&ctrl_kill_mask; }

    /// @brief Returns value of FLIP control register bit
    /// @returns true if FLIP control bit set or false if it is not.
      bool get_flip() volatile { return control&ctrl_flip_mask; }

    /// @brief Set the value of ENAB control register bit
    /// Will not perform operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param state State to set ENAB bit to: true->1, false->0
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_enable(bool state, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }
        control = password|(state ? control|ctrl_enab_mask
                                  : control&(~ctrl_enab_mask));
        return true;
      }

    /// @brief Set the value of KILL control register bit
    /// @param state State to set KILL bit to: true->1, false->0
      void set_kill(bool state) volatile
      { 
        control = password|(state ? control|ctrl_kill_mask
                                  : control&(~ctrl_kill_mask));
      }

    /// @brief Set the value of FLIP control register bit
    /// Will not perform operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param state State to set FLIP bit to: true->1, false->0
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_flip(bool state, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }
        control = password|(state ? control|ctrl_flip_mask
                                  : control&(~ctrl_flip_mask));
        return true;
      }
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

    /// @brief Return status of control register BUSY flag for specified clock.
    /// @param clk  Clock id of clock to return busy status of
    /// @returns true if control register BUSY bit set, false if not.
      bool is_busy(clock_id clk) volatile { return (this->*clk).is_busy(); }

    /// @brief Return value of control register ENAB bit for specified clock.
    /// @param clk  Clock id of clock to return enable bit value of
    /// @returns true if control register ENAB bit set, false if not.
      bool get_enable(clock_id clk) volatile {return (this->*clk).get_enable();}

    /// @brief Return value of control register KILL bit for specified clock.
    /// @param clk  Clock id of clock to return kill bit value of
    /// @returns true if control register KILL bit set, false if not.
      bool get_kill(clock_id clk) volatile { return (this->*clk).get_kill(); }

    /// @brief Return value of control register FLIP bit for specified clock.
    /// @param clk  Clock id of clock to return flip bit value of
    /// @returns true if control register FLIP bit set, false if not.
      bool get_flip(clock_id clk) volatile { return (this->*clk).get_flip(); }

    /// @brief Set the value of ENAB control register bit
    /// Will not perform operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param clk  Clock id of clock to modify enable bit value of
    /// @param state State to set ENAB bit to: true->1, false->0
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_enable
      ( clock_id clk
      , bool state
      , busy_override force=busy_override::no
      ) volatile 
      {
        return (this->*clk).set_enable(state, force);
      }

    /// @brief Set the value of KILL control register bit
    /// @param clk  Clock id of clock to modify kill bit value of
    /// @param state State to set KILL bit to: true->1, false->0
      void set_kill(clock_id clk, bool state) volatile 
      {
        (this->*clk).set_kill(state);
      }

    /// @brief Set the value of FLIP control register bit
    /// Will not perform operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param clk  Clock id of clock to modify flip bit value of
    /// @param state State to set FLIP bit to: true->1, false->0
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_flip
      ( clock_id clk
      , bool state
      , busy_override force=busy_override::no
      ) volatile 
      {
        return (this->*clk).set_flip(state, force);
      }
    };

    clock_id const gp0_clk_id{&clock_registers::gp0_clk};
    clock_id const gp1_clk_id{&clock_registers::gp1_clk};
    clock_id const gp2_clk_id{&clock_registers::gp2_clk};
    clock_id const pwm_clk_id{&clock_registers::pwm_clk};
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H
