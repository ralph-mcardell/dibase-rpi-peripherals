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
    /// @brief GPIO clock control type. There is only 1 (yes it's a singleton!)
    /// Maps BCM2708/2835 GPIO clock registers into the requesite physical
    /// memory mapped area
      struct clock_ctrl
      {
      /// @brief Pointer to BCM2708 / BCM2835 clock control registers instance
        phymem_ptr<volatile clock_registers>   regs;

      /// @brief Singleton instance getter
      /// @returns THE instance of the GPIO clock control object.
        static clock_ctrl & instance();

      private:
      /// @brief Construct: intialise regs with correct physical address & size 
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
