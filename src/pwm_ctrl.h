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
      class clock_parameters;

    /// @brief Number of channels supported by the BCM2835 PWM peripheral
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
      /// @param cp   Clock set-up an frequency parameters
      /// @throws peripheral_in_use if any PWM channel is in use (as reported
      ///         by the alloc allocator member) at the time of the call.
        void set_clock(clock_parameters const & cp);

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
