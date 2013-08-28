// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin.h 
/// @brief GPIO pin as clock classes' specifications.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCK_PIN_H
# define DIBASE_RPI_PERIPHERALS_CLOCK_PIN_H
# include "pin_id.h"
# include "clockdefs.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Use a GPIO pin as general purpose clock.
  /// General purpose clocks 0, 1 and 2 may be output to GPIO pins when set
  /// to the appropriate alternate pin function. See
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf"\>
  /// table 6-31 to see which pin/alt function combinations support a GPCLK
  /// function.
  /// A clock_pin instance is constructed with a GPIO pin_id which is used to
  /// determine which, if any, GPCLK function is supported and if so which
  /// pin alternative function to use. 
  /// If the pin supports a GPCLK function and the source and frequency
  /// parameters are valid and the clock not already in use locally within
  /// the same process then the clock is set-up and the pin allocated and
  /// set to the relevant alt-fn. Note: no attempt is made to see if the clock
  /// is in use externally by other processes.
  ///
  /// Once constructed the clock can be started and stopped.
    class clock_pin
    {
    /// @brief Helper. Non-templated helper to perform construction logic
    /// Checks pin has clock function, can be allocated, clock parameters are
    /// sane and clock not in use. Sets up clock, allocates and sets up pin alt
    /// function for clock.
    /// @param pin        Id of pin to use as clock
    /// @param src_freq   Clock source frequency in Hertz
    /// @param src_type   Clock source type
    /// @param freq       Clock requested frequency characteristics
    /// @throws std::invalid_argument if the requested pin has no clock
    ///         function, the clock frequency is out of range for the
    ///         filtering mode requested or the source and requested
    ///         frequencies are not in the valid range of ratios:
    ///         1<=(requested frequency)/(source frequency)<=0xfff
    /// @throws std::range_error if the pin supports more than one clock
    ///         function or the special function type is not one of the GPCLK
    ///         values (neither of which should occur) or the clock divisor
    ///         DIVI field value is too small for the selected MASH mode.
    /// @throws bad_peripheral_alloc if the clock related to the pin is already
    ///         being used within the same process (program).
      unsigned  construct( pin_id pin
                         , hertz src_freq
                         , clock_source src_type
                         , clock_frequency const & freq
                         );

      hertz           freq_min; ///< Minimum frequency of clock
      hertz           freq_avg; ///< Average frequency of clock
      hertz           freq_max; ///< Maximum frequency of clock
      unsigned const  clk;      ///< Value indicating pin's clock function
      pin_id const    pin;      ///< Id of GPIO pin clock signal output to

    public:
    /// @brief Construct from GPIO pin and clock source and frequency parameters
    /// Checks GPIO pin supports a clock function and converts it to a clock::id
    /// Calculates and sets clock frequency values and sets clock manager
    /// control and divisor fields for associated clock.
    /// Allocates GPIO pin.
    /// Note: clock is initially disabled - i.e. not running.
    /// @tparam Source Clock source type supporting frequency()
    ///                   member function returning source frequency in Hertz
    ///                   and source() member function returning a clock_source
    ///                   value.
    /// @param p          Pin id of GPIO pin to use with clock
    /// @param src        Instance of Source specifying clock source parameters
    /// @param freq       Clock requested frequency characteristics
    /// @throws std::invalid_argument if the requested pin has no clock
    ///         function, the clock frequency is out of range for the
    ///         filtering mode requested or the source and requested
    ///         frequencies are not in the valid range of ratios:
    ///         1<=(requested frequency)/(source frequency)<=0xfff
    /// @throws std::range_error if the pin supports more than one clock
    ///         function or the special function type is not one of the GPCLK
    ///         values (neither of which should occur) or the clock divisor
    ///         DIVI field value is too small for the selected MASH mode.
      template <class Source>
      clock_pin(pin_id p, Source src, clock_frequency const & freq)
      : clk{construct((p), src.frequency(), src.source(), freq)}
      , pin{p}
      {}

    /// @brief Destroy: stop the clock and de-allocate clock and GPIO pin.
      ~clock_pin();

      clock_pin(clock_pin const &) = delete;
      clock_pin& operator=(clock_pin const &) = delete;
      clock_pin(clock_pin &&) = delete;
      clock_pin& operator=(clock_pin &&) = delete;

    /// @brief Start clock running (enable clock)
      void start() const;

    /// @brief Stop clock running (disable clock)
      void stop() const;

    /// @brief Return enabled (running) state of clock
    /// @returns true if clock is running (enabled), false if not.
      bool is_running() const;

    /// @brief Return creation calculated clock minimum frequency
    /// @returns Calculated clock minimum frequency in Hertz
      hertz frequency_min() const { return freq_min; }

    /// @brief Return creation calculated clock average frequency
    /// @returns Calculated clock average frequency in Hertz
      hertz frequency_avg() const { return freq_avg; }

    /// @brief Return creation calculated clock maximum frequency
    /// @returns Calculated clock maximum frequency in Hertz
      hertz frequency_max() const { return freq_max; }
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_PIN_H
