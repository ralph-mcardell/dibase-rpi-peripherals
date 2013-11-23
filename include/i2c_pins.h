// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_pins.h 
/// @brief Use a pair of GPIO pins for use with I2C/BSC peripherals:
///        type definitions.
///
/// The BCM2835 supports 3 I2C style serial interfaces called BSC (for
/// Broadcom Serial Controller) in the Broadcom documentation. The peripheral
/// are known as BSC0, BSC1 and BSC2. Only BSC0 and BSC1 are for general use
/// (BSC2 is used by the HDMI interface). Each BSC peripheral requires 2 GPIO
/// lines for the I2C SCL (serial clock) and SDA (serial data) lines.
/// For more details see the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 3 BSC for details.
/// The I2C bus information can be found in the
/// <a href="http://www.nxp.com/documents/user_manual/UM10204.pdf">
/// I2C-bus specification and user manual</a> from NXP Semiconductors.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_I2C_PINS_H
# define DIBASE_RPI_PERIPHERALS_I2C_PINS_H
# include "pin_id.h"
# include "clockdefs.h"
# include <array>
# include <cstdint>

namespace dibase { namespace rpi {
  namespace peripherals
  {

    /// @brief Default value for constructor f parameters - BSC/I2C bus clock
    /// frequency. Note: 100KHz is the maximum frequency for I2C standard mode
    ///
    /// Note: not a class static as in that case g++ thinks storage required
    ///       causing ld to complain when storage (i.e. definition) not found!
    ///
      constexpr hertz  i2c_pins_default_frequency = hertz{100000};

  /// @brief Use a pair of GPIO pins with a I2C / BSC peripheral.
  ///
  /// Each of the BSC peripherals that can be mapped to GPIO pins have multiple
  /// GPIO pin pairs that can be used. Unfortunately one such pair of pins
  /// can support either of the BSC peripherals under different alternative
  /// functions. In the Raspberry Pi case this is not a problem as the pin pair
  /// in question are not used. However additional support for optionally
  /// specifying which BSC peripheral to use is provided.
  ///
  /// A i2c_pins object is constructed with the pin pair to use, and optionally
  /// which BSC peripheral to use in the case of the ambiguity mentioned above,
  /// I2C bus frequency and timing parameters, which have defaults.
  ///
  /// Assuming the parameters are sane, the GPIO pins are available and the BSC
  /// peripheral is not already in use locally within the same process then the
  /// BSC peripheral is set-up as per the parameters, with the GPIO pins and
  /// BSC peripheral marked as in use. Note that no attempt is made to see if
  /// the BSC peripheral is in use externally by other processes.
    class i2c_pins
    {
      constexpr static unsigned number_of_pins = 2U;

      std::array<pin_id_int_t, number_of_pins>  pins;
      std::size_t                               bsc_idx; // 0 or 1

    public:
    /// @brief Error state enumeration
      enum state
      { goodbit = 0         ///< No BSC errors
      , timeoutbit = 1      ///< Slave stretched clock beyond the set time out
      , noackowledgebit = 2 ///< Slave did not acknowledge its address
      };

    /// @brief Default value for constructor tout parameters, the BSC/I2C bus
    /// clock stretch time-out value.
      constexpr static std::uint16_t default_tout = 0x40U;

    /// @brief Default value for constructor fedl parameters, the BSC/I2C bus
    /// SCL falling edge delay before transmitting next data bit
      constexpr static std::uint16_t default_fedl = 0x30U;

    /// @brief Default value for constructor redl parameters, the BSC/I2C bus
    /// SCL rising edge delay before reading next data bit
      constexpr static std::uint16_t default_redl = 0x30U;

