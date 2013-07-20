// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs.h 
/// @brief Linux sys filesystem utilities.
///
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_SYSFS_H
 #define DIBASE_RPI_PERIPHERALS_INTERNAL_SYSFS_H

 #include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
    /// @brief Check to see if GPIO pin is exported
    /// @param pin  Id of GPIO pin to check sys filesystem export status of
    /// @returns true if GPIO pin is exported in sys filesystem flase otherwise.
    /// @throws std::system_error if unexpected errors from system calls.
      bool is_exported(pin_id pin);

    /// @brief Export a GPIO pin in the sys filesystem
    /// @param pin  Id of GPIO pin export
    /// @returns true on success, flase on failure.
      bool export_pin(pin_id pin);

    /// @brief Unexport a GPIO pin from the sys filesystem
    /// @param pin  Id of GPIO pin unexport
    /// @returns true on success, flase on failure.
      bool unexport_pin(pin_id pin);
    }
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SYSFS_H
