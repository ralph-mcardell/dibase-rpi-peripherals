// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin.h 
/// @brief Use a GPIO pin for pulse width modulation (PWM): type definitions.
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
  /// @brief Very simple type to hold count of values of the type's ratio.
  ///
  /// The value represented by an instance \c i of specialisation \c s is:
  ///
  ///        i.count()*s::num / s::den
  ///
  /// @tparam CntIntT  Integer type used to hold count
  /// @tparam Rat      std::ratio specialisation type
    template<typename CntIntT, typename Rat>
    class pwm_ratio
    {
    public:
      typedef CntIntT count_type; ///< Type used to store count value

    private:
      count_type      cnt;

    public:
      static constexpr intmax_t num = Rat::num; ///< Ratio numerator
      static constexpr intmax_t den = Rat::den; ///< Ratio denominator

    /// @brief Construct from count value
    /// @param[in] c    Count value
      constexpr pwm_ratio(count_type c) : cnt{c} {}

    /// @brief Obtain the count value constructed with
    /// @returns The count value used to construct instance
      constexpr count_type count() const { return cnt; }
    };

    typedef pwm_ratio<unsigned, std::deci>  pwm_tenths;     ///< Quantities in 1/10ths
    typedef pwm_ratio<unsigned, std::centi> pwm_hundredths; ///< Quantities in 1/100ths
    typedef pwm_ratio<unsigned, std::milli> pwm_thousandths;///< Quantities in 1/1000ths
    typedef pwm_ratio<unsigned, std::micro> pwm_millionths; ///< Quantities in 1/1000000ths

  /// @brief Use a GPIO pin for pulse width modulation
  ///
  /// The output from PWM channels 1 and 2 may be output to GPIO pins as
  /// special functions PWM0 and PWM1 when set to the appropriate alternate
  /// pin function. Refer to the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// BCM2835 ARM Peripherals data sheet</a>, table 6-31 to see which pin/alt
  /// function combinations support the PWM0 and PWM1 special functions.
  ///
  /// A pwm_pin object is constructed with a GPIO pin_id which is used to
  /// determine which, if any, PWM function is supported and if so which
  /// pin alternative function to use. If the pin supports a PWM function
  /// and the PWM channel is not already in use locally within the same
  /// process then the PWM channel is set-up,  the pin allocated and set to
  /// the relevant alt-fn. Note that no attempt is made to see if the PWM
  /// channel is in use externally by other processes.
  ///
  /// Once constructed the PWM channel can be started and stopped and the 
  /// high-to-low output ratio modified.
  ///
  /// All (well, both) PWM channels share a common clock, which may be set to
  /// a specific clock source and output frequency when _no_ PWM channels are
  /// allocated (e. g. before any pwm_pin objects are created).
  ///
    class pwm_pin
    {
      static void do_set_clock
      ( hertz src_freq
      , clock_source src_type
      , clock_frequency const & freq
      );

      void set_data(unsigned data);

      static hertz    freq_min;
      static hertz    freq_avg;
      static hertz    freq_max;
      unsigned        pwm;
      pin_id const    pin;
      unsigned const  range;

    public:
      constexpr static unsigned range_default = 2400U;///< Default range
      constexpr static unsigned range_minimum = 2U;  ///< Smallest allowed range

    /// @brief Static class-operation: set common PWM clock source & frequency
    ///
    /// Calculates and sets clock frequency values and sets clock manager
    /// control and divisor fields for the common PWM clock.
    /// @tparam Source    Clock source type supporting \b frequency()
    ///                   member function returning source \ref frequency
    ///                   in Hertz (::hertz) and \b source()
    ///                   member function returning a #clock_source value.
    /// @param[in] src    Instance of Source specifying clock source parameters
    /// @param[in] freq   PWM clock requested frequency characteristics
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

    /// @brief Construct from GPIO pin_id and optional PWM range value.
    ///
    /// Checks GPIO pin supports a PWM function. 
    /// The optional range value, defaulting to pwm_pin::range_default,
    /// is used as the PWM clock cycle range over which the requested PWM
    /// high-to-low ratio is produced. The cycle repeats every range PWM
    /// clock cycles. Although not mentioned in the data sheet a minimum
    /// value of pwm_pin::range_minimum is imposed for clock_pin objects.
    ///
    /// a pwm_pin is created with the PWM channel disabled (not running) and
    /// with a high-to-low ratio of 0 (i.e. output is always low).
    /// @param[in] p      Id of GPIO pin to use for PWM
    /// @param[in] range  PWM range value: at least pwm_pin::range_minimum.
    /// @throws std::invalid_argument if the requested pin has no PWM function
    /// @throws std::out_of_range if \b range is less than pwm_pin::range_minimum.
    /// @throws std::range_error if the pin supports more than one PWM function
    ///         or the special function type is not PWM0 or PWM1 (neither of
    ///         which should be possible).
    /// @throws bad_peripheral_alloc if either the pin or the PWM channel
    ///         related to the pin are already in use.
      explicit pwm_pin(pin_id p, unsigned range=range_default);

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

    /// @brief Return running (enabled) state of PWM channel
    /// @returns true if PWM channel is running (enabled), false if not.
      bool is_running() const;

    /// @brief Set the PWM ratio of high to low output.
    /// @param[in] r  Ratio value in the range [0.0,1.0]
    /// @throws std::out_of_range if r is greater than one or less than zero
      void set_ratio(double r);

    /// @brief Set the PWM ratio of high to low output.
    ///
    /// Can be used like so:
    ///
    ///     a_pwm_pin.set_ratio(pwm_hundredths(10)); // o/p high 10% of the time
    ///
    /// @tparam    C  CntInT parameter of pwm_ratio class template
    /// @tparam    R  Rat parameter of pwm_ratio class template
    /// @param[in] r  Ratio value in the range [0,1]
    /// @throws std::out_of_range if r is greater than one or less than zero
      template<typename C, typename R>
      void set_ratio(pwm_ratio<C,R> r);
    };
    
    template<typename C, typename R>
    void pwm_pin::set_ratio(pwm_ratio<C,R> r)
    {
      auto count(pwm_ratio<C,R>::num*r.count());
      if ((count<0) || (count>pwm_ratio<C,R>::den))
        {
          throw std::out_of_range{"pwm_pin::set_ratio: r parameter value "
                                  "represents a negative ratio or ratio > 1"
                                 };
        }
      set_data((range*count+(pwm_ratio<C,R>::den/2))/pwm_ratio<C,R>::den);
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PWM_PIN_H
