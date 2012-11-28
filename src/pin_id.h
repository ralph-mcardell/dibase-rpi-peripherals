// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_id.h 
/// @brief Value classes supporting validated BCM2835 GPIO pin id numbers.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_ID_H
 #define DIBASE_RPI_PERIPHERALS_PIN_ID_H

 #include "peridef.h"
 #include <stdexcept>

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Value class validating and wrapping integer GPIO pin id value.
  ///
  /// A pin_id represents a BCM2835 GPIO pin id number. It wraps a positive
  /// integer value that is set and validated on construction. pid_id instances
  /// must be explicitly constructed from a positive integer value which if
  /// not a valid GPIO pin id number causes an exception to be thrown.
  ///
  /// pin_id instances may be converted back to their associated integer type
  /// defined by the pin_id_int_t type alias.
    class pin_id
    {
    public:
    /// @brief Minimum BCM2835 GPIO pin id number.
      static pin_id_int_t const min_id = 0;

    /// @brief Maximum BCM2835 GPIO pin id number.
      static pin_id_int_t const max_id = 53;

    /// @brief Number of BCM2835 GPIO pins.
      static pin_id_int_t const number_of_pins = max_id - min_id + 1;

    /// @brief Explicitly construct a pin_id from an integer value.
    ///
    /// The integer value must be in the range [min_id..max_id]. Values outside
    /// this range cause a std::invalid_argument exception to be thrown.
    ///
    /// @param[in]  v   Integer value of pin id number to represent.
    /// @exception std::invalid_argument raised if v > max_id or v < min_id
      explicit pin_id( pin_id_int_t v )
      : value( v )
      {
        if ( v > max_id ) // v < min_id not needed with v unsigned and min_id=0
          {
            throw std::invalid_argument("pin_id::pin_id: Invalid GPIO pin id value");
          }
      }

    /// @brief Convert a pin_id to an integer pin id number
    /// @return an integer with the same value used to construct the pin_id 
      operator pin_id_int_t()
      {
        return value;
      }

    private:
      pin_id_int_t  value;  ///< Wrapped integer value of the pin id.
    };
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_PIN_ID_H