    /// @brief Construct from GPIO pin pair and BSC parameters.
    ///
    /// Note: This constructor should _not_ be used if either pin is ambiguous
    /// with respect to which BSC peripheral should be used.
    ///
    /// Creates an i2c_pins object from a pair of GPIO pins values and
    /// optionally BSC frequency and timing values. The pair of GPIO pins
    /// specified _must_ both support a function for exactly _one_ ( 1 ) BSC
    /// peripheral.
    ///
    /// @post the GPIO pins indicated by the sda_pin and scl_pin parameters
    ///       will be allocated as in use within the process and outside the
    ///       process if such a GPIO pin allocator is used.
    /// @post the BSC peripheral associated with the GPIO pins is marked
    ///       as allocated and in use within the process only.
    /// @post The GPIO pins with be set to be used with the appropriate BSC
    ///       peripheral
    /// @post The BSC device will have its clock frequency and timings set
    ///       according to the relevant parameters.
    /// @param[in] sda_pin  GPIO pin id number of the I2C/BSC SDA (serial data)
    ///                     line
    /// @param[in] scl_pin  GPIO pin id number of the I2C/BSC SCL (serial clock)
    ///                     line
    /// @param[in] f        Frequency of the I2C/BSC clock SCL in the range 
    ///                     [fc/2, fc/32768]. Non-integral values of fc/f
    ///                     rounded down so actual frequency may be higher.
    ///                     Defaults to 100KHz.
    /// @param[in] tout     Clock stretch time-out in SCL cycles in the range
    ///                     [0,65535], 0 indicates feature is disabled. The
    ///                     amount of time the peripheral assumes a slave device
    ///                     can stretch a clock pulse before deciding it is
    ///                     not responding. Defaults to 0x40 (64) SCL cycles.
    /// @param[in] fedl     Number of _core_ clock cycles to wait after the
    ///                     falling edge of SCL before outputting next bit of
    ///                     data. In the range [0,65535] but must not exceed 
    ///                     fc/(2*f). Defaults to 0x30 (48) core clock cycles.
    /// @param[in] redl     Number of _core_ clock cycles to wait after the
    ///                     rising edge of SCL before reading the next bit of
    ///                     data. In the range [0,65535] but must not exceed 
    ///                     fc/(2*f). Defaults to 0x30 (48) core clock cycles.
    /// @param[in] fc       APB core frequency \ref frequency type. Should be
    ///                     fixed for a given board boot configuration.
    ///                     Defaults to rpi_apb_core_frequency.
    ///
    /// @throws std::invalid_argument if either requested pin does not support
    ///         the required special function or their functions are for
    ///         different BSC peripherals.
    /// @throws std::range_error if any pin supports the same BSC function
    ///         by more than one alternative function -- use the constructor
    ///         taking an integer index indicating which BSC peripheral
    ///         explicitly as a 3rd parameter.
    /// @throws std::out_of_range if the f, fedl or redl parameters are not
    ///         in range. 
    /// @throws bad_peripheral_alloc if either any of the pins or the BSC
    ///         peripheral are already in use.
      i2c_pins( pin_id        sda_pin
              , pin_id        scl_pin
              , hertz         f     = i2c_pins_default_frequency
              , std::uint16_t tout  = default_tout
              , std::uint16_t fedl  = default_fedl
              , std::uint16_t redl  = default_redl
              , hertz         fc    = rpi_apb_core_frequency
              );

    /// @brief Construct from GPIO pin pair, BSC peripheral index and BSC
    ///        parameters
    ///
    /// Note: Use this constructor if either pin is ambiguous with respect to
    /// which BSC peripheral should be used.
    ///
    /// Creates an i2c_pins object from a pair of GPIO pins values and an index
    /// value that explicitly specifies whether BSC0 or BSC1 is to be used and
    /// optionally BSC frequency and timing values. The pair of GPIO pins
    /// specified _must_ both support a function for exactly _one_ ( 1 ) BSC
    /// peripheral.
    ///
    /// @post the GPIO pins indicated by the sda_pin and scl_pin parameters
    ///       will be allocated as in use within the process and outside the
    ///       process if such a GPIO pin allocator is used.
    /// @post the BSC peripheral associated with the GPIO pins is marked
    ///       as allocated and in use within the process only.
    /// @post The GPIO pins with be set to be used with the appropriate BSC
    ///       peripheral
    /// @post The BSC device will have its clock frequency and timings set
    ///       according to the relevant parameters.
    /// @param[in] sda_pin  GPIO pin id number of the I2C/BSC SDA (serial data)
    ///                     line
    /// @param[in] scl_pin  GPIO pin id number of the I2C/BSC SCL (serial clock)
    ///                     line
    /// @param[in] bsc_num  BSC peripheral number specifying BSC0 or BSC1
    ///                     In the range [0,1]; 0 to use BSC0, 1 to use BSC1.
    /// @param[in] f        Frequency of the I2C/BSC clock SCL in the range 
    ///                     [fc/2, fc/32768]. Non-integral values of fc/f
    ///                     rounded down so actual frequency may be higher.
    ///                     Defaults to 100KHz.
    /// @param[in] tout     Clock stretch time-out in SCL cycles in the range
    ///                     [0,65535], 0 indicates feature is disabled. The
    ///                     amount of time the peripheral assumes a slave device
    ///                     can stretch a clock pulse before deciding it is
    ///                     not responding. Defaults to 0x40 (64) SCL cycles.
    /// @param[in] fedl     Number of _core_ clock cycles to wait after the
    ///                     falling edge of SCL before outputting next bit of
    ///                     data. In the range [0,65535] but must not exceed 
    ///                     fc/(2*f). Defaults to 0x30 (48) core clock cycles.
    /// @param[in] redl     Number of _core_ clock cycles to wait after the
    ///                     rising edge of SCL before reading the next bit of
    ///                     data. In the range [0,65535] but must not exceed 
    ///                     fc/(2*f). Defaults to 0x30 (48) core clock cycles.
    /// @param[in] fc       APB core frequency \ref frequency type. Should be
    ///                     fixed for a given board boot configuration.
    ///                     Defaults to rpi_apb_core_frequency.
    ///
    /// @throws std::invalid_argument if either requested pin does not support
    ///         the required special function or their functions are for
    ///         different BSC peripherals.
    /// @throws std::range_error if any pin supports the same BSC function
    ///         by more than one alternative function -- should not occur.
    /// @throws std::out_of_range if the f, fedl or redl parameters are not
    ///         in range or bsc_num is not 0 or 1.
    /// @throws bad_peripheral_alloc if either any of the pins or the BSC
    ///         peripheral are already in use.
      i2c_pins( pin_id        sda_pin
              , pin_id        scl_pin
              , int           bsc_num   
              , hertz         f     = i2c_pins_default_frequency
              , std::uint16_t tout  = default_tout
              , std::uint16_t fedl  = default_fedl
              , std::uint16_t redl  = default_redl
              , hertz         fc    = rpi_apb_core_frequency
              );

