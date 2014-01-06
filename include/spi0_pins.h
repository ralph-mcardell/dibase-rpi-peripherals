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
# include <cstdint>

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

  /// @brief Enumeration of SPI0 chip select polarity options
    enum class spi0_cs_polarity
    { low   ///< Active (asserted) low
    , high  ///< Active (asserted) high
    };

  /// @brief Enumeration of SPI0 communication modes
    enum class spi0_mode
    { none          ///< Indicates not communicating with any slave device
    , standard      ///< Standard 3-wire SPI mode - uses MOSI _and_ MISO
    , bidirectional ///< 2-wire SPI bidirectional mode - does not use MISO
    , lossi         ///< 2-wire LoSSI mode - does not use MISO
    };

  /// @brief Enumeration of LoSSI mode write type
    enum class spi0_lossi_write
    { data      ///< Writing LoSSI parameter data to slave device
    , command   ///< Writing LoSSI command to slave device
    };

  /// @brief Enumeration of valid SPI0 slave devices chip numbers
  /// 
  /// Note that only 2 devices are directly supported. Although the field is
  /// 2 bits in size value 2 de-selects all (both) devices and 3 (11 binary)
  /// is marked as 'reserved'.
    enum class spi0_slave
    { chip0 = 0U  ///< Device addressed by CS = 0 (assert CE0, de-assert CE1)
    , chip1 = 1U  ///< Device addressed by CS = 1 (de-assert CE0, assert CE1)
    };

  /// @brief Enumeration of SPI0 clock polarity options
    enum class spi0_clk_polarity
    { low   ///< Rest state of clock low
    , high  ///< Rest state of clock high
    };

  /// @brief Enumeration of SPI0 clock phase options.
    enum class spi0_clk_phase
    { middle  ///< Clock transitions at middle of data bit
    , start   ///< Clock transitions at start of data bit
    };

  /// @brief SPI0 peripheral slave device context
  ///
  /// spi0_slave_context objects contain information defining how the SPI0
  /// peripheral SPI master performs raw communication with a specific SPI
  /// slave device. A context is defined from construction parameters which
  /// include:
  ///
  /// - the slave's chip select value (0 or 1)
  /// - the required SPI0 clock frequency
  /// - the clock polarity and phase (defaults to low at rest, start of clock)
  /// - the communication mode (standard 3-wire SPI, bidirectional 2-wire SPI, 
  ///   or LoSSI 2-wire, or none at all) (defaults to standard 3-wire SPI)
  /// - the APB core frequency - which is fixed for a specific board boot
  ///   configuration and defaults to \ref rpi_apb_core_frequency.
  ///
  /// An spi0_slave_context is applied to the SPI0 peripheral when starting
  /// to converse with a slave chip via a spi0_pins object.
  ///
  /// Note that spi0_slave_context objects hold SPI0 peripheral register values
  /// representing the slave context's definition parameters along with the
  /// communication mode. As all of these are value types spi0_slave_context
  /// objects can be copied and assigned.
    class spi0_slave_context
    {
    friend class spi0_pins;

      std::uint32_t cs_reg;
      std::uint32_t clk_reg;
      std::uint32_t ltoh_reg;
      spi0_mode     mode;

    public:
    /// @brief Construct from slave context parameters
    ///
    /// The context defines which slave device to communicate with and how to 
    /// do so. Most of the parameters have defaults.
    ///
    /// @post Object is in a closed state.
    ///
    /// @param[in] cs     Chip select - which of the two chip enable lines
    ///                   should be asserted during an open conversation.
    /// @param[in] f      Required frequency of the SPI clock SCLK while
    ///                   communicating [\c fc/2, \c fc/65536]. Non-integral
    ///                   values of \c fc/f rounded down so actual frequency
    ///                   may be higher.
    /// @param[in] mode   Communications mode. Defaults to spi0_mode::standard,
    ///                   standard SPI 3-wire mode.
    /// @param[in] cpol   Clock polarity. Defaults to rest state low.
    /// @param[in] cpha   Clock phase. Defaults to clock transition in middle
    ///                   of data bit.
    /// @param[in] ltoh   LoSSI mode hold delay (in APB core clock ticks) [1,15]
    ///                   Only relevant for \c mode == spi0_mode::lossi.
    ///                   Defaults to 1.
    /// @param[in] fc     APB core frequency. Should be fixed for a given board
    ///                   boot configuration. Defaults to
    ///                   \ref rpi_apb_core_frequency.
    ///
    /// @throws std::invalid_argument if the \c cs parameter is invalid.
    /// @throws std::out_of_range if the \c f or \c ltoh parameters are not in
    ///         range.
      spi0_slave_context
      ( spi0_slave cs
      , hertz f
      , spi0_mode         mode  = spi0_mode::standard
      , spi0_clk_polarity cpol  = spi0_clk_polarity::low
      , spi0_clk_phase    cpha  = spi0_clk_phase::middle
      , std::uint32_t     ltoh  = 1U
      , hertz             fc    = rpi_apb_core_frequency
      );
    };

  /// @brief Use a set of 4 or 5 GPIO pins with the SPI0 peripheral.
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
  /// A spi0_pins object is constructed with a set of GPIO pin_ids to use for
  /// SPI0 signal functions and values for each chip enable line indicating
  /// which state (low or high) indicates a slave device is enabled - which
  /// default to enable when low if not given explicitly.
  /// 
  /// If all the pins in the pin set support the requisite SPI0 function
  /// and the SPI0 peripheral is not already in use locally within the same
  /// process then the SPI0 peripheral is set-up with the requested chip enable
  /// line polarities and the pins allocated and set to the relevant alt-fns.
  /// Note that no attempt is made to see if the SPI0 peripheral is in use
  /// externally by other processes.
  ///
  /// Once constructed spi0_slave_context objects may be used with the
  /// spi0_pins object to allow communicating with slave devices.
    class spi0_pins
    {
      constexpr static unsigned number_of_pins = 5U;

      std::array<pin_id_int_t, number_of_pins>  pins;
      spi0_mode                                 mode;

      void construct
      ( pin_id ce0
      , pin_id ce1
      , pin_id sclk
      , pin_id mosi
      , pin_id miso
      , spi0_cs_polarity  cspol0
      , spi0_cs_polarity  cspol1
      );

    public:
    /// @brief Construct a spi0_pins object from a spi0_pin_set specialisation
    /// and slave chip select assertion polarity values.
    ///
    /// @post The SPI0 peripheral is marked as in use.
    /// @post The pins in the spi0_pin_set are marked as in use (note: does
    ///       not include a pin for the MISO SPI0 function if it has a value of
    ///       \ref spi0_pin_not_used.
    /// @post The object is not conversing with a slave device (is_conversing() 
    ///       == \c false).
    /// @post The SPI0 peripheral CS register has chip select polarity bits
    ///       set appropriately for the cspol0, cspol1 parameter values. 
    ///
    /// @tparam CE0   spi0_pin_set CE0 template parameter.
    /// @tparam CE1   spi0_pin_set CE1 template parameter.
    /// @tparam SCLK  spi0_pin_set SCLK template parameter.
    /// @tparam MOSI  spi0_pin_set MOSI template parameter.
    /// @tparam MISO  spi0_pin_set MISO template parameter.
    ///
    /// @param[in] ps     spi0_pin_set specialisation specifying the set of
    ///                   GPIO pins to use for the various SPI0 functions.
    /// @param[in] cspol0 Chip 0 select polarity. Defaults to chip select
    ///                   line asserted when low (CE0 is low).
    /// @param[in] cspol1 Chip 1 select polarity. Defaults to chip select
    ///                   line asserted when low (CE1 is low).
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
      explicit spi0_pins
      ( spi0_pin_set<CE0,CE1,SCLK,MOSI,MISO> ps
      , spi0_cs_polarity  cspol0 = spi0_cs_polarity::low
      , spi0_cs_polarity  cspol1 = spi0_cs_polarity::low
      )
      {
        construct ( pin_id(ps.ce0()), pin_id(ps.ce1())
                  , pin_id(ps.sclk()), pin_id(ps.mosi()), pin_id(ps.miso())
                  , cspol0, cspol1
                  );
      }

    /// @brief Destroy: Stop any SPI0 conversation and de-allocate GPIO pins.
    /// @post Any conversation is stopped (CS register TA field set to 0).
    /// @post The pins allocated during construction are marked as free.
    /// @post The SPI0 peripheral is marked as free.
      ~spi0_pins();

      spi0_pins(spi0_pins const &) = delete;
      spi0_pins& operator=(spi0_pins const &) = delete;
      spi0_pins(spi0_pins &&) = delete;
      spi0_pins& operator=(spi0_pins &&) = delete;

    /// @brief Applies a spi0_slave_context, replacing any existing context and
    /// starts data transfers (CS register TA field == 1).
    ///
    /// @note
    /// Passing a slave context with a communication mode of spi0_mode::none
    /// will stop communication by setting the Transfer Active field to false.
    /// No other changes to control fields for an applied slave context are
    /// made. In general it is simpler to call stop_conversing().
    ///
    /// @post The SPI0 data transfer context will be that represented by the
    ///       object passed for the \c c parameter.
    ///
    /// @param  c   Conversation object specifying context for conversation.
    /// @throws std::invalid_argument if slave context is for 3-wire
    ///         standard mode and only 2-wire mode pins have been allocated
    ///         (i.e. \c c.mode==spi0_mode::standard && ! has_std_mode_support()).
      void  start_conversing(spi0_slave_context const & c);

    /// @brief Stop data transfers.
    /// @post Transfer Active field is \c false, stopping further data transfer
    ///       Communication mode is set to spi0_mode::none for the purposes of
    ///       reads and writes.
      void  stop_conversing();

    /// @brief Write a single byte to the transmit FIFO
    /// @param[in] data Data byte to be written.
    /// @param[in] lossi_write_type 
    ///                 Only relevant if using LoSSI communication mode.
    ///                 Specifies whether this is a LoSSI command or parameter
    ///                 data write. Default to parameter data write.
    /// @returns  \c true if byte written to transmit FIFO,
    ///           \c false if it could not as FIFO full or communication mode is
    ///           spi0_mode::none and conversation is stopped.
      bool write
      ( std::uint8_t data
      , spi0_lossi_write lossi_write_type=spi0_lossi_write::data
      );

    /// @brief Write bytes from buffer to the transmit FIFO
    ///
    /// @note
    /// For LoSSI mode only multiple parameter writes are supported.
    /// Write command  bytes using the single-byte overload of write with
    /// lossi_write_type==spi0_lossi_write::command.
    ///
    /// @param[in] pdata  Pointer to data bytes to be written.
    /// @param[in] count  Maximum number of bytes to write.
    /// @returns  Number of bytes actually written. Less than \c count if FIFO
    ///           fills. Zero if FIFO full or communication mode is
    ///           spi0_mode::none and conversation is stopped.
      std::size_t write
      ( std::uint8_t const * pdata
      , std::size_t count
      );

    /// @brief Read a single byte from the receive FIFO
    ///
    /// @note
    /// When in bidirectional mode a read has to be initiated. This is done
    /// by calling read when there is no data in the receive FIFO. This sets
    /// the Read Enable bit in the CS register and writes junk to the FIFO
    /// register to initiate a read transaction and returns false. The data
    /// should appear in the receive FIFO some time later.
    ///
    /// In LoSSI mode data is send to the receive FIFO following the writing
    /// of a read command to a slave device (using \ref write).
    ///
    /// @param[out] data Data byte to receive read value.
    /// @returns  \c true if byte read from receive FIFO,
    ///           \c false if it could not as FIFO empty or communication mode
    ///           is spi0_mode::none and conversation is stopped.
      bool read(std::uint8_t & data);

    /// @brief Read bytes from the receive FIFO into a buffer
    ///
    /// @note
    /// When in bidirectional mode a read has to be initiated. This is done
    /// by calling read when there is no data in the receive FIFO. This sets
    /// the Read Enable bit in the CS register and writes junk to the FIFO
    /// register to initiate a read transaction and returns false. The data
    /// should appear in the receive FIFO some time later.
    ///
    /// In LoSSI mode data is send to the receive FIFO following the writing
    /// of a read command to a slave device (using \ref write).
    ///
    /// @param[out] pdata Pointer to data buffer to receive read values.
    /// @param[in] count  Maximum number of bytes to read.
    /// @param[out] ppending_count 
    ///                   Only relevant for bidirectional mode. Defaults to
    ///                   \c nullptr. Pointer to std::size_t that will have the
    ///                   count of pending reads 'queued' up by performing
    ///                   writes of junk written to it.
    /// @returns  Number of bytes actually read. Less than \c count if FIFO
    ///           empties. Zero if FIFO empty or communication mode is
    ///           spi0_mode::none and conversation is stopped.
      std::size_t read
      ( std::uint8_t * pdata
      , std::size_t count
      , std::size_t * ppending_count=nullptr
      );

    /// @brief Query whether there is an on going conversation.
    ///
    /// An ongoing conversation is one in which the communication
    /// mode is not spi0_mode::none. If there is an ongoing conversation then
    /// data transfers will be active (CS register TA field will be 1) .
    ///
    /// @returns \c true if there is an ongoing SPI0 conversation, 
    ///          \c false if not.
      bool is_conversing() const;

    /// @brief Query whether SPI standard 3-wire protocol is supported
    ///
    /// If created with a spi_pin_set specialisation that did not
    /// include a value for a MISO pin to allocate and use then only 2
    /// wire protocols (SPI bidirectional and LoSSI modes) are supported.
    ///
    /// @returns  \c true if standard 3-wire SPI supported,
    ///           \c false if only 2-wire protocols supported.
      bool has_std_mode_support() const;

    /// @brief Query whether there is no data to write from the transmit FIFO.
    ///
    /// @returns \c true if there is no data to write from the transmit FIFO,
    ///          \c false if the transmit FIFO still contains data to write
    ///           _or_ if is_conversing()==\c false.
      bool write_fifo_is_empty() const;

    /// @brief Query whether there is room for more data in the transmit FIFO.
    ///
    /// @returns \c true if there is space to write data into the transmit FIFO,
    ///          \c false if the transmit FIFO is full.
      bool write_fifo_has_space() const;

    /// @brief Query whether there is no room for read data in the receive FIFO.
    ///
    /// @note
    /// When the receive FIFO is full and cannot accept any more
    /// incoming data all serial read _and_ write transfers are
    /// suspended until some data is read out from the receive FIFO.
    ///
    /// @returns \c true if the receive FIFO is full,
    ///          \c false if the receive FIFO still has room for more read data
      bool read_fifo_is_full() const;

    /// @brief Query whether there is any data to be read from the receive FIFO.
    ///
    /// @returns  \c true if there is data available to read data from the
    ///           receive FIFO, \c false if the receive FIFO is empty.
      bool read_fifo_has_data() const;

    /// @brief Query whether the receive FIFO is approaching being full and
    ///        needs reading.
    ///
    /// @returns  \c true if the receive FIFO is at least 75% full,
    ///           \c false if the receive FIFO is less than 75% full.
      bool read_fifo_needs_reading() const;
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_SPI0_PINS_H
