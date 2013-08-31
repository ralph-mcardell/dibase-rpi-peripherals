// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_ctrl.h 
/// @brief \b Internal : GPIO control type definition
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_GPIO_CTRL_H
 #define DIBASE_RPI_PERIPHERALS_INTERNAL_GPIO_CTRL_H

#include "phymem_ptr.h"
#include "gpio_registers.h"
#include "pin_alloc.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief GPIO control type. There is only ONE (yes it is a singleton!)
    /// Groups BCM2708/2835 GPIO control registers physical memory mapped area
    /// with a(GPIO) pin allocator - with one slot for each GPIO pin.
      struct gpio_ctrl
      {
      /// @brief Pointer to BCM2708 / BCM2835 GPIO control registers instance
        phymem_ptr<volatile gpio_registers>   regs;

      /// @brief (GPIO) pin allocator
        pin_allocator alloc;

      /// @brief Singleton instance getter
      /// @returns THE instance of the GPIO pin control object.
        static gpio_ctrl & instance();

      private:
      /// @brief Construct: intialise regs with correct physical address & size 
        gpio_ctrl();

        gpio_ctrl(gpio_ctrl const &) = delete;
        gpio_ctrl(gpio_ctrl &&) = delete;
        gpio_ctrl & operator=(gpio_ctrl const &) = delete;
        gpio_ctrl & operator=(gpio_ctrl &&) = delete;
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_GPIO_CTRL_H
