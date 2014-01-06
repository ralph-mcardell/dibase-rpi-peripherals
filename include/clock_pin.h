// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin.h 
/// @brief Use a GPIO pin as a general purpose clock : class definition.
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
  ///
  /// General purpose clocks 0, 1 and 2 may be output to GPIO pins when set
  /// to the appropriate alternate pin function. Refer to the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// BCM2835 ARM Peripherals data sheet</a>, table 6-31 to see which pin/alt
  /// function combinations support a GPCLK function.
  /// A clock_pin instance is constructed with a GPIO pin_id which is used to
  /// determine which, if any, GPCLK function is supported and if so which
  /// pin alternative function to use. 
  /// If the pin supports a GPCLK function and is not in use, the source and
  /// frequency parameters are valid and the clock is not already in use
  /// locally within the same process then the clock is set-up and the pin
  /// allocated and set to the relevant alt-fn. Note: no attempt is made to
  /// see if the clock is in use externally by other processes.
  ///
  /// Once constructed the clock can be started and stopped.
    class clock_pin
    {
      unsigned  construct( pin_id pin
                         , hertz src_freq
                         , clock_source src_type
                         , clock_frequency const & freq
                         );

      hertz           freq_min;
      hertz           freq_avg;
      hertz           freq_max;
      unsigned const  clk;
      pin_id const    pin;

    public:
    /// @brief Construct from GPIO pin and clock source and frequency parameters
    ///
    /// Checks GPIO pin supports a clock function and converts it to a clock::id
    /// Calculates and sets clock frequency values and sets clock manager
    /// control and divisor fields for associated clock.
    /// Allocates GPIO pin.
    /// Note: clock is initially disabled - i.e. not running.
    /// @tparam Source Clock source type supporting \b frequency()
    ///                   member function returning source \ref frequency in
    ///                   Hertz (::hertz) and \b source() member function
    ///                    returning a #clock_source value.
    /// @param[in] p      Pin id of GPIO pin to use with clock
    /// @param[in] src    Instance of Source specifying clock source parameters
    /// @param[in] freq   Clock requested frequency characteristics
    /// @throws std::invalid_argument if the requested pin has no clock
    ///         function, the clock frequency is out of range for the
    ///         filtering mode requested or the source and requested
    ///         frequencies are not in the valid range of ratios:
    ///         1<=(requested frequency)/(source frequency)<=0xfff
    /// @throws std::range_error if the pin supports more than one clock
    ///         function or the special function type is not one of the GPCLK
    ///         values (neither of which should occur) or the clock divisor
    ///         DIVI field value is too small for the selected MASH mode.
    /// @throws bad_peripheral_alloc if either the pin or the clock related to
    ///         the pin are already in use.
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

    /// @brief Return running (enabled) state of clock
    /// @returns true if clock is running (enabled), false if not.
      bool is_running() const;

    /// @brief Return clock minimum frequency calculated during construction
    /// @returns Calculated clock minimum frequency in Hertz
      hertz frequency_min() const { return freq_min; }

    /// @brief Return clock average frequency calculated during construction
    /// @returns Calculated clock average frequency in Hertz
      hertz frequency_avg() const { return freq_avg; }

    /// @brief Return clock maximum frequency calculated during construction
    /// @returns Calculated clock maximum frequency in Hertz
      hertz frequency_max() const { return freq_max; }
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_PIN_H
