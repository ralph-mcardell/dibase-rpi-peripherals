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
    ///       causing ld to complain when storage (i.ew. definition) not found!
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
    /// @param[in] bsc_idx  BSC peripheral index number specifying BSC0 or BSC1
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
    ///         in range. 
    /// @throws bad_peripheral_alloc if either any of the pins or the BSC
    ///         peripheral are already in use.
      i2c_pins( pin_id        sda_pin
              , pin_id        scl_pin
              , int           bsc_idx   
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
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_I2C_PINS_H
