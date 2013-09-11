// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_pins.h 
/// @brief Use a set of GPIO pins for use with SPI0: type definitions.
///
/// The BCM2835 primary SPI serial interface peripheral supports one channel,
/// designated SPI0 in the Broadcom documentation. It supports 2 chip select
/// lines that allow for one of two slave SPI chips to be selected, or none.
/// Presumably it is possible to use extra GPIO lines and external logic to
/// allow more slave devices to be controlled. As common for bus linked devices,
/// the master can only be conversing (exchanging data with) one slave device
/// at a time.
///
/// Rather than requiring one GPIO pin to be allocated to the SPI function
/// a group of 5 GPIO pins are required for the full 3 wire standard SPI mode,
/// or 4 for 2 wire modes: 
///
///   - MOSI, MISO, SCLK, CE0, CE1, for standard 3 wire mode
///   - MOSI (as MOMI function), SCLK, CE0, CE1 in 2 wire bidirectional SPI
///     mode
///   - MOSI (as SDA), SCLK (as SCL) and CE0 & CE1 (as CS0 & CS1) in 2 wire
///     LoSSI mode.
///
/// For more details see the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 10 SPI for details
/// along with additional information on SPI found in the Gertboard source code.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_SPI0_PINS_H
# define DIBASE_RPI_PERIPHERALS_SPI0_PINS_H
# include "pin_id.h"
# include "clockdefs.h"
# include <array>

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief GPIO pin id used to indicate unused or not required pin
  ///
  /// Note that GPIO pin 53 has no useful alternative special functions
    constexpr pin_id_int_t spi0_pin_not_used{53U};

  /// @brief Simple constexpr type template to hold SPI0 pin sets
  /// @tparam CE0   SPI0 CE0 (chip enable line 0) GPIO pin number
  /// @tparam CE1   SPI0 CE1 (chip enable line 1) GPIO pin number
  /// @tparam SCLK  SPIO SCLK (SPI clock) GPIO pin number
  /// @tparam MOSI  SPIO MOSI (SPI master out slave in) GPIO pin number
  /// @tparam MISO  SPIO MISO (SPI master in slave out) GPIO pin number
  ///               Optional, defaults to spi0_pin_not_used indicating no
  ///               MISO pin in the pin set. MISO is not used by 2-wire modes.
    template  < pin_id_int_t CE0
              , pin_id_int_t CE1
              , pin_id_int_t SCLK
              , pin_id_int_t MOSI
              , pin_id_int_t MISO=spi0_pin_not_used
              >
    struct spi0_pin_set
    {
    /// @returns Specialisation type's CE0 parameter value
      constexpr pin_id_int_t ce0() { return CE0; }

    /// @returns Specialisation type's CE1 parameter value
      constexpr pin_id_int_t ce1() { return CE1; }

    /// @returns Specialisation type's SCLK parameter value
      constexpr pin_id_int_t sclk() { return SCLK; }

    /// @returns Specialisation type's MOSI parameter value
      constexpr pin_id_int_t mosi() { return MOSI; }

    /// @returns Specialisation type's MISO parameter value
      constexpr pin_id_int_t miso() { return MISO; }
    };

  /// @brief Full 5-pin SPI0 pin set provided by Raspberry Pi's P1 connector
    constexpr spi0_pin_set<8U, 7U, 11U, 10U, 9U>  rpi_p1_spi0_full_pin_set;

  /// @brief 2-wire mode only 4-pin SPI0 pin set provided by Raspberry Pi's P1
  /// connector
    constexpr spi0_pin_set<8U, 7U, 11U, 10U>    rpi_p1_spi0_2_wire_only_pin_set;
  
  /// @brief Type representing a conversation between the SPI0 master device
  /// and an SPI slave device.
    class spi0_conversation;

  /// @brief Use a GPIO pins for use with the SPI0 peripheral
  ///
  /// The control lines for the SPI0 serial interface peripheral may be
  /// output to a set of GPIO pins as special functions SPI0_CE0_N, SPI0_CE1_N,
  /// SPI0_MOSI, SPI0_MISO and SPI0_SCLK when set to the appropriate alternate
  /// pin functions. Refer to the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// BCM2835 ARM Peripherals data sheet</a>, table 6-31 to see which pin/alt
  /// function combinations support the required special functions. Note that
  /// for 2-wire modes (bidirectional SPI and LoSSI), only 4 of the 5 pins are
  /// required: MISO is not used so need not be allocated _if_ _all_ slave
  /// devices use a 2 wire protocol.
  ///
  /// A spi0_pin object is constructed with a set of GPIO pin_ids which are
  /// used to determine which, if any, SPI0 functions are supported and if so
  /// which pin alternative function to use. If the pin supports a SPI0 function
  /// and the SPI0 peripheral is not already in use locally within the same
  /// process then the SPI0 channel is set-up,  the pins allocated and set to
  /// the relevant alt-fns. Note that no attempt is made to see if the SPI0
  /// peripheral is in use externally by other processes.
  ///
  /// Once constructed the spi0_pins object may be used in the construction of
  /// spi0_conversation objects (one at a time).
    class spi0_pins
    {
      constexpr static unsigned number_of_pins = 5U;

      spi0_conversation const *                 open_conversation;
      std::array<pin_id_int_t, number_of_pins>  pins;

      void construct
      ( pin_id ce0
      , pin_id ce1
      , pin_id sclk
      , pin_id mosi
      , pin_id miso
      );

    public:
    /// @brief Construct a spi0_pins object from a spi0_pin_set specialisation
    ///
    /// @post The SPI0 peripheral is marked as in use
    /// @post The pins in the spi0_pin_set are marked as in use (note: does
    ///       not include a pin for the MISO SPI0 function if it has a value of
    ///       spi0_pin_not_used.
    /// @post The object has no associated spi0_conversation.
    ///
    /// @tparam CE0   spi0_pin_set CE0 template parameter
    /// @tparam CE1   spi0_pin_set CE1 template parameter
    /// @tparam SCLK  spi0_pin_set SCLK template parameter
    /// @tparam MOSI  spi0_pin_set MOSI template parameter
    /// @tparam MISO  spi0_pin_set MISO template parameter
    ///
    /// @param ps   spi0_pin_set specialisation specifying the set of GPIO pins
    ///             to use for the various SPI0 functions.
    ///
    /// @throws std::invalid_argument if any requested pin does not support the
    ///         required special function.
    /// @throws std::range_error if any pin supports the same SPI0 function
    ///         by more than one alternative function (should not be possible).
    /// @throws bad_peripheral_alloc if either any of the pins or the SPI0
    ///         peripheral are already in use.
      template  < pin_id_int_t CE0
                , pin_id_int_t CE1
                , pin_id_int_t SCLK
                , pin_id_int_t MOSI
                , pin_id_int_t MISO
                >
      explicit spi0_pins(spi0_pin_set<CE0,CE1,SCLK,MOSI,MISO> ps)
      : open_conversation{nullptr}
      {
        construct ( pin_id(ps.ce0()), pin_id(ps.ce1())
                  , pin_id(ps.sclk()), pin_id(ps.mosi()), pin_id(ps.miso())
                  );
      }

    /// @brief Destroy: close any SPI0 conversation and de-allocate GPIO pins.
    /// @post Any open spi0_conversation is closed.
    /// @post The pins allocated during construction are marked as free.
    /// @post The SPI0 peripheral is marked as free.
      ~spi0_pins();

      spi0_pins(spi0_pins const &) = delete;
      spi0_pins& operator=(spi0_pins const &) = delete;
      spi0_pins(spi0_pins &&) = delete;
      spi0_pins& operator=(spi0_pins &&) = delete;

    /// @brief Query whether there is an open SPI0 conversation
    /// @returns true if there is an open SPI0 conversation, false if not.
      bool has_conversation() const;

    /// @brief Query whether SPI standard 3-wire protocol is supported
    ///
    /// Note that if created with an spi_pin_set specialisation that did not
    /// include a value for a MISO  p[in to allocate and use then only 2
    /// wire protocols (SPI bidirectional and LoSSI modes) are supported.
    /// @returns  true if standard 3-wire SPI supported,
    ///           false if only 2-wire protocols supported
      bool has_std_mode_support() const;
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_SPI0_PINS_H
