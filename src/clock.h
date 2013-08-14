// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock.h 
/// @brief GPIO clock classes' specifications.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCK_H
# define DIBASE_RPI_PERIPHERALS_CLOCK_H
# include "pin_id.h"
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
  /// frequeny values provide explicit support for default construction,
  /// construction from a count value and comparisons using ==, !=, <, >, >=,
  /// <= providing the count represetnation and multiplier ratio are the same
  /// for the LHS & RHS operands. 
  ///
  /// Additionally a frequency_cast is provided to allow casting between
  ///frequency types of differing representation and multiplier ratios.
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

  /// @brief Convert on frequency type to another.
  /// Works in a similar fashon to std::chrono::duration_cast, and in fact
  /// uses std::chrono::duration_cast to do the leg work.
  ///
  /// @param (template) ToFreq      frequency type to cast to
  /// @param (template) Rep         Rep type of frequency type to cast from
  /// @param (template) Multiplier  Multiplier type of frequency to cast from
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
      typedef Rep         rep;          ///< Count reresentation type
      typedef Multiplier  multiplier;   ///< Multiplier std::ratio type
      
    private:
        rep rep_;     ///< Oscillation count

    public:
    /// @brief Default constructor
    /// Zero/default initialises count representation value.
      constexpr frequency() : rep_{} {}

    /// @brief Construct from  count value
    /// @param r  count of oscillations per second divided by Multiplier
      constexpr frequency(rep const & r) : rep_{r} {}

    /// @brief Returns the count of oscillations per 1/Multipler seconds
    /// @returns Count of oscillations per 1/Multipler seconds
      constexpr rep count() const {return rep_;}

    /// @brief Compare two frequncies of the SAME TYPE for equality
    /// @param rhs  Frequency of the SAME TYPE to compare this frequency to 
    /// @returns true of this frequency count equals rhs frequency count
      constexpr bool operator==(frequency const & rhs) const 
      {
        return rep_==rhs.rep_;
      }

    /// @brief Compare this frequncy being less than other of the SAME TYPE
    /// @param rhs  Frequency of the SAME TYPE to compare this frequency to 
    /// @returns true of this frequency count less than rhs frequency count
      constexpr bool operator<(frequency const & rhs) const 
      {
        return rep_<rhs.rep_;
      }
    };
    using std::rel_ops::operator!=;
    using std::rel_ops::operator>;
    using std::rel_ops::operator<=;
    using std::rel_ops::operator>=;

  /// @brief Alias for (unisgned) integer frequency type alias
  /// Count of 1==1Hz
  /// @param Rep          unsigned int (32-bit on Raspberry Pi)
  /// @param Multiplier   std::ratio<1> - i.e. 1:1
    typedef frequency<unsigned>             i_hertz;

  /// @brief Alias for (unisgned) integer frequency type alias
  /// Count of 1==1000Hz (ie. 1KHz)
  /// @param Rep          unsigned int (32-bit on Raspberry Pi)
  /// @param Multiplier   std::kilo - i.e. 1000:1
    typedef frequency<unsigned, std::kilo>  i_kilohertz;

  /// @brief Alias for (unisgned) integer frequency type alias
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

  /// @brief Strongly typed enumeration of clock sources supported by BCM2835.
  /// These values map on to the source values for a clock's SRC control
  /// register field.
    enum class clock_source
    { ground = 0      ///< Ground - no source clock (?)
    , oscillator = 1  ///< External oscillator (probably easiest to use)
    , testdebug0 = 2  ///< ?
    , testdebug1 = 3  ///< ?
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
    /// @param (template) Frequency  frequency type to construct from
    /// @param f  Frequency value for clock
      template <typename Freqency>
      constexpr explicit fixed_oscillator_clock_source(Freqency const & f)
      : freq{frequency_cast<hertz>(f)}
      {}

    /// @brief Observer. Result is the frequency value in Hertz
    /// @returns  Frequency in Hertz
      constexpr hertz frequency() const { return freq; }

    /// @brief Observer. Result indicating an external oscillator clock source
    /// @returns clock_source::oscillator.
      constexpr clock_source source() const { return clock_source::oscillator; }
    };

  /// @brief Clock frequency jitter reducing filtering values
  /// Jitter can be reduced by applying a number of MASH filtering stages to
  /// the fequency of a clock so that its average frequency is close to the
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
    { none      = 0  ///< No filter, use only integer divider value
    , minimum   = 1  ///< Least amount of filtering 
    , medium    = 2  ///< Median amount of filtering 
    , maximum   = 3  ///< Greatest amount of filtering 
    };

  /// @brief Clock frequency characteristics
  /// Instances specify the characteristics requirements for clocks frequency.
  /// This includes the required average frequency and severity of signal
  /// filter to apply, which affect the clock MASH filter settings.
  ///
  /// Note that the filtering value is used as a suggestion and maps to a MASH
  /// filtering mode starting point. A less severe mode may be used if the 
  /// frequency range would exceed the maximum permissable frequency.
    class clock_frequency
    {
      hertz         avg_freq;     ///< The average frequency, in Hertz
      clock_filter  filter_mode;  ///< Amount of filter to clock fequency

    public:
    /// @brief Construct from a frequency value and filter severity
    /// @param (template) Frequency  frequency type to construct from
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

    class clock
    {
    public:
      enum class id
      { gp0
      , gp1
      , gp2
      , pwm
      };

    private:
      hertz     freq_min;
      hertz     freq_avg;
      hertz     freq_max;
      id        clk_id;

      void initialise
      ( hertz src_freq
      , clock_source src_type
      , clock_frequency const & freq
      );

    public:
      template <class Source>
      clock(id id_,  Source src, clock_frequency const & freq)
      : clk_id{id_}
      {
        initialise(src.frequency(), src.source(), freq);
      }

      clock(clock const &) = delete;
      clock& operator=(clock const &) = delete;
      clock(clock &&) = delete;
      clock& operator=(clock &&) = delete;

      ~clock()
      {
        stop();
      }

      void  start() const;
      void  stop() const;
      bool is_running() const;

      hertz frequency_min() const { return freq_min; }
      hertz frequency_avg() const { return freq_avg; }
      hertz frequency_max() const { return freq_max; }
    };
 
    class clock_pin : public clock
    {
      static clock::id pin_id_to_clock_id(pin_id pid);
      pin_id const pin;

    public:
      template <class Source>
      clock_pin(pin_id p, Source src, clock_frequency const & freq)
      : clock(pin_id_to_clock_id(p), src, freq)
      , pin(p)
      {}

      ~clock_pin();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_H
