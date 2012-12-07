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

  /// @brief pin_id subtype. Lookup id in a map by R'Pi version and pin keys
  ///
  /// rpi_version_mapped_pin_id is an intermediate type not intended for use in
  /// client code.
  ///
  /// Uses a pin number key and a map to lookup an associated BCM2835 GPIO pin
  /// id. The map contains entries mapping connector pins to GPIO pins for
  /// each supported Raspberry Pi board version, and the correct set choosen
  /// for the Raspberry Pi in use.
    class rpi_version_mapped_pin_id : public pin_id
    {
    protected:
    /// @brief Create rpi_version_mapped_pin_id from keys and mapping array
    ///
    /// Constructor for sub-class use.
    ///
    /// The base pin_id is intialised by the value for the associated pin key
    /// in a map vector. The vector may be one of several in a 2D array and is
    /// selected based on the Raspberry Pi board version. The lookup is like
    /// so: pin_id( map[rpi_board_version_index][connector_pin_number] )
    /// The Raspberry Pi board version index value is obtained from the 
    /// rpi_info.index_version() member function.
    ///
    /// @param[in]  pin Raspberry Pi connector pin number key value
    /// @param[in]  map Pointer to satrt of 2D mapping array of pin_id integer
    ///                 values having one mapping vector for each supported
    ///                 Raspberry Pi board version. If an array slot does not
    ///                 map to a valid pin_id use an invalid pin_id value which
    ///                 will force pin_id construction failure.
    /// @exception  std::invalid_argument raised if reported board version >=
    ///             n_versions, pin >= n_pins or map[version][pin] is not a
    ///             valid pin_id value.
      rpi_version_mapped_pin_id
      ( pin_id_int_t pin
      , pin_id_int_t const * map
      , pin_id_int_t n_pins
      , std::size_t n_versions
      );
    };

  /// @brief Number of versions of Raspberry Pi connectors pin outs supported
    std::size_t const pinout_versions{2};

  /// @brief Number of pins on Raspberry Pi P1 connector
    std::size_t const p1_pin_count{26};

  /// @brief Number of slots for Raspberry Pi P1 connector pin_id map.
  ///
  /// As connector pins count from 1 and C/C++ array indexes count from 0 the
  /// map includes an extra slot for the non-existent pin 0.
    std::size_t const p1_map_size{p1_pin_count+1};
    
  /// @brief Array mapping Raspberry Pi P1 connector pins to BCM2835 GPIO pins
  ///
  /// Each Raspberry Pi P1 connector version has its own vector mapping P1
  /// pin numbers to BCM2835 GPIO pin numbers (that is, pin_id values). One
  /// extra slot is required for the non-existent pin 0, which resolves to an
  /// invalid GPIO pin id value.
    extern pin_id_int_t p1_gpio_pin_map[pinout_versions][p1_map_size];

  /// @brief Raspberry Pi P1 connector pin representation
  /// 
  /// A rpi_version_mapped_pin_id sub-type that takes simply a value
  /// representing a P1 connector pin number. It converts to an integer having
  /// the value of the underlying BCM2835 GPIO pin id value or, for invalid,
  /// non-GPIO pin numbers or unsupported board versions throws an exception.
    struct p1_pin : rpi_version_mapped_pin_id
    {
    /// @brief Construct from a Raspberry Pi P1 connector pin number
    /// @param[in] pin_number   P1 connector pin number (1..26)
    /// @exception std::invalid_argument raised if pin_number out of range or
    ///             represents a pin with a non-GPIO function or the Raspberry
    ///             Pi board has an unsupported version number.
      p1_pin( pin_id_int_t pin_number )
      : rpi_version_mapped_pin_id
          ( pin_number
          , &(p1_gpio_pin_map[0][0])
          , p1_map_size
          , pinout_versions
          )
      {}
    };

  /// @brief Number of pins on Raspberry Pi P5 connector (V2 boards onwards)
    std::size_t const p5_pin_count{8};

  /// @brief Number of slots for Raspberry Pi P5 connector pin_id map.
  ///
  /// As connector pins count from 1 and C/C++ array indexes count from 0 the
  /// map includes an extra slot for the non-existent pin 0.
    std::size_t const p5_map_size{p5_pin_count+1};

  /// @brief Array mapping Raspberry Pi P5 connector pins to BCM2835 GPIO pins
  ///
  /// Each Raspberry Pi P5 connector version has its own vector mapping P5
  /// pin numbers to BCM2835 GPIO pin numbers (that is, pin_id values). One
  /// extra slot is required for the non-existent pin 0, which resolves to an
  /// invalid GPIO pin id value. AllP5 pins for version 1 boards map to invalid
  /// pins as there is no P5 on version 1 boards.
    extern pin_id_int_t p5_gpio_pin_map[pinout_versions][p5_map_size];

  /// @brief Raspberry Pi P5 connector pin representation
  /// 
  /// A rpi_version_mapped_pin_id sub-type that takes simply a value
  /// representing a P5 connector pin number. It converts to an integer having
  /// the value of the underlying BCM2835 GPIO pin id value or, for invalid,
  /// non-GPIO pin numbers or unsupported board versions throws an exception.
    struct p5_pin : rpi_version_mapped_pin_id
    {
    /// @brief Construct from a P5 connector pin number
    /// @param[in] pin_number   P5 GPIO pin number (1..8, 3..6 valid GPIO pins)
    /// @exception std::invalid_argument raised if pin_number out of range,
    ///             represents a pin with a non-GPIO function or the Raspberry
    ///             Pi board has an unsupported version number or does not
    ///             support P5.
      p5_pin( pin_id_int_t pin_number )
      : rpi_version_mapped_pin_id
          ( pin_number
          , &(p5_gpio_pin_map[0][0])
          , p5_map_size
          , pinout_versions
          )
      {}
    };

  /// @brief Static Raspberry Pi BCM2835 GPIO pin representation.
  /// Template class that takes a static BCM2835 GPIO pin id number as a
  /// template parameter and applies it only when first used as a pin_id. 
  /// This defers the work of creating the pin_id until first use.
  /// @param PIN  BCM2835 GPIO pin id number (0..53).
    template <pin_id_int_t PIN>
    struct static_pin_id
    {
    /// @brief Convert a static_pin_id to a pin_id
    /// @return a pin_id representing the value of the PIN template argument
    /// @exception  std::invalid_argument raised if PIN template parameter
    ///             value is out of range.
      operator pin_id() const;
    };

    template <pin_id_int_t PIN>    
    static_pin_id<PIN>::operator pin_id() const
    {
      static pin_id pinid{PIN};
      return pinid;
    }

  /// @brief Static Raspberry Pi P1 connector pin representation.
  /// Template class that takes a static P1 connector pin number as a template
  /// parameter and applies it only when first used as a pin_id to lookup and
  /// create a pin_id, or fail. This defers the work of performing the P1
  /// connector pin -> BCM2835 GPIO pin mapping and only has to happen if an
  /// object of the P1 pin is used and prevents pins that are only valid in
  /// certain board versions from throwing if used at the point of creation
  /// (e.g. pre-main dynamic static initialisation for global instances).
  /// @param PIN  Raspberry Pi P1 connector pin id number (1..26).
    template <pin_id_int_t PIN>
    struct static_p1_pin
    {
    /// @brief Convert a static_p1_pin to a pin_id
    /// @return a pin_id, specifically a p1_pin, representing the BCM2835 GPIO
    ///         pin connected to the P1 connector pin specified by the PIN
    ///         template parameter value.
    /// @exception  std::invalid_argument raised if the PIN template parameter
    ///             value is out of range or represents a pin with a non-GPIO
    ///             function or the Raspberry Pi board has an unsupported
    ///             version number.
      operator pin_id() const;
    };

    template <pin_id_int_t PIN>    
    static_p1_pin<PIN>::operator pin_id() const
    {
      static p1_pin pinid{PIN};
      return pinid;
    }

  /// @brief Static Raspberry Pi P5 connector pin representation.
  /// Template class that takes a static P5 connector pin number as a template
  /// parameter and applies it only when first used as a pin_id to lookup and
  /// create a pin_id, or fail. This defers the work of performing the P5
  /// connector pin -> BCM2835 GPIO pin mapping and only has to happen if an
  /// object of the P5 pin is used and prevents pins that are only valid in
  /// certain board versions from throwing if used at the point of creation
  /// (e.g. pre-main dynamic static initialisation for global instances).
  /// @param PIN  Raspberry Pi P5 connector pin id number (1..8, 3..6 valid).
    template <pin_id_int_t PIN>
    struct static_p5_pin
    {
    /// @brief Convert a static_p5_pin to a pin_id
    /// @return a pin_id, specifically a p5_pin, representing the BCM2835 GPIO
    ///         pin connected to the P5 connector pin specified by the PIN
    ///         template parameter value.
    /// @exception  std::invalid_argument raised if PIN template parameter
    ///             value is out of range, represents a pin with a non-GPIO
    ///             function or the Raspberry Pi board has an unsupported
    ///             version number or does not support P5.
      operator pin_id() const;
    };

    template <pin_id_int_t PIN>
    static_p5_pin<PIN>::operator pin_id() const
    {
      static p5_pin pinid{PIN};
      return pinid;
    }

  // Pre-created GPIO pin objects named for Raspberry Pi GPIO P1 and P5
  // connector pin names, but less numeric suffixes except for general GPIO
  // pins so as to avoid naming changes over board versions.
  // Note: For P1 pins that have not changed their P1 pin->BCM2835 GPIO pin id
  // mapping static_pid_id values are used that use the BCM2835 GPIO pin id
  // values directly as this should prevent dragging in the version check and
  // pin mapping machinery unnecessarily.
    static_p1_pin<3>  const sda;       ///< Raspberry Pi SDA0|1 pin, versioned, pull up
    static_p1_pin<5>  const scl;       ///< Raspberry Pi SCL0|1 pin, versioned, pull up
    static_pin_id<14> const txd;       ///< Raspberry Pi TXD0 pin
    static_pin_id<15> const rxd;       ///< Raspberry Pi RXD0 pin
    static_pin_id<10> const spi_mosi;  ///< Raspberry Pi SPI_MOSI pin
    static_pin_id<9>  const spi_miso;  ///< Raspberry Pi SPI_MISO pin
    static_pin_id<11> const spi_sclk;  ///< Raspberry Pi SPI_SCLK pin
    static_pin_id<8>  const spi_ce0_n; ///< Raspberry Pi SPI_CE0_N pin
    static_pin_id<7>  const spi_ce1_n; ///< Raspberry Pi SPI_CE1_N pin
    static_pin_id<4>  const gpio_gclk; ///< Raspberry Pi GPIO_GCLK pin
    static_pin_id<17> const gpio_gen0; ///< Raspberry Pi GPIO_GEN0 pin
    static_pin_id<18> const gpio_gen1; ///< Raspberry Pi GPIO_GEN1 pin
    static_p1_pin<13> const gpio_gen2; ///< Raspberry Pi GPIO_GEN2 pin, versioned
    static_pin_id<22> const gpio_gen3; ///< Raspberry Pi GPIO_GEN3 pin
    static_pin_id<23> const gpio_gen4; ///< Raspberry Pi GPIO_GEN4 pin
    static_pin_id<24> const gpio_gen5; ///< Raspberry Pi GPIO_GEN5 pin
    static_pin_id<25> const gpio_gen6; ///< Raspberry Pi GPIO_GEN6 pin
    static_p5_pin<3>  const gpio_gen7; ///< Raspberry Pi GPIO_GEN7 pin versioned V2 only
    static_p5_pin<4>  const gpio_gen8; ///< Raspberry Pi GPIO_GEN8 pin versioned V2 only
    static_p5_pin<5>  const gpio_gen9; ///< Raspberry Pi GPIO_GEN9 pin versioned V2 only
    static_p5_pin<6>  const gpio_gen10;///< Raspberry Pi GPIO_GEN10 pin versioned V2 only
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_ID_H
