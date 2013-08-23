// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_ctrl.h 
/// @brief InternalPWM control type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_CTRL_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_CTRL_H

# include "phymem_ptr.h"
# include "pwm_registers.h"
# include "simple_allocator.h"
# include "clockdefs.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      constexpr std::size_t number_of_pwm_channels{2};

    /// @brief GPIO PWM control type. There is only 1 (yes it's a singleton!)
    /// Maps BCM2708/2835 PWM registers into the requisite physical
    /// memory mapped area, provides simple allocator for in-process clock
    /// resource use tracking.
      struct pwm_ctrl
      {
      /// @brief Pointer to BCM2708 / BCM2835 PWM control registers instance
        phymem_ptr<volatile pwm_registers>        regs;

      /// @brief PWM channel allocator instance
        simple_allocator<number_of_pwm_channels>  alloc;

      /// @brief Singleton instance getter
      /// @returns THE instance of the PWM control object.
        static pwm_ctrl & instance();

      /// @brief set source and frequency of the common PWM clock
      /// Note that the clock is common to all (both) PWM channels. The
      /// clock should only be set when no PWM channels are in use.
      /// @param src_freq   Clock source frequency in Hertz
      /// @param src_type   Clock source type
      /// @param freq       Clock requested frequency characteristics
      /// @throws std::invalid_argument the clock frequency is out of range
      ///         for the filtering mode requested or the source and requested
      ///         frequencies are not in the valid range of ratios:
      ///         1<=(requested frequency)/(source frequency)<=0xfff
      /// @throws std::range_error if the clock divisor DIVI field value is
      ///         too small for the selected MASH mode.
      /// @throws peripheral_in_use if any PWM channel is in use (as reported
      ///         by the alloc allocator member) at the time of the call.
        void set_clock
        ( hertz src_freq
        , clock_source src_type
        , clock_frequency const & freq
        );


      private:
      /// @brief Construct: initialise regs with correct physical address & size 
        pwm_ctrl();

        pwm_ctrl(pwm_ctrl const &) = delete;
        pwm_ctrl(pwm_ctrl &&) = delete;
        pwm_ctrl & operator=(pwm_ctrl const &) = delete;
        pwm_ctrl & operator=(pwm_ctrl &&) = delete;
      };
   } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_CTRL_H
