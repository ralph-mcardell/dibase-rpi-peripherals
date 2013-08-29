// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clockdefs.h 
/// @brief GPIO pin as clock supporting types specifications.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCKDEFS_H
# define DIBASE_RPI_PERIPHERALS_CLOCKDEFS_H
# include <ratio>
# include <chrono>
# include <utility>

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Bare bones representation of frequency
  ///
  /// Allow frequencies to be represented as a count of oscillations per
  /// inverse period (just called the multiplier) represented as a std::ratio.
  ///
  /// Hence 10MHz could be represented by:
  ///  - a count of 10 with a multiplier ratio of std::mega
  ///  - a count of 10000 with a multiplier ratio of std::kilo
  ///  - a count of 10000000 with a multiplier ratio std::ratio<1>
  ///
  /// frequency values provide explicit support for default construction,
  /// construction from a count value and comparisons using ==, !=, <, >, >=,
  /// <= providing the count representation and multiplier ratio are the same
  /// for the LHS & RHS operands. 
  ///
  /// Additionally a frequency_cast is provided to allow casting between
  /// frequency types of differing representation and multiplier ratios.
  ///
  /// All else depends on compiler provided defaults and conversions.
  ///
  /// Note that a full implementation would probably look very much like an
  /// implementation of std::chrono::duration.
  ///
  /// @param Rep        Frequency count representation type
  /// @param Multiplier Frequency count multiplier ratio type
    template <typename Rep, typename Multiplier=std::ratio<1>>
    class frequency;

  /// @brief Convert one frequency type to another.
  /// Works in a similar fashion to std::chrono::duration_cast, and in fact
  /// uses std::chrono::duration_cast to do the leg work.
  ///
  /// @tparam ToFreq      frequency type to cast to
  /// @tparam Rep         Rep type of frequency type to cast from
  /// @tparam Multiplier  Multiplier type of frequency to cast from
  /// @param f          Frequency instance to cast from.
    template <class ToFreq, class Rep, class Multiplier>
    constexpr
    ToFreq frequency_cast(const frequency<Rep, Multiplier>& f)
    {
      return ToFreq(std::chrono::duration_cast
                    <std::chrono::duration
                      <typename ToFreq::rep, typename ToFreq::multiplier>
                    >(std::chrono::duration<Rep,Multiplier>(f.count())).count()
                   );
    }

    template <typename Rep, typename Multiplier>
    class frequency
    {
    public:
      typedef Rep         rep;          ///< Count representation type
      typedef Multiplier  multiplier;   ///< Multiplier std::ratio type
      typedef frequency<rep,multiplier> self; ///< self type
    private:
        rep rep_;     ///< Oscillation count

    public:
    /// @brief Default constructor
    /// Zero/default initialises count representation value.
      constexpr frequency() : rep_{} {}

    /// @brief Construct from  count value
    /// @param r  count of oscillations per second divided by Multiplier
      constexpr frequency(rep const & r) : rep_{r} {}


    /// @brief Construct from instance of other frequency type
    /// @tparam Rep2        Rep type of other frequency type
    /// @tparam Multiplier2 Multiplier type of other frequency
    /// @param f  Other frequency type instance
      template <typename Rep2, typename Multiplier2>
      constexpr frequency(frequency<Rep2,Multiplier2> const & f) 
      : rep_{frequency_cast<self>(f).count()} {}

    /// @brief Returns the count of oscillations per 1/Multiplier seconds
    /// @returns Count of oscillations per 1/Multiplier seconds
      constexpr rep count() const {return rep_;}

    /// @brief Compare two frequencies of the SAME TYPE for equality
    /// @param rhs  Frequency of the SAME TYPE to compare this frequency to 
    /// @returns true of this frequency count equals rhs frequency count
      constexpr bool operator==(frequency const & rhs) const 
      {
        return rep_==rhs.rep_;
      }

    /// @brief Compare this frequency being less than other of the SAME TYPE
    /// @param rhs  Frequency of the SAME TYPE to compare this frequency to 
    /// @returns true if this frequency count less than rhs frequency count
      constexpr bool operator<(frequency const & rhs) const 
      {
        return rep_<rhs.rep_;
      }
    };
    using std::rel_ops::operator!=;
    using std::rel_ops::operator>;
    using std::rel_ops::operator<=;
    using std::rel_ops::operator>=;

  /// @brief Alias for (unsigned) integer frequency type alias
  /// Count of 1==1Hz
  /// @param Rep          unsigned int (32-bit on Raspberry Pi)
  /// @param Multiplier   std::ratio<1> - i.e. 1:1
    typedef frequency<unsigned>             i_hertz;

  /// @brief Alias for (unsigned) integer frequency type alias
  /// Count of 1==1000Hz (ie. 1KHz)
  /// @param Rep          unsigned int (32-bit on Raspberry Pi)
  /// @param Multiplier   std::kilo - i.e. 1000:1
    typedef frequency<unsigned, std::kilo>  i_kilohertz;

  /// @brief Alias for (unsigned) integer frequency type alias
  /// Count of 1==1000000Hz (ie. 1MHz)
  /// @param Rep          unsigned int (32-bit on Raspberry Pi)
  /// @param Multiplier   std::mega - i.e. 1000000:1
    typedef frequency<unsigned, std::mega>  i_megahertz;

  /// @brief Alias for (double) floating point frequency type alias
  /// Count of 1.0==1000Hz (ie. 1KHz)
  /// @param Rep          double
  /// @param Multiplier   std::kilo - i.e. 1000:1
    typedef frequency<double, std::kilo>    f_kilohertz;

  /// @brief Alias for (double) floating point frequency type alias
  /// Count of 1.0==1000000Hz (ie. 1MHz)
  /// @param Rep          double
  /// @param Multiplier   std::mega - i.e. 1000000:1
    typedef frequency<double, std::mega>    f_megahertz;

  /// @brief Short alias integer hertz frequency type 
    typedef i_hertz                         hertz;

  /// @brief Short alias integer kilohertz frequency type 
    typedef i_kilohertz                     kilohertz;

  /// @brief Short alias integer megahertz frequency type 
    typedef i_megahertz                     megahertz;

  /// @brief Clock frequency jitter reducing filtering values
  /// Jitter can be reduced by applying a number of MASH filtering stages to
  /// the frequency of a clock so that its average frequency is close to the
  /// requested frequency but the frequency varies between a maximum and
  /// minimum value. There is a limit on the maximum clock frequency of 25Mhz
  /// for BCM2835 MASH filtered clocks.
  ///
  /// The enumeration values specify an amount of filtering to apply.
  ///
  /// Note also that for the none mode the selected frequency may not be as
  /// close to the requested frequency as for other modes because only an
  /// integer divisor of the source clock frequency is used as opposed to
  /// a combined integer and fractional value.
    enum class clock_filter
    { none      = 0  ///< No filter, use only integer divisor value
    , minimum   = 1  ///< Least amount of filtering 
    , medium    = 2  ///< Median amount of filtering 
    , maximum   = 3  ///< Greatest amount of filtering 
    };

  /// @brief Clock frequency characteristics
  /// Instances specify the characteristics requirements for clocks' frequency.
  /// This includes the required average frequency and severity of signal
  /// filter to apply, which affect the clock MASH filter settings.
  ///
  /// Note that the filtering value is used as a suggestion and maps to a MASH
  /// filtering mode starting point. A less severe mode may be used if the 
  /// frequency range would exceed the maximum permissible frequency.
    class clock_frequency
    {
      hertz         avg_freq;     ///< The average frequency, in Hertz
      clock_filter  filter_mode;  ///< Amount of filtering

    public:
    /// @brief Construct from a frequency value and filter severity
    /// @tparam Frequency  frequency type to construct from
    /// @param af Average frequency value for clock
    /// @param f  Filter mode value for clock. 
      template <typename Freqency>
      constexpr explicit clock_frequency
      ( Freqency const & af
      , clock_filter f=clock_filter::none
      )
      : avg_freq{frequency_cast<hertz>(af)}
      , filter_mode{f}
      {}

    /// @brief Observer. Result is the average frequency value in Hertz
    /// @returns  Average frequency in Hertz
      constexpr hertz average_frequency() const { return avg_freq; }

    /// @brief Observer. Result is the filter mode value
    /// @returns Requested frequency filter severity for clock.
      constexpr clock_filter filter() const { return filter_mode; }
    };

  /// @brief Strongly typed enumeration of clock sources supported by BCM2835.
  /// These values map on to the source values for a clock's SRC control
  /// register field.
    enum class clock_source
    { ground = 0      ///< Ground - no source clock (?)
    , oscillator = 1  ///< External oscillator (probably easiest to use)
    , testdebug0 = 2  ///< ? no information ?
    , testdebug1 = 3  ///< ? no information ?
    , plla = 4        ///< BCM2835 phase locked loop A
    , pllc = 5        ///< BCM2835 phase locked loop C
    , plld = 6        ///< BCM2835 phase locked loop D
    , hdmi_aux = 7    ///< HDMI auxiliary clock (?)
    };

  /// @brief Type providing a fixed frequency oscillator clock source
  /// Objects are immutable having a frequency as specified during construction.
    class fixed_oscillator_clock_source
    {
      hertz   freq; ///< The frequency, in Hertz

    public:
    /// @brief Construct from a frequency value.
    /// @tparam R Rep parameter for frequency type to construct from
    /// @tparam M Multiplier parameter for frequency type to
    ///                     construct from
    /// @param f  Frequency value for clock
      template <typename R, typename M>
      constexpr explicit fixed_oscillator_clock_source(frequency<R,M> const & f)
      : freq{f}
      {}

    /// @brief Observer. Result is the frequency value in Hertz
    /// @returns  Frequency in Hertz
      constexpr hertz frequency() const { return freq; }

    /// @brief Observer. Result indicating an external oscillator clock source
    /// @returns clock_source::oscillator.
      constexpr clock_source source() const { return clock_source::oscillator; }
    };

  /// @brief Raspberry Pi 19.2MHz fixed oscillator external clock source
    constexpr fixed_oscillator_clock_source rpi_oscillator{kilohertz{19200}};
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_CLOCKDEFS_H
