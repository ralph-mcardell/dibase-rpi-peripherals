// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_parameters.cpp
/// @brief Library internal use clock_parameters class implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_parameters.h"
#include <stdexcept>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
      static clock_src clock_source_to_clock_src( clock_source in )
      {
        return in==clock_source::oscillator ? clock_src::oscillator :
               in==clock_source::plla       ? clock_src::plla :
               in==clock_source::pllc       ? clock_src::pllc :
               in==clock_source::plld       ? clock_src::plld :
               in==clock_source::hdmi_aux   ? clock_src::hdmi_aux :
               in==clock_source::testdebug0 ? clock_src::testdebug0 :
               in==clock_source::testdebug1 ? clock_src::testdebug1 :
               in==clock_source::ground     ? clock_src::gnd : clock_src::gnd
               ;
      }

      clock_parameters::clock_parameters
      ( clock_source const & src_type
      , hertz src_freq
      , clock_frequency freq
      )
      : freq_max(25654321) // too large for jitter reducing filtering modes 
      , source{clock_source_to_clock_src(src_type)}
      {
        hertz const max_filter_freq{frequency_cast<hertz>(megahertz{25})};
        hertz const max_freq{frequency_cast<hertz>(megahertz{125})};
        
        if (freq.average_frequency()==0U)
          {
            throw std::invalid_argument
                  { "clock_parameters::clock_parameters: clock_frequency "
                    "average frequency of zero is invalid."
                  };
          }
        if (   freq.average_frequency()>max_freq 
           || (  freq.filter()!=clock_filter::none
              && freq.average_frequency()>max_filter_freq )
           )
          {
            throw std::invalid_argument
                  { "clock_parameters::clock_parameters: clock_frequency "
                    "average frequency too high, should be 25MHz or less"
                  };
          }
        register_t src_hz{src_freq.count()};
        register_t avg_hz{freq.average_frequency().count()};
        divi = src_hz/avg_hz;
        if (divi==0U)
          {
            throw std::invalid_argument
                  { "clock_parameters::clock_parameters: clock_source frequency"
                    " lower than requested clock_frequency average frequency."
                  };

          }
        if (divi>0xfffU)
          {
            throw std::invalid_argument
                  { "clock_parameters::clock_parameters: clock_source frequency"
                    " too high for requested clock_frequency average frequency."
                  };
          }
        register_t divi_rmdr{src_hz-divi*avg_hz};
        register_t rnd_coeff{avg_hz/2};
        divf = (divi_rmdr*1024ULL+rnd_coeff)/avg_hz;

        clock_filter filter{freq.filter()};
        bool more_to_try{true};
        do
        {
          switch (filter)
          {
          case clock_filter::none:
            more_to_try = false;
            if (divf>511U && divi<0xfffU)
              {
                ++divi;
              }
            divf = 0U;
            freq_avg = src_hz / divi;
            freq_min = freq_max = freq_avg;
            mash = clock_mash_mode::integer_division;
            if ( freq_max>max_freq )
              {
                throw std::range_error
                      { "clock_parameters::clock_parameters: clock frequency "
                        "exceeds 125MHz absolute maximum."
                      };
              }
            break;

          case clock_filter::minimum:
            freq_avg = src_hz*1024ULL / (divi*1024U+divf);
            freq_max = src_hz / divi;
            freq_min = src_hz / (divi+1);
            mash = clock_mash_mode::mash_1_stage;
            filter = clock_filter::none;
            break;

          case clock_filter::medium:
             if ( divi>1)
              {
                freq_avg = src_hz*1024ULL / (divi*1024U+divf);
                freq_max = src_hz / (divi-1);
                freq_min = src_hz / (divi+2);
                mash = clock_mash_mode::mash_2_stage;
              }
            filter = clock_filter::minimum;
            break;

          case clock_filter::maximum:
            if ( divi>3)
              {
                freq_avg = src_hz*1024ULL / (divi*1024U+divf);
                freq_max = src_hz / (divi-3);
                freq_min = src_hz / (divi+4);
                mash = clock_mash_mode::mash_3_stage;
              }
            filter = clock_filter::medium;
            break;
          };
        }
        while (freq_max>max_filter_freq && more_to_try);

        register_t const mash_3_stage_divi_min{5U};
        register_t const mash_2_stage_divi_min{3U};
        register_t const mash_1_stage_divi_min{2U};
        register_t const divi_min{1U};
        if (   divi<divi_min 
            || (mash==clock_mash_mode::mash_1_stage&&divi<mash_1_stage_divi_min)
            || (mash==clock_mash_mode::mash_2_stage&&divi<mash_2_stage_divi_min)
            || (mash==clock_mash_mode::mash_3_stage&&divi<mash_3_stage_divi_min)
           )
          {
            throw std::range_error
                    { "clock_parameters::clock_parameters: DIVI value too "
                      "low for selected MASH mode."
                    };
          }
      }
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