    /// @brief Destroy: Clear data, abort any transfer, free GPIO pins & BSC
    /// peripheral.
    /// @post The FIFO is cleared, any in progress transfer is aborted
    /// @post The BSC peripheral is disabled
    /// @post The BSC peripheral is marked as free.
    /// @post The pins allocated during construction are marked as free.
      ~i2c_pins();

      i2c_pins(i2c_pins const &) = delete;
      i2c_pins& operator=(i2c_pins const &) = delete;
      i2c_pins(i2c_pins &&) = delete;
      i2c_pins& operator=(i2c_pins &&) = delete;

    /// @brief Start a write transaction to the specified slave device and
    /// write initial bytes from a buffer to the FIFO for transmission to the
    /// slave.
    ///
    /// @param[in] addrs  Slave address [0,127]. Note: values [0,7],[120,127]
    ///                   have special meanings in the I2C specification.
    /// @param[in] dlen   Number of bytes to be written for transaction [0,65535].
    /// @param[in] data   Pointer to data bytes to be written. Pass nullptr if
    ///                   to write nothing to FIFO initially.
    /// @param[in] count  Maximum number of bytes to write in this call
    /// @returns  Number of bytes actually written to the FIFO.
    ///           Less than count if FIFO fills.
    /// @throws std::out_of_range if the addrs or dlen parameters are not
    ///         in the allowed range of value.
    /// @throws std::logic_error if there is already a transaction in progress.
      std::size_t start_write
      ( std::uint32_t addrs
      , std::uint32_t dlen
      , std::uint8_t const * pdata
      , std::size_t count
      );

    /// @brief Write bytes from buffer to the FIFO for transmission to the
    /// slave addressed in a currently active write operation
    ///
    /// @param[in] data   Pointer to data bytes to be written
    /// @param[in] count  Maximum number of bytes to write
    /// @returns  Number of bytes actually written. Less than count if FIFO
    ///           fills. Zero if FIFO full or not currently in an active write
    ///           transaction.
      std::size_t write
      ( std::uint8_t const * pdata
      , std::size_t count
      );

    /// @brief Start a read transaction to the specified slave device.
    ///
    /// Note: Initially the FIFO will be empty as no data will have been
    /// received yet, hence there is no facility to read data from the FIFO
    /// when starting a read transaction.
    ///
    /// @param[in] addrs  Slave address [0,127]. Note: values [0,7],[120,127]
    ///                   have special meanings in the I2C specification.
    /// @param[in] dlen   Number of bytes to be received from slave [0,65535].
    /// @throws std::out_of_range if the addrs or dlen parameters are not
    ///         in the allowed range of value.
    /// @throws std::logic_error if there is already a transaction in progress.
      void start_read
      ( std::uint32_t addrs
      , std::uint32_t dlen
      );

