// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_ctrl.h 
/// @brief Internal GPIO clock control type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H
 #define DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H

#include "phymem_ptr.h"
#include "clock_registers.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      class clock_parameters;
      constexpr unsigned gpclk0{0U}; ///< GPCLK0 internal index value
      constexpr unsigned gpclk1{1U}; ///< GPCLK1 internal index value
      constexpr unsigned gpclk2{2U}; ///< GPCLK2 internal index value
      constexpr unsigned pwmclk{3U}; ///< PWMCLK internal index value

    /// @broef Convert an internal clock index value to a clock_id enum value
    /// @param i  Internal clock index value: gpclk0, gpclk1, gpclk2 or pwmclk
    /// @returns clock_id enumeration value used with clock register operations
      clock_id index_to_clock_id(unsigned i);

    /// @brief GPIO clock control type. There is only 1 (yes it's a singleton!)
    /// Maps BCM2708/2835 GPIO clock registers into the requisite physical
    /// memory mapped area
      struct clock_ctrl
      {
      /// @brief Pointer to BCM2708 / BCM2835 clock control registers instance
        phymem_ptr<volatile clock_registers>   regs;

      /// @brief Singleton instance getter
      /// @returns THE instance of the GPIO clock control object.
        static clock_ctrl & instance();

      /// @brief Initialise a clock with passed parameters
      /// @param clk  id of clock to initialise
      /// @param cp   Parameters to use for clock.
        static void initialise_clock(clock_id clk, clock_parameters const & cp);

      private:
      /// @brief Construct: initialise regs with correct physical address & size 
        clock_ctrl();

        clock_ctrl(clock_ctrl const &) = delete;
        clock_ctrl(clock_ctrl &&) = delete;
        clock_ctrl & operator=(clock_ctrl const &) = delete;
        clock_ctrl & operator=(clock_ctrl &&) = delete;
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H
