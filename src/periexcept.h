// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file periexcept.h 
/// @brief Peripheral related exception types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PERIEXCEPT_H
# define DIBASE_RPI_PERIPHERALS_PERIEXCEPT_H

# include <stdexcept>

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Base for all dibase::rpi::peripherals exceptions.
    struct peripheral_exception : std::runtime_error
    {
    /// @brief Construct from an explanatory std::string.
    /// @param[in]  what_arg  std::string explaining error
      peripheral_exception(std::string const & what_arg)
      : std::runtime_error{what_arg}
      {}

    /// @brief Construct from an explanatory C string.
    /// @param[in]  what_arg  C string explaining error
      peripheral_exception(char const * what_arg)
      : std::runtime_error{what_arg}
      {}
    };

  /// @brief Exception type raised if a peripheral cannot be allocated.
    struct bad_peripheral_alloc : peripheral_exception
    {
    /// @brief Construct from an explanatory std::string.
    /// @param[in]  what_arg  std::string explaining allocation failure
      bad_peripheral_alloc(std::string const & what_arg)
      : peripheral_exception{what_arg}
      {}

    /// @brief Construct from an explanatory C string.
    /// @param[in]  what_arg  C string explaining allocation failure
      bad_peripheral_alloc(char const * what_arg)
      : peripheral_exception{what_arg}
      {}
    };

  /// @brief Exception type raised when expecting a pin to be (un)exported
  /// Some functionality relies on sys filesystem GPIO support and expects
  /// pins to be either exported or unexported.
    struct bad_pin_export_state : peripheral_exception
    {
    /// @brief Construct from an explanatory std::string.
    /// @param[in]  what_arg  std::string explaining error
      bad_pin_export_state(std::string const & what_arg)
      : peripheral_exception{what_arg}
      {}

    /// @brief Construct from an explanatory C string.
    /// @param[in]  what_arg  C string explaining error
      bad_pin_export_state(char const * what_arg)
      : peripheral_exception{what_arg}
      {}
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PERIEXCEPT_H
