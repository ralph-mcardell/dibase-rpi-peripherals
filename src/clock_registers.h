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

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Strongly typed enumeration of clock busy override values
  /// Many operations are best not performed if a clock is busy and 
  /// interface functions usually do not allow such operations to proceed
  /// if a clock is busy by default but allow forcing the issue if the
  /// caller specifies busy_override::yes.
    enum class busy_override
    { no      ///< Do not override do-not-do-if-clock-busy advise
    , yes     ///< Override do-not-do-if-clock-busy advise
    };

  /// @brief Strongly typed enumeration of clock MASH control mode values.
  ///
  /// Each clock uses one of several MASH noise shaping behaviour modes.
  /// Refer to 
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
  /// I/O (GPIO), section 6.3 "General Purpose GPIO Clocks" for more
  /// information.
    enum class clock_mash_mode : register_t
    { integer_division = 0
    , mash_1_stage = (1<<9)
    , mash_2_stage = (2<<9)
    , mash_3_stage = (3<<9)
    };

  /// @brief Strongly typed enumeration of clock SRC control mode values.
  ///
  /// Each clock takes its 'master' clock from one of several sources as
  /// specified by the clock's control SRC field. See 
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
  /// I/O (GPIO), section 6.3 "General Purpose GPIO Clocks" for more
  /// information.
    enum class clock_src : register_t
    { gnd = 0
    , oscillator = 1
    , testdebug0 = 2
    , testdebug1 = 3
    , plla = 4
    , pllc = 5
    , plld = 6
    , hdmi_aux = 7
    /* field values 8..15 are also mapped to GND so are not used here */
    };

  /// @brief Clock manager control registers record for a single clock
  ///
  /// Each clock has two control registers: 
  ///   - a main control register, XX_CTL
  ///   - a frequency divisor reqgister, XX_DIV
  /// These are grouped together in this structure as the members control and
  /// divisor respectfully.
  ///
  /// Member function operations are provided to query and set the various
  /// fields and flags of a clock's control and divisor registers. These are
  /// all marked volatile as a matter of course as real-use instances will
  /// map onto IO peripheral device control registers.
    struct clock_record
    {
    private:
      enum
      { ctrl_enab_mask = (1U<<4)
      , ctrl_kill_mask = (1U<<5)
      , ctrl_busy_mask = (1U<<7)
      , ctrl_flip_mask = (1U<<8)
      , ctrl_mash_mask = (3U<<9)
      , ctrl_src_mask  = 15U
      , ctrl_src_max_used = 7U
      , div_divi_min   = 1U // Note: varies with ctrl MASH mode
      , div_divi_max   = 0xfffU
      , div_divi_shift = 12U
      , div_divi_mask  = (div_divi_max<<div_divi_shift)
      , div_divf_min   = 0U
      , div_divf_max   = 0xfffU
      , div_divf_mask  = div_divf_max

      /// @brief Magic value: see BCM2835 peripherals manual tables 6-34 & 35.
      , password = 0x5a000000U
      };

    public:
      register_t control; ///< Clock control (XX_CTL) register
      register_t divisor; ///< Clock fequency divisor (XX_DIV) register

    /// @brief Return status of control register BUSY flag.
    /// @returns true if control register BUSY bit set, false if not.
      bool is_busy() volatile const { return control&ctrl_busy_mask; }

    /// @brief Returns value of ENAB control register bit
    /// @returns true if ENAB control bit set or false if it is not.
      bool get_enable() volatile const { return control&ctrl_enab_mask; }

    /// @brief Returns value of KILL control register bit
    /// @returns true if KILL control bit set or false if it is not.
      bool get_kill() volatile const { return control&ctrl_kill_mask; }

    /// @brief Returns value of FLIP control register bit
    /// @returns true if FLIP control bit set or false if it is not.
      bool get_flip() volatile const { return control&ctrl_flip_mask; }

    /// @brief Returns value of MASH control register field
    /// @returns Clock control register MASH field value.
      clock_mash_mode get_mash() volatile const 
      { 
        return static_cast<clock_mash_mode>(control&ctrl_mash_mask);
      }

    /// @brief Returns value of SRC control register field.
    /// Note: Raw field values of 8...15 are mapped to clock_src::gnd
    /// (value 0).
    /// @returns Clock control register SRC field value.
      clock_src get_source() volatile const
      { 
        register_t raw_src{control&ctrl_src_mask};
        return raw_src>ctrl_src_max_used? clock_src::gnd
                                        : static_cast<clock_src>(raw_src);
      }

    /// @brief Returns value of DIVI divisor register field.
    /// @returns Value of clock divisor DIVI field register in range [1..0xFFF]
      register_t get_divi() volatile const
      {
        return (divisor&div_divi_mask)>>div_divi_shift;
      }

    /// @brief Returns value of DIVF divisor register field.
    /// @returns Value of clock divisor DIVF field register in range [0..0xFFF]
      register_t get_divf() volatile const
      {
        return (divisor&div_divf_mask);
      }

    /// @brief Set the value of ENAB control register bit
    /// Will not perform the operation if clock is busy and force is not
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
    /// Will not perform the operation if clock is busy and force is not
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

    /// @brief Set the value of MASH control register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param mode  Value to set MASH field to
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_mash(clock_mash_mode mode, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }

      // Write to control with password all in one go
        control = ((control|password)               // password bits on
                    & ~ctrl_mash_mask)              // current MASH bits off
                  | static_cast<register_t>(mode);  // new MASH bits on
        return true;
      }

    /// @brief Set the value of SRC control register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param src  Value to set SRC field to
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_source(clock_src src, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }

      // Write to control with password all in one go
        control = ((control|password)             // password bits on
                    & ~ctrl_src_mask)             // current SRC bits off
                  | static_cast<register_t>(src); // new SRC bits on
        return true;
      }

    /// @brief Set the value of DIVI divisor register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// Does not check control MASH setting to adjust current minimum value.
    /// The minimums are: 1 for MASH integer divide mode, 2 for MASH stage 1,
    /// 3 for MASH stage 2 and 5 for MASH stage 3.
    /// @param divi  Value to set divi field to in range [1,0xfff].
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy or divi value is out of range.
      bool set_divi(register_t divi, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }
        if (divi<div_divi_min || divi>div_divi_max)
          {
            return false;
          }
      // Write to control with password all in one go
        divisor = ((divisor|password)         // password bits on
                    & ~div_divi_mask)         // current DIVI bits off
                  | (divi<<div_divi_shift);   // new DIVI bits on
        return true;
      }

    /// @brief Set the value of DIVF divisor register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param divf  Value to set divf field to in range [0,0xfff].
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy or divf value is out of range.
      bool set_divf(register_t divf, busy_override force=busy_override::no)
      volatile
      { 
        if (force==busy_override::no && is_busy())
          {
            return false;
          }
        if (divf>div_divf_max) // || divf<div_divf_min but div_divf_min==0U
          {
            return false;
          }
      // Write to control with password all in one go
        divisor = ((divisor|password)   // password bits on
                    & ~div_divf_mask)   // current DIVF bits off
                  | divf;               // new DIVF bits on
        return true;
      }
    };

    struct clock_registers;
    typedef clock_record clock_registers::*   clock_id;

  /// @brief Represents layout of clock control registers with operations.
  ///
  /// Permits access to BCM2835 (GPIO) clock mananger control registers 
  /// when an instance is mapped to the correct physical memory location.
  ///
  /// See the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 6 for published
  /// details. PWN clock control and divisor register offsets gleaned from
  /// Gertboard gb_pwm.h source code.
  ///
  /// Member function operations are provided to query and set the various
  /// fields and flags of a specified clock's control and divisor registers.
  /// These are all marked volatile as a matter of course as real-use instance
  /// will map onto IO peripheral device control registers.
    struct clock_registers
    {
    private:
      enum
      { gp_offset = 28///< General purpose control & divisor registers' offset
      , pwm_offset=40 ///< PWM control & divisor registers' offset
      , regs_per_clk=2///< Number of 32-bit (4-byte) registers for each clock
      , num_gp_clks=3 ///< Number of general purpose clocks
      
    /// @brief 32-bit register gap between GP clocks end & PWM clocks start
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
      bool is_busy(clock_id clk) volatile const
      {
        return (this->*clk).is_busy();
      }

    /// @brief Return value of control register ENAB bit for specified clock.
    /// @param clk  Clock id of clock to return enable bit value of
    /// @returns true if control register ENAB bit set, false if not.
      bool get_enable(clock_id clk) volatile const
      {
        return (this->*clk).get_enable();
      }

    /// @brief Return value of control register KILL bit for specified clock.
    /// @param clk  Clock id of clock to return kill bit value of
    /// @returns true if control register KILL bit set, false if not.
      bool get_kill(clock_id clk) volatile const
      {
        return (this->*clk).get_kill();
      }

    /// @brief Return value of control register FLIP bit for specified clock.
    /// @param clk  Clock id of clock to return flip bit value of
    /// @returns true if control register FLIP bit set, false if not.
      bool get_flip(clock_id clk) volatile const
      {
        return (this->*clk).get_flip();
      }

    /// @brief Returns value of clock MASH control register field
    /// @param clk  Clock id of clock to return MASH mode value of
    /// @returns Value of clock control register MASH field.
      clock_mash_mode get_mash(clock_id clk) volatile const 
      { 
        return (this->*clk).get_mash();
      }

    /// @brief Returns value of clock SRC control register field.
    /// Note: Raw field values of 8...15 are mapped to clock_src::gnd
    /// (value 0).
    /// @param clk  Clock id of clock to return SRC field value of
    /// @returns Value of clock control register SRC field.
      clock_src get_source(clock_id clk) volatile const 
      { 
        return (this->*clk).get_source();
      }

    /// @brief Returns value of clock DIVI divisor register field.
    /// @param clk  Clock id of clock to return DIVI field value of
    /// @returns Value of clock divisor register DIVI field in range [1..0xFFF]
      register_t get_divi(clock_id clk) volatile const
      {
        return (this->*clk).get_divi();
      }

    /// @brief Returns value of clock DIVF divisor register field.
    /// @param clk  Clock id of clock to return DIVF field value of
    /// @returns Value of clock divisor register DIVF field in range [0..0xFFF]
      register_t get_divf(clock_id clk) volatile const
      {
        return (this->*clk).get_divf();
      }

    /// @brief Set the value of clock ENAB control register bit
    /// Will not perform the operation if clock is busy and force is not
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

    /// @brief Set the value of clock KILL control register bit
    /// @param clk  Clock id of clock to modify kill bit value of
    /// @param state State to set KILL bit to: true->1, false->0
      void set_kill(clock_id clk, bool state) volatile 
      {
        (this->*clk).set_kill(state);
      }

    /// @brief Set the value of clock FLIP control register bit
    /// Will not perform the operation if clock is busy and force is not
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

    /// @brief Set the value of clock MASH control register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param clk   Clock id of clock to modify MASH field value of
    /// @param mode  Value to set clock MASH field to
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_mash
      ( clock_id clk
      , clock_mash_mode mode
      , busy_override force=busy_override::no
      ) volatile
      { 
        return (this->*clk).set_mash(mode, force);
      }

    /// @brief Set the value of clock SRC control register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param clk   Clock id of clock to modify SRC field value of
    /// @param src  Value to set clock SRC field to
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy.
      bool set_source
      ( clock_id clk
      , clock_src src
      , busy_override force=busy_override::no
      ) volatile
      { 
        return (this->*clk).set_source(src, force);
      }

    /// @brief Set the value of clock DIVI divisor register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// Does not check control MASH setting to adjust current minimum value.
    /// The minimums are: 1 for MASH integer divide mode, 2 for MASH stage 1,
    /// 3 for MASH stage 2 and 5 for MASH stage 3.
    /// @param clk   Clock id of clock to modify DIVI field value of
    /// @param divi  Value to set divi field to in range [1,0xfff].
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy or divi value is out of range.
      bool set_divi
      ( clock_id clk
      , register_t divi
      , busy_override force=busy_override::no
      ) volatile
      { 
        return (this->*clk).set_divi(divi, force);
      }

    /// @brief Set the value of clock DIVF divisor register field
    /// Will not perform the operation if clock is busy and force is not
    /// busy_override::yes.
    /// @param clk   Clock id of clock to modify DIVF field value of
    /// @param divf  Value to set divf field to in range [0,0xfff].
    /// @param force Specifies whether to allow operation if clock is busy
    /// @returns  true if operation performed, false if it was not performed
    ///           because clock was busy or divf value is out of range.
      bool set_divf
      ( clock_id clk
      , register_t divf
      , busy_override force=busy_override::no
      ) volatile
      { 
        return (this->*clk).set_divf(divf, force);
      }
    };

    clock_id const gp0_clk_id{&clock_registers::gp0_clk};
    clock_id const gp1_clk_id{&clock_registers::gp1_clk};
    clock_id const gp2_clk_id{&clock_registers::gp2_clk};
    clock_id const pwm_clk_id{&clock_registers::pwm_clk};
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_REGISTERS_H
