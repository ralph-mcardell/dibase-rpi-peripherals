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
            throw std::invalid_argument("pin_id::pin_id: Invalid GPIO pin number");
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
    
  /// @brief Return a pin_id with a value produced via a mapping array
  ///
  /// The returned pin_id's integer value is that given by the index key into
  /// a map array of integers viz:
  ///
  /// returned pin_id = pin_id( map[key] )
  ///
  /// @param[in]  key     Mapping key value
  /// @param[in]  map     Mapping array of pin_id integer values. If an array
  ///                     slot does not map to a valid pin_id use an invalid
  ///                     pin_id value to force a pin_id creation failure.
  /// @param[in] map_size Number of pin_id integer values in map.
  /// @return a pin_id created using the mapped value for the key:
  ///         pin_id(map[key])
  /// @exception  std::invalid_argument raised if key >= map_size or map[key]
  ///             is not a valid pin_id value.
    inline
    pin_id mapped_pin_id
    ( pin_id_int_t key
    , pin_id_int_t const * map
    , std::size_t map_size
    )
    {
      if ( key >= map_size )
        {
          throw std::invalid_argument("mapped_pin_id: Invalid GPIO pin number");
        }
      return pin_id(map[key]);
    }

  /// @brief Number of pins on Raspberry Pi P1 connector
    std::size_t const p1_pin_count{26};

  /// @brief Number of versions of Raspberry Pi P1 connector pin connections
    std::size_t const p1_versions{2};

  /// @brief Number of slotes for Raspberry Pi P1 connector pin_id map.
  ///
  /// As connector pins count from 1 and C/C++ arrays indexes count from 0 the
  /// map includes an extra slot for the non-existent pin 0.
    std::size_t const p1_map_size{p1_pin_count+1};

    /// @brief Mapping arrays for each version of Raspberry Pi P1 connector pins
  ///
  /// Each Raspberry Pi P1 connector version has its own vector mapping P1
  /// pin numbers to BCM2835 GPIO pin numbers (that is, pin_id values). One
  /// extra slot is required for the non-existent pin 0, which resolves to an
  /// invalid GPIO pin id value.
    extern pin_id_int_t p1_gpio_pin_map[p1_versions][p1_map_size];

  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_ID_H
