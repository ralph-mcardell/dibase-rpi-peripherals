// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.h 
/// @brief Single GPIO input pin edge event class specification.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H
# define DIBASE_RPI_PERIPHERALS_PIN_EDGE_EVENT_H

# include "pin.h"
# include <chrono>

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief GPIO pin edge event abstraction.
  ///
  /// Allows working with GPIO pin input edge events.
  ///
  /// pin_edge_event instances may be created for single input GPIO pins.
  /// The implementation relies on the sys file system GPIO input pin event
  /// support, hence \ref ipin instances associated with pin_edge_event
  /// instances _must_ be exported in the sys file system - which will be so
  /// if using the provided pin allocator.
    class pin_edge_event
    {
      int     pin_event_fd;
      pin_id  id;

      bool     wait_(long t_rel_secs, long t_rel_ns) const;

    public:
    /// @brief Monitored edge transition type options
      enum edge_mode
      { rising    ///< Rising edge transitions.
      , falling   ///< Falling edge transitions.
      , both      ///< Both rising and falling edge transitions.
      };

    /// @brief Construct from open \ref ipin.
    ///
    /// The \ref ipin should be open and the pin it represents should be
    /// exported in GPIO part of the sys file system - which is so if using
    /// the provided pin allocator.
    /// @param[in] in   Open \ref ipin to monitor for edge transitions.
    /// @param[in] mode Which edge transition types raise events.
    /// @throws std::invalid_argument if \b in not open or mode is invalid.
    /// @throws std::runtime_error on failure to open a pin mode set-up file.
    /// @throws std::ios_base::failure on failure or error writing pin
    ///         set-up information.
    /// @throws std::system_error if error obtaining watchable file descriptor.
    /// @throws bad_pin_alloc if the pin represented by \b in already has a
    ///         pin_edge_event object associated with it.
      pin_edge_event(ipin const & in, edge_mode mode);
 
      pin_edge_event(pin_edge_event const &) = delete;
      pin_edge_event& operator=(pin_edge_event const &) = delete;
      pin_edge_event(pin_edge_event &&) = delete;
      pin_edge_event& operator=(pin_edge_event &&) = delete;
    
    /// @brief Destroy, closing watchable file descriptor.
      ~pin_edge_event();

    /// @brief Check if a monitored edge event occurred on the associated pin.
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
    /// @tparam Rep       template parameter for std::chrono::duration -
    ///                   see C++11 standard section 20.11.5.
    /// @tparam Period    template parameter for std::chrono::duration -
    ///                   see C++11 standard section 20.11.5.
    /// @param[in] rel_time   Amount of time to wait for a monitored edge event
    ///                   to occur on the associated pin.
    /// @returns true if an event occurred or false if no event occurred and
    ///          the call timed out.
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
    /// @tparam Clock     template parameter for std::chrono::time_point -
    ///                   see C++1 standard section 20.11.6.
    /// @tparam Duration  template parameter for std::chrono::time_point -
    ///                   see C++1 standard section 20.11.6.
    /// @param[in] abs_time   Point in time to wait until for a monitored edge
    ///                   event to occur on the associated pin.
    /// @returns true if an event occurred or false if no event occurred and
    ///          the call timed out.
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
