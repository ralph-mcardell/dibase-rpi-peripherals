// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_parameters.h 
/// @brief Library internal use clock_parameters class specification.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_CLOCK_PARAMETERS_H
# define DIBASE_RPI_PERIPHERALS_CLOCK_PARAMETERS_H
# include "clock.h"
# include "clock_registers.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
    /// @brief For library interal use. Check and determine clock parameters
    /// From clock source and clock requested frequency information check and
    /// determine various clock parameters in two groups: user frequency
    /// infromation (min,max, average) and clock peripheral register field
    /// values: (SRC, MASH,  DIVI, DIVF) required to setup a clock.
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
      /// @brief Create from user clock paraemter type values
      /// 
      /// Values are checked and any that are invalid cause an exception to
      ///  be thrown.
      ///
      /// The DIVI and DIVF divisor field values are calculated and the clock
      /// MASH filtering mode is determined using the clock_frequency::filter()
      /// value as a starting point. If the DIVI value would cause division
      /// by zero or a negative value or the maximum frequency is out of range
      /// the MASH mode is reduced down until a usable DIVI and maximim
      /// frequency are found. An exception is raised if none can be found or
      /// if the resultant DIVI value is less than that allowed for the
      /// selected MASH mode.
      ///
      /// @param src_type Library user layer clock source type value
      /// @param src_freq Clock source frequency
      /// @param freq     Requested clock frequency spacification
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
      /// @returns Clock control field source type value
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
 