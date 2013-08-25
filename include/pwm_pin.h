// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin.h 
/// @brief GPIO pin as PWM classes' specifications.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PWM_PIN_H
# define DIBASE_RPI_PERIPHERALS_PWM_PIN_H
# include "pin_id.h"
# include "clockdefs.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Very simple type to hold count of values of type's ratio
  /// @param CntIntT  Integer type used to hold count
  /// @param Rat      std::ratio specialisation type
    template<typename CntIntT, typename Rat>
    class pwm_ratio
    {
      typedef CntIntT count_type;
      count_type      cnt; ///< Count of values of type's ratio
    public:
      static constexpr intmax_t num = Rat::num; ///< Ratio numerator
      static constexpr intmax_t den = Rat::den; ///< Ratio denominator

    /// @brief Construct from count value
    /// @param c    Count value
      constexpr pwm_ratio(count_type c) : cnt{c} {}

    /// @brief Obtain the count value constructed with
    /// @returns The count value used to construct instance
      constexpr count_type count() const { return cnt;}
    };

    typedef pwm_ratio<unsigned, std::deci>       pwm_tenths;
    typedef pwm_ratio<unsigned, std::centi>      pwm_hundredths;
    typedef pwm_ratio<unsigned, std::milli>      pwm_thousandths;
    typedef pwm_ratio<unsigned, std::micro>      pwm_millionths;

  /// @brief Use a GPIO pin for pulse width modulation
  /// PWM channels 1 and 2 PWM signal may be output to GPIO pins as function 
  /// pwm0 and pwm1 when set to the appropriate alternate pin function. See
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// table 6-31 to see which pin/alt function combinations support the PWM0
  /// PWM1 functions.
  ///
  /// A pwm_pin object is constructed with a GPIO pin_id which is used to
  /// determine which, if any, PWM function is supported and if so which
  /// pin alternative function to use. 
  /// If the pin supports a PWM function and the PWM channel is not already in
  ///  use locally within the same process then the PWM channel is set-up,
  ///  the pin allocated and set to the relevant alt-fn.
  /// Note: no attempt is made to see if the clock is in use externally by
  ///  other processes.
  ///
  /// Once constructed the PWM channel can be started and stopped and the 
  /// high/low output ratio modified.
  ///
  /// All (well, both) PWM channels share a common clock, which may be set to
  /// a specific clock source and output frequency when _no_ PWM channels are
  /// allocated (e. g. before any pwm_pin objects are created)
  ///
    class pwm_pin
    {
    /// @brief static class-operation: set common PWM clock source & frequency
    /// Private, non template member static function that set_clock hands off to
    /// @param src_freq   Clock source frequency in Hertz
    /// @param src_type   Clock source type
    /// @param freq       Clock requested frequency characteristics
    /// @throws std::invalid_argument if the clock frequency is out of range
    ///         for the filtering mode requested or the source and requested
    ///         frequencies are not in the valid range of ratios:
    ///         1<=(requested frequency)/(source frequency)<=0xfff
    /// @throws std::range_error if the clock divisor DIVI field value is
    ///         too small for the selected MASH mode.
    /// @throws peripheral_in_use if any PWM channel is in use (as reported
    ///         by the alloc allocator member) at the time of the call.
      static void do_set_clock
      ( hertz src_freq
      , clock_source src_type
      , clock_frequency const & freq
      );

      static hertz    freq_min; ///< Minimum frequency of PWM clock
      static hertz    freq_avg; ///< Average frequency of PWM clock
      static hertz    freq_max; ///< Maximum frequency of PWM clock
      unsigned const  pwm;      ///< Value indicating pin's PWM function
      pin_id const    pin;      ///< Id of GPIO pin clock signal output to
      unsigned const  range;    ///< PWM channel range value

    public:
    /// @brief static class-operation: set common PWM clock source & frequency
    /// Calculates and sets clock frequency values and sets clock manager
    /// control and divisor fields for the common PWM clock.
    /// @param (template) Source Clock source type supporting frequency()
    ///                   member function returning source frequency in Hertz
    ///                   and source() member function returning a clock_source
    ///                   value.
    /// @param src        Instance of Source specifying clock source parameters
    /// @param freq       PWM clock requested frequency characteristics
    /// @throws std::invalid_argument if the clock frequency is out of range
    ///         for the filtering mode requested or the source and requested
    ///         frequencies are not in the valid range of ratios:
    ///         1<=(requested frequency)/(source frequency)<=0xfff
    /// @throws std::range_error if the clock divisor DIVI field value is too
    ///         small for the selected MASH mode.
    /// @throws peripheral_in_use if any PWM channel is in use (that is any
    ///         pwm_pin objects exist) at the time of the call.
      template <class Source>
      static void set_clock(Source src, clock_frequency const & freq)
      {
        do_set_clock(src.frequency(), src.source(), freq);
      }

    /// @brief Return current calculated PWM clock minimum frequency
    /// @returns Calculated clock minimum frequency in Hertz
      static hertz clock_frequency_min() { return freq_min; }

    /// @brief Return current calculated PWM clock average frequency
    /// @returns Calculated clock average frequency in Hertz
      static hertz clock_frequency_avg() { return freq_avg; }

    /// @brief Return current calculated PWM clock maximum frequency
    /// @returns Calculated clock maximum frequency in Hertz
      static hertz clock_frequency_max() { return freq_max; }

    /// @brief construct from GPIO pin id and optional PWM range value.
    /// Checks GPIO pin supports a PWM function. 
    /// The optional range value, defaulting to 2400, is used as the PWM clock
    /// cycle range over which the requested PWM high-to-low ratio is
    /// produced. The cycle repeats every range PWM clock cycles. Although not
    /// mentioned in the data sheet a minimum value of 2 is imposed for
    /// clock_pin objects, being the lowest value giving any degree of control.
    /// Note: PWM channel is created disabled (not running) and with a high/low
    /// ratio of 0 (i.e. always low).
    /// @param p          Pin id of GPIO pin to use for PWM
    /// @param range      PWM range value. Should be at least 2.
    /// @throws std::invalid_argument if the requested pin has no PWM function
    /// @throws std::out_of_range if range is less than 2.
    /// @throws std::range_error if the pin supports more than one PWM function
    ///         or the special function type is not PWM0 or PWM1 (neither of
    ///         which should be possible).
      explicit pwm_pin(pin_id p, unsigned range=2400);

    /// @brief Destroy: stop the PWM channel and de-allocate channel & GPIO pin.
      ~pwm_pin();

      pwm_pin(pwm_pin const &) = delete;
      pwm_pin& operator=(pwm_pin const &) = delete;
      pwm_pin(pwm_pin &&) = delete;
      pwm_pin& operator=(pwm_pin &&) = delete;

    /// @brief Start PWM channel running (enable channel)
      void start() const;

    /// @brief Stop PWM channel running (disable channel)
      void stop() const;

    /// @brief Return enabled (running) state of PWM channel
    /// @returns true if PWM channel is running (enabled), false if not.
      bool is_running() const;

    /// @brief Set the PWM ratio of high to low output.
    /// @param r  Ratio value in the range [0.0,1.0]
    /// @throws std::out_of_range if r is greater than one or less than zero
      void set_ratio(double r);

    /// @brief Set the PWM ratio of high to low output.
    /// Can be used like so: 
    ///   set_ratio(pwm_hundredths(10))
    /// @param (template) C  CntInT parameter of pwm_ratio class template
    /// @param (template) R  Rat parameter of pwm_ratio class template
    /// @param r  Ratio value in the range [0,1]
    /// @throws std::out_of_range if r is greater than one or less than zero
      template<typename C, typename R>
      void set_ratio(pwm_ratio<C,R> r);
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PWM_PIN_H
