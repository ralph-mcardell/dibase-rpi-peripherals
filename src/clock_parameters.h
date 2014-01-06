// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_parameters.h 
/// @brief \b Internal : Check and determine clock parameters : class definition
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCK_PARAMETERS_H
# define DIBASE_RPI_PERIPHERALS_CLOCK_PARAMETERS_H
# include "clockdefs.h"
# include "clock_registers.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
    /// @brief Check and determine clock frequency and register parameters
    ///
    /// From clock source and clock requested frequency information check and
    /// determine various clock parameters in two groups: user frequency
    /// information (min,max, average) and clock peripheral register field
    /// values: (SRC, MASH,  DIVI, DIVF) required to set-up a clock.
      class clock_parameters
      {
        hertz freq_min;         ///< Clock minimum frequency in Hz
        hertz freq_avg;         ///< Clock average frequency in Hz
        hertz freq_max;         ///< Clock maximum frequency in Hz

        clock_src       source; ///< Clock control register SRC field value
        clock_mash_mode mash;   ///< Clock control register MASH field value
        register_t      divi;   ///< Clock divisor register DIVI field value
        register_t      divf;   ///< Clock divisor register DIVF field value
        
      public:
      /// @brief Create from user clock parameter type values
      /// 
      /// Values are checked and any that are invalid cause an exception to
      ///  be thrown.
      ///
      /// The DIVI and DIVF divisor field values are calculated and the clock
      /// MASH filtering mode is determined using the clock_frequency::filter()
      /// value as a starting point. If the DIVI value would cause division
      /// by zero or a negative value or the maximum frequency is out of range
      /// the MASH mode is reduced down until a usable DIVI and maximum
      /// frequency are found. An exception is raised if none can be found or
      /// if the resultant DIVI value is less than that allowed for the
      /// selected MASH mode.
      ///
      /// For a filtering mode of ::clock_filter::none is specified the
      /// minimum, average and maximum frequencies will all be the same.
      /// Because only an integer divisor value (DIVI field) is used the
      /// calculated frequency may not be as close to the requested target
      /// frequency as an average frequency with filtering that employs a
      /// fractional divisor value (DIVF field) would be.
      ///
      /// For further details see the
      /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
      /// Broadcom BCM2835 ARM Peripherals Datasheet</a>, section 6.3 General
      /// Purpose GPIO Clocks. Note: in table 6-32 the values in the
      /// "min output freq" and "max output freq"  columns should be in each
      /// others.
      /// @param src_type Public clock oscillation source type value
      /// @param src_freq Clock oscillation source frequency
      /// @param freq     Requested clock target frequency specification
      /// @throws std::invalid_argument if the requested clock frequency is
      ///         out of range for the filtering mode requested or the source
      ///         and requested frequencies are not in the valid range of
      ///         ratios: 1<=(requested frequency)/(source frequency)<=0xfff
      /// @throws std::range_error if DIVI is too small for the selected MASH
      ///         mode.
        clock_parameters
        ( clock_source const & src_type
        , hertz src_freq
        , clock_frequency freq
        );

      /// @brief Return calculated clock minimum frequency
      /// @returns Calculated clock minimum frequency
        hertz           frequency_min() const { return freq_min; }

      /// @brief Return calculated clock average frequency
      /// @returns Calculated clock average frequency
        hertz           frequency_avg() const { return freq_avg; }

      /// @brief Return calculated clock maximum frequency
      /// @returns Calculated clock maximum frequency
        hertz           frequency_max() const { return freq_max; }


      /// @brief Return clock control field source type value
      /// @returns Internal clock control field source type value
        clock_src       clk_source() const { return source; }

      /// @brief Return clock control field MASH mode value
      /// @returns Clock control field MASH mode value
        clock_mash_mode clk_mash() const { return mash; }

      /// @brief Return clock divisor field DIVI value
      /// @returns Clock divisor field DIVI value
        register_t      clk_divi() const { return divi; }

      /// @brief Return clock divisor field DIVF value
      /// @returns Clock divisor field DIVF value
        register_t      clk_divf() const { return divf; }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_CLOCK_PARAMETERS_H
 