// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.h 
/// @brief Single GPIO pin edge event class specification.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
# define DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H

# include "pin_id.h"
# include "pin.h"
# include <chrono>

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
      pin_id  id;           ///< Id of pin on which edge events watched for.

    /// @brief Internal non-templated member function to perform timed waits.
    /// @param t_rel_secs  Seconds portion of relative time to wait.
    /// @param t_rel_secs  Nano seconds portion of relative time to wait.
    /// @returns true if an event occured or false if no event occured and the
    ///          call timed out.
    /// @throws std::system_error if any system function call returns failure.
     bool     wait_(long t_rel_secs, long t_rel_ns) const;

    public:
    /// @brief Enumeration of edge transitions to monitor.
      enum edge_mode
      { rising    ///< Rising edge trasitions.
      , falling   ///< Falling edge transitions.
      , both      ///< Both rising and falling edge transitions.
      };

    /// @brief Construct from open ipin.
    /// The ipin should be open and the pin it represents should be exported
    /// in GPIO part of the sys file system - which is so if using the provided
    /// pinallocator.
    /// @param in   Open ipin to monitor for edge transitions.
    /// @param mode Which edge trasitions raise events.
    /// @throws std::invalid_argument if in not open or mode is invalid.
    /// @throws std::runtime_error on failure to open a pin mode setup file.
    /// @throws std::ios_base::failure on failure or error writing pin
    ///         setup information.
    /// @throws std::system_error if error obtaining watchable file descriptor.
    /// @throws bad_pin_alloc if the pin represented by in already has a
    ///         pin_edge_event object associated with it.
      pin_edge_event(ipin const & in, edge_mode mode);
 
      pin_edge_event(pin_edge_event const &) = delete;
      pin_edge_event& operator=(pin_edge_event const &) = delete;
      pin_edge_event(pin_edge_event &&) = delete;
      pin_edge_event& operator=(pin_edge_event &&) = delete;
    
    /// @brief Destroy, closing watchable file descriptor.
      ~pin_edge_event();

    /// @brief Check if a monitored edge event occured on the associated pin.
    /// @returns true if an event has been signalled, false if not.
    /// @throws std::system_error if any system function call returns failure.
      bool signalled() const;

    /// @brief Clear signalled event. Events remain signalled until cleared.
    /// @throws std::system_error if any system function call returns failure.
      void clear() const;

    /// @brief Wait for a monitored edge event to occur on the associated pin.
    /// @throws std::system_error if any system function call returns failure.
      void wait() const;

    /// @brief Wait for edge event for a given amount of time.
    /// @param (template) Rep     std::duration parameter see C++11 standard
    ///                           section 20.11.5.
    /// @param (template) Period  std::duration parameter see C++11 standard
    ///                           section 20.11.5.
    /// @param rel_time   Amount of time to wait for a monitored edge event
    ///                   to occur on the associated pin.
    /// @returns true if an event occured or false if no event occured and the
    ///          call timed out.
    /// @throws std::system_error if any system function call returns failure.
      template <class Rep, class Period>
      bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) const
      {
        using std::chrono::duration_cast;
        auto t_ns(duration_cast<std::chrono::nanoseconds>(rel_time).count());
        auto dur_secs(duration_cast<std::chrono::seconds>(rel_time));
        auto t_secs(dur_secs.count());
        auto t_secs_ns(duration_cast<std::chrono::nanoseconds>(dur_secs).count());
        return wait_(t_secs, t_ns-t_secs_ns);
      }

    /// @brief Wait for edge event until a given point in time.
    /// @param (template) Clock     std::time_point parameter see C++11
    ///                             standard section 20.11.6.
    /// @param (template) Duration  std::time_point parameter see C++11
    ///                             standard section 20.11.6.
    /// @param abs_time   Point in time to wait until for a monitored edge
    ///                   event to occur on the associated pin.
    /// @returns true if an event occured or false if no event occured and the
    ///          call timed out.
    /// @throws std::system_error if any system function call returns failure.
      template <class Clock, class Duration>
      bool
      wait_until(const std::chrono::time_point<Clock,Duration>& abs_time) const
      {
        return wait_for(abs_time - std::chrono::system_clock::now());
      }
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
