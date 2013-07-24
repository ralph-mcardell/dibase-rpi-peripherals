// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.h 
/// @brief Single GPIO pin edge event class specifications.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
 #define DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H

 #include "pin_id.h"
 #include "pin.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief GPIO pin edge event abstraction.
  ///
  /// Allows working with GPIO pin edge events.
  ///
  /// pin_edge_event instances may be created for single input GPIO pins.
  /// GPIO pins associated with pin_edge_event instances _must_ be exported in
  /// the sys file system and should be expected to be used as input pins.
  /// This is because the implementation has to use the GPIO sys file system
  /// interface and specifies pins having an "in" direction.
    class pin_edge_event
    {
      int     pin_event_fd; ///< Watchable file descriptor for edge events.

    public:
    /// @brief Enumeration of edge transitions to monitor
      enum edge_mode
      { rising    ///< Rising edge trasitions
      , falling   ///< Falling edge transitions
      , both      ///< Both rising and falling edge transitions
      };

    /// @brief Construct from pin_id value
    /// The pin id should be exported in GPIO part of the sys file system.
    /// @param pin_id id of pin to monitor for edge transitions
    /// @param mode   edge_mode enumeration value indicating which edge
    ///               trasitions raise events
    /// @throws std::invalid_argument if mode value is invalid.
    /// @throws std::runtime_error on failure to open a pin mode setup file
    /// @throws std::ios_base::failure on failure or error writing pin
    ///         setup information
    /// @throws std::system_error if error obtaining watchable file descriptor.
      pin_edge_event(pin_id id, edge_mode mode);

    /// @brief Construct from open ipin
    /// The ipin id should be open and the pin it represents should be exported
    /// in GPIO part of the sys file system - which is so if using the provided
    /// pinallocator.
    /// @param in    Open ipin to monitor for edge transitions
    /// @param mode   edge_mode enumeration value indicating which edge
    ///               trasitions raise events
    /// @throws std::invalid_argument if in not open or mode is invalid.
    /// @throws std::runtime_error on failure to open a pin mode setup file
    /// @throws std::ios_base::failure on failure or error writing pin
    ///         setup information
    /// @throws std::system_error if error obtaining watchable file descriptor.
      pin_edge_event(ipin const & in, edge_mode mode);
 
      pin_edge_event(pin_edge_event const &) = delete;
      pin_edge_event& operator=(pin_edge_event const &) = delete;
    // Note: move construction and assignment are possibly allowable.
    
    /// @brief Destroy closing watchable file descriptor
      ~pin_edge_event();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
