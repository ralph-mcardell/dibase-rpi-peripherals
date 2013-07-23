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
      int     pin_event_fd;

    public:
      enum edge_mode
      { rising
      , falling
      , both
      };

      pin_edge_event(pin_id id, edge_mode mode);
      pin_edge_event(ipin const & in, edge_mode mode);
      pin_edge_event(pin_edge_event const &) = delete;
      pin_edge_event& operator=(pin_edge_event const &) = delete;
    // Note: move construction and assignment are possibly allowable.
    
    //  ~pin_edge_event();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