    /// @brief Start a read transaction following the write of a single byte 
    /// to the specified slave device.
    ///
    /// Provides support for read following write of a single byte using I2C
    /// repeated start. The most common requirement is for a single byte to
    /// be written to provide extended slave address information (I2C 10-bit
    /// addressing), a slave register value that is to be read or a command
    /// code that results in data to read.
    ///
    /// Note #1: It is not clear that the BCM2835 BSC masters support more
    /// general I2C repeated start transaction sequences.
    ///
    /// Note #2: Initially the FIFO will be empty as no data will have been
    /// received yet, hence there is no facility to read data from the FIFO
    /// when starting a read transaction.
    ///
    /// @param[in] addrs  Slave address [0,127]. Note: values [0,7],[120,127]
    ///                   have special meanings in the I2C specification.
    /// @param[in] desc   Single byte that will be written to slave immediately
    ///                   before setting up the BSC peripheral to read data.
    /// @param[in] dlen   Number of bytes to be received from slave [0,65535].
    /// @return true if repeated start write then read transition OK., false
    ///         if start of write transaction missed for some reason, in which
    ///         case read transaction not started and the request should be
    ///         resubmitted.
    /// @throws std::out_of_range if the addrs or dlen parameters are not
    ///         in the allowed range of value.
    /// @throws std::logic_error if there is already a transaction in progress.
      bool start_read
      ( std::uint32_t addrs
      , std::uint8_t desc
      , std::uint32_t dlen
      );

    /// @brief Read bytes received from the slave addressed in a currently
    /// active read operation from the FIFO into a buffer
    ///
    /// @param[out] data  Data buffer to receive read values
    /// @param[in] count  Maximum number of bytes to read
    /// @returns  Number of bytes actually read. Less than count if FIFO
    ///           empties. Zero if FIFO empty or not currently in an active
    ///           read transaction.
      std::size_t read
      ( std::uint8_t * pdata
      , std::size_t count
      );

    /// @brief Query whether the I2C/BSC peripheral is busy with data transfer
    /// @returns true if data transfer is ongoing (C:TA field is 1),
    ///          false if no data transfer is ongoing (C:TA field is 0)
      bool is_busy() const;

    /// @brief Query whether there is no data to write from the FIFO.
    ///
    /// @returns true if there is no data to write from the FIFO
    ///          false if the FIFO still contains data to write
      bool write_fifo_is_empty() const;

    /// @brief Query whether there is room for more data FIFO.
    ///
    /// Note:  When the FIFO is empty no further serial data will be
    ///         transmitted until some data is written to the FIFO.
    ///
    /// @returns true if there is space to write data into the FIFO
    ///          false if the FIFO is full.
      bool write_fifo_has_space() const;

    /// @brief Query whether the FIFO is approaching being empty while writing
    ///        data and needs more data written to it.
    ///
    /// Note: It is unknown if the 'nearing being empty' value is 25% full as
    ///       the inverse of the SPI0 device's read nearing full value of
    ///       75% as it is never mentioned in the documentation
    ///
    /// @returns  true if the FIFO is nearing being empty while writing.
    ///           false otherwise.
      bool write_fifo_needs_writing() const;

    /// @brief Query whether there is no room for read data in the FIFO.
    ///
    /// Note:  When the FIFO is full and cannot accept any more
    ///        incoming data no further serial data will be received until
    ///        some data is read out from the FIFO.
    ///
    /// @returns true if the receive FIFO is full.
    ///          false if the receive FIFO still has room for more read data
      bool read_fifo_is_full() const;

    /// @brief Query whether there is any data to be read from the FIFO.
    ///
    /// @returns  true if there is data available to read data from the
    ///           FIFO.
    ///           false if the FIFO is empty.
      bool read_fifo_has_data() const;

    /// @brief Query whether the FIFO is approaching being full while reading
    ///        data and needs reading.
    ///
    /// Note: It is unknown if the 'nearing being full' value is 75% full as
    ///       for the SPI0 peripheral as it is never mentioned in the
    ///       documentation
    ///
    /// @returns  true if the FIFO is nearing being full while reading.
    ///           false otherwise.
      bool read_fifo_needs_reading() const;

    /// @brief Return the current appropriate error state flags
    /// @returns i2c_pins::goodbit if there are no errors or one or both
    ///          i2c_pins::timeoutbit, i2c_pins::noackowledgebit.
      int error_state();

    /// @brief Clear all set error states
    /// @post good() is true.
      void clear();

    /// @brief Clear a specific error state
    /// @param statebit state enumeration indicating which error state to clear
    ///                 i2c_pins::timeoutbit or i2c_pins::noackowledgebit.
    ///                 i2c_pins::goodbit is ignored.
      void clear(state statebit);

    /// @brief Query whether communication state is good
    /// @returns true if there are no errors
      bool good() { return error_state()==goodbit; }

    /// @brief Query whether there has been any clock stretch time out errors.
    /// @returns true if a slave stretched the clock beyond the set time out
      bool clock_timeout() { return error_state()&timeoutbit; }

    /// @brief Query whether any slave has failed to acknowledge its address
    /// @returns true is a slave has failed to acknowledge its address.
      bool no_acknowledge() { return error_state()&noackowledgebit; }

    /// @brief Abort an in progress transaction.
    /// @post The FIFO is cleared
    /// @post Transfers are not active
    /// @post Transfers are done
      void abort();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_I2C_PINS_H
