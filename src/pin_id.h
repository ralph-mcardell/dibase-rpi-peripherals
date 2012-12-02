// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_id.h 
/// @brief Value classes supporting validated BCM2835 GPIO pin id numbers.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_ID_H
 #define DIBASE_RPI_PERIPHERALS_PIN_ID_H

 #include "peridef.h"
 #include "rpi_info.h"
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
            throw std::invalid_argument( "pin_id::pin_id: Invalid pin number or"
                                         " board revision is not supported"
                                       );
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
    
  /// @brief Subclass of pin_id that initialises value using a key into a map
  /// mapped_pin_id is an intermediate type not intended for use in client code.
  /// @param MAP_KEY_SIZE   Number of pin keys in a mapping vector as these
  ///                       will be constant for each connector's pin mapping.
    template <std::size_t MAP_KEY_SIZE>
    struct mapped_pin_id : public pin_id
    {
    /// @brief Create mapped_pin_id from key and mapping array
    ///
    /// The base pin_id is intialised by the value of a map array item at a key
    /// index viz: pin_id( map[key] )
    ///
    /// @param[in]  key     Mapping key value
    /// @param[in]  map     Mapping array of pin_id integer values. If an array
    ///                     slot does not map to a valid pin_id use an invalid
    ///                     pin_id value to force a pin_id creation failure.
    /// @return a pin_id created using the mapped value for the key:
    ///         pin_id(map[key])
    /// @exception  std::invalid_argument raised if key >= map_size or map[key]
    ///             is not a valid pin_id value.
      mapped_pin_id
      ( pin_id_int_t key
      , pin_id_int_t const * map
      )
      : pin_id(key>=MAP_KEY_SIZE ? -1 : map[key])
      {}
    };

  /// @brief Subclass of mapped_pin_id that notes Raspberry Pi board version
  /// rpi_version_mapped_pin_id is an intermediate type template not intended
  /// for use in client code.
  /// @param MAP_KEY_SIZE     Number of pin keys in a mapping vector.
  ///                         Needed for 2D [version][pin key] array type
  ///                         decls. Constant for each connector's pin mapping.
  /// @param MAP_VERSION_SIZE Number of board versions' mapping vectors.
  ///                         Constant for all connectors' pin mappings.
    template <std::size_t MAP_KEY_SIZE, std::size_t MAP_VERSION_SIZE>
    struct rpi_version_mapped_pin_id : public mapped_pin_id<MAP_KEY_SIZE>
    {
      typedef mapped_pin_id<MAP_KEY_SIZE> super;  ///< Super class type alias

    /// @brief Create rpi_version_mapped_pin_id from key and mapping array
    ///
    /// The base pin_id is intialised by the value in a map vector in a 2D array
    /// selected based on the Raspberry Pi board version item at a key
    /// index viz: pin_id( map[rpi_board_version_index][key] )
    /// The Raspberry Pi board version index value is obtained from the 
    /// rpi_info.index_version() member function.
    ///
    /// @param[in]  key             Mapping key value
    /// @param[in]  map             Mapping array of pin_id integer values, one
    ///                             mapping vector for each supported Raspberry
    ///                             Pi board version. If an array  slot does
    ///                             not map to a valid pin_id use an invalid
    ///                             pin_id value to force a failure.
    /// @return a pin_id created using the mapped value for the key:
    ///         pin_id(map[key])
    /// @exception  std::invalid_argument raised if reported board version >=
    ///             MAP_VERSION_SIZE, key >= MAP_KEY_SIZE or map[key] is not a
    ///             valid pin_id value.
      rpi_version_mapped_pin_id
      ( pin_id_int_t key
      , pin_id_int_t const map[][MAP_KEY_SIZE]
      )
      : super( rpi_info().index_version()>=MAP_VERSION_SIZE ? -1 : key
             , map[rpi_info().index_version()]
             )
      {}
    };

  /// @brief Number of versions of Raspberry Pi connectors pin out supported
    std::size_t const pinout_versions{2};

  /// @brief Number of pins on Raspberry Pi P1 connector
    std::size_t const p1_pin_count{26};

  /// @brief Number of slots for Raspberry Pi P1 connector pin_id map.
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
    extern pin_id_int_t p1_gpio_pin_map[pinout_versions][p1_map_size];

  /// @brief Raspberry Pi P1 connector pin representation
  /// 
  /// A rpi_version_mapped_pin_id specialisation sub-type that takes simply
  /// a value representing a P1 connector pin number. It converts to an
  /// integer having the value of the underlying BCM2835 GPIO pin id value or,
  /// for invalid, non-GPIO pin numbers or unsupported board versions throws
  /// and exception.
    struct p1_pin : rpi_version_mapped_pin_id<p1_map_size, pinout_versions>
    {
    /// @brief Super class type alias
      typedef rpi_version_mapped_pin_id<p1_map_size, pinout_versions> super;

    /// @brief Construct from a P1 connector pin_number
    /// @param[in] pin_number   P1 GPIO pin number (1..26)
    /// @exception std::invalid_argument raised if pin_number out of range or
    ///             represents a pin with a non-GPIO function or the Raspberry Pi
    ///             board has an unsupported version number.
      p1_pin( pin_id_int_t pin_number )
      : super( pin_number, p1_gpio_pin_map )
      {}
    };

  /// @brief Number of pins on Raspberry Pi P5 connector (V2 baords onwards)
    std::size_t const p5_pin_count{8};

  /// @brief Number of slotes for Raspberry Pi P5 connector pin_id map.
  ///
  /// As connector pins count from 1 and C/C++ arrays indexes count from 0 the
  /// map includes an extra slot for the non-existent pin 0.
    std::size_t const p5_map_size{p5_pin_count+1};

  /// @brief Mapping arrays for each version of Raspberry Pi P5 connector pins
  ///
  /// Each Raspberry Pi P5 connector version has its own vector mapping P5
  /// pin numbers to BCM2835 GPIO pin numbers (that is, pin_id values). One
  /// extra slot is required for the non-existent pin 0, which resolves to an
  /// invalid GPIO pin id value. All mappings for V1 P5 map to invalid pins as 
  /// there is no P5 on V1 boards.
    extern pin_id_int_t p5_gpio_pin_map[pinout_versions][p5_map_size];

  /// @brief Raspberry Pi P5 connector pin representation
  /// 
  /// A rpi_version_mapped_pin_id specialisation sub-type that takes simply
  /// a value representing a P5 connector pin number. It converts to an
  /// integer having the value of the underlying BCM2835 GPIO pin id value or,
  /// for invalid, non-GPIO pin numbers or unsupported board versions throws
  /// and exception.
    struct p5_pin : rpi_version_mapped_pin_id<p5_map_size, pinout_versions>
    {
    /// @brief Super class type alias
      typedef rpi_version_mapped_pin_id<p5_map_size, pinout_versions> super;

    /// @brief Construct from a P5 connector pin_number
    /// @param[in] pin_number   P5 GPIO pin number (1..8, 3..6 valid GPIO pins)
    /// @exception std::invalid_argument raised if pin_number out of range or
    ///             represents a pin with a non-GPIO function or the Raspberry
    ///             Pi board has an unsupported version number or does not
    ///             support P5.
      p5_pin( pin_id_int_t pin_number )
      : super( pin_number, p5_gpio_pin_map )
      {}
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_ID_H

