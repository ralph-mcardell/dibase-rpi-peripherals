// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs.h 
/// @brief Linux sys filesystem utilities.
///
///
/// @copyright Copyright (c) Dibase Limited 2013
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
    
    /// @brief Internal use input pin edge event mode values
    ///
    /// rising  : edge events on low to high rising edges
    /// falling : edge events on high to low falling edges
    /// both    : edge events on rising and falling edges
    /// bad_mode: should never be used. Will cause std::invalid_argument to be
    ///           thrown from open_ipin_for_edge_events. Assigned only when
    ///           converting from external mode value types which are
    ///           themselves invalid.
      enum class edge_event_mode{rising, falling, both, bad_mode=0xbad};

    /// @brief Open a pin for use in edge events IO multiplexing.
    /// @param pin  Id of input pin to open for edge event monitoring uses
    /// @param mode Indication of edge events (rising, falling or both) to 
    ///             monitor.
    /// @returns File descriptor value that can be used with system
    ///          functions such as select or -1 on open failure.
    /// @throws std::invalid_argument if mode value is bad_mode or invalid.
    /// @throws std::runtime_error on failure to open a pin mode setup file
    /// @throws std::ios_base::failure on failure or error writing pin
    ///         setup information
    /// @throws std::system_error if unexpected error obtaining file descriptor.
      int open_ipin_for_edge_events(pin_id pin, edge_event_mode mode);

    /// @brief close file previously opened by open_ipin_for_edge_events.
    /// @param pin_fd file descriptor returned by open_ipin_for_edge_events.
    /// @returns true if close sucessful, false otherwise.
      bool close_ipin_for_edge_events(int pin_fd);
    }
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SYSFS_H
