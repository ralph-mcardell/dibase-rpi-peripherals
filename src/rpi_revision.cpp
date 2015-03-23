// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_revision.cpp
/// @brief Raspberry Pi revision value abstraction : class implementation
///
/// @copyright Copyright (c) Dibase Limited 2015
/// @author Ralph E. McArdell

#include "rpi_revision.h"
#include <limits>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

 namespace dibase {
  namespace rpi
  {
    namespace
    {
      template <typename TPACKED,unsigned LSB,unsigned BITLEN,typename TUNPACKED=TPACKED>
      struct bitfield
      {
        static_assert
        ( std::is_integral<TPACKED>::value
        , "Error: struct bitfield: expected TPACKED to be an integral type"
        );
        static_assert
        ( std::is_integral<TUNPACKED>::value
        , "Error: struct bitfield: expected TUNPACKED to be an integral type"
        );
        static_assert
        ( LSB<(sizeof(TPACKED)*std::numeric_limits<unsigned char>::digits)
        , "Error: struct bitfield: "
          "expected LSB to be less than the number of bits in TPACKED"
        );
        static_assert
        ( (std::is_same<TPACKED,bool>::value&&(LSB==0))||(!std::is_same<TPACKED,bool>::value)
        , "Error: struct bitfield: expected LSB to be 0 if TPACKED bool"
        );
        static_assert
        ( BITLEN+LSB<=(sizeof(TPACKED)*std::numeric_limits<unsigned char>::digits)
        , "Error: struct bitfield: expected BITLEN to be no more than LSB+1"
        );

        typedef TPACKED     packed_type;
        typedef TUNPACKED   unpacked_type;
        enum
        { lsb = LSB
        , digits = BITLEN
        , max = (1<<digits)-1
        };

        static auto pack(unpacked_type field, packed_type word) -> packed_type
        {
          return (word&(~(max<<lsb))) | ((field&max)<<lsb);
        }

        static auto unpack(packed_type word) -> unpacked_type
        {
          return (word>>lsb)&max;
        }
      };

      constexpr unsigned warrenty_bit{25};
      constexpr unsigned warrenty_bitlen{1};
      typedef bitfield<std::size_t
                      , warrenty_bit
                      , warrenty_bitlen
                      , bool>                               warranty_bitfield;

      constexpr unsigned turbo_bit{24};
      constexpr unsigned turbo_bitlen{1};
      typedef bitfield<std::size_t
                      , turbo_bit
                      , turbo_bitlen
                      , bool>                               turbo_bitfield;

      constexpr unsigned scheme_bit{23};
      constexpr unsigned scheme_bitlen{1};
      typedef bitfield<std::size_t
                      , scheme_bit
                      , scheme_bitlen
                      , bool>                               scheme_bitfield;

      constexpr unsigned ram_bit{20};
      constexpr unsigned ram_bitlen{3};
      typedef bitfield<std::size_t, ram_bit, ram_bitlen>    ram_bitfield;

      constexpr unsigned maker_bit{16};
      constexpr unsigned maker_bitlen{4};
      typedef bitfield<std::size_t,maker_bit,maker_bitlen>  maker_bitfield;

      constexpr unsigned processor_bit{12};
      constexpr unsigned processor_bitlen{4};
      typedef bitfield
              <std::size_t,processor_bit,processor_bitlen>  processor_bitfield;

      constexpr unsigned type_bit{4};
      constexpr unsigned type_bitlen{8};
      typedef bitfield<std::size_t,type_bit,type_bitlen>    type_bitfield;

      constexpr unsigned revision_bit{0};
      constexpr unsigned revision_bitlen{4};
      typedef bitfield
              <std::size_t, revision_bit, revision_bitlen>  revision_bitfield;


      static std::size_t convert_old_revision_to_new_scheme
      (std::size_t old_rev)
      {
        constexpr std::size_t highest_old_rev{0x12};
      
        static std::size_t invalid_old_revs[] = {0x0,0x1,0xa,0xb,0xc};
        constexpr std::size_t number_of_invalid_old_revs
                                {sizeof(invalid_old_revs)/sizeof(std::size_t)};

       std::size_t new_rev{0};
      // In the old revision scheme turbo=>warranty, so only the turbo bit
      // was set:
        if ( turbo_bitfield::unpack(old_rev) )
          { // in the new scheme turbo and warranty are different, and set both
            // for an old board with combined turbo/warranty bit set      
            new_rev = turbo_bitfield::pack(true, new_rev);
            new_rev = warranty_bitfield::pack(true, new_rev);
            old_rev &= (~(1<<turbo_bit)); // unset turbo bit on old revision value
          }
        auto invalid_old_revs_end(invalid_old_revs+number_of_invalid_old_revs);
        if ( (old_rev>highest_old_rev)
          || (std::find(invalid_old_revs, invalid_old_revs_end, old_rev)
                                                        !=invalid_old_revs_end)  
           )
          {
            throw std::runtime_error( "rpi_init::init_major_version: Unable to "
                                      "deduce board information from "
                                      "/proc/cpuinfo."
                                    );
          }
        std::size_t rev_map_index{old_rev-2}; // old revisions 0 & 1 not used
        if (old_rev>invalid_old_revs[number_of_invalid_old_revs-1])
          {
            rev_map_index -= 3; // revisions 0xa, 0xb and 0xc not used
          }
        static rpi_revision rev_conversion_map[] =
        { rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::unknown, 1 ) // rev==2
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::unknown, 1 ) // rev==3
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::sony,  2 )   // rev==4
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::qisda,  2 )  // rev==5
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::egoman,  2 ) // rev==6
        , rpi_revision( rpi_model::a, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::egoman,  2 ) // rev==7
        , rpi_revision( rpi_model::a, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::sony,  2 )   // rev==8
        , rpi_revision( rpi_model::a, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::qisda,  2 )  // rev==9
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb512, rpi_maker::egoman,  2 ) // rev==d
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb512, rpi_maker::sony,  2 )   // rev==e
        , rpi_revision( rpi_model::b, rpi_processor::bcm2835
                      , rpi_ram::mb512, rpi_maker::qisda,  2 )  // rev==f
        , rpi_revision( rpi_model::b_plus, rpi_processor::bcm2835
                      , rpi_ram::mb512, rpi_maker::sony,  1 )  // rev==10
        , rpi_revision( rpi_model::compute_module, rpi_processor::bcm2835
                      , rpi_ram::mb512, rpi_maker::sony,  1 )  // rev==11
        , rpi_revision( rpi_model::a_plus, rpi_processor::bcm2835
                      , rpi_ram::mb256, rpi_maker::sony,  1 )  // rev==12
        };
        new_rev |= rev_conversion_map[rev_map_index].raw_value();
        return new_rev;
      }
    }

    rpi_revision::rpi_revision(size_t rev)
    : revision
        ( scheme_bitfield::unpack(rev)  
            ? rev 
            : convert_old_revision_to_new_scheme(rev)
        )
    {
    }

    rpi_revision::rpi_revision
    ( rpi_model model
    , rpi_processor processor
    , rpi_ram ram
    , rpi_maker maker
    , unsigned int version // 0..15
    , bool turbo
    , bool warranty_void
    )
    : revision(version&0xf)
    {
      if (version>0xf)
        {
          throw std::invalid_argument
                  ( "rpi_revision::rpi_revision : the value of the "
                    "revision version field is too big, range is [0,15]."
                  );
        }
      revision = type_bitfield::pack(static_cast<std::size_t>(model), revision);
      revision = processor_bitfield::pack(static_cast<std::size_t>(processor), revision);
      revision = maker_bitfield::pack(static_cast<std::size_t>(maker), revision);
      revision = ram_bitfield::pack(static_cast<std::size_t>(ram), revision);
      revision = scheme_bitfield::pack(true, revision);  // new scheme format
      revision = turbo_bitfield::pack(turbo, revision);
      revision = warranty_bitfield::pack(warranty_void, revision);
    }

    rpi_model rpi_revision::model()
    {
      return static_cast<rpi_model>(type_bitfield::unpack(revision));
    }

    rpi_processor rpi_revision::processor()
    {
      return static_cast<rpi_processor>(processor_bitfield::unpack(revision));
    }

    rpi_ram rpi_revision::ram()
    {
      return static_cast<rpi_ram>(ram_bitfield::unpack(revision));
    }

    unsigned int rpi_revision::ram_MB()
    {
      switch (ram())
      {
        case rpi_ram::mb256: return 256;
        case rpi_ram::mb512: return 512;
        case rpi_ram::mb1024: return 1024;
        default: return 0;
      };
    }

    rpi_maker rpi_revision::maker()
    {
      return static_cast<rpi_maker>(maker_bitfield::unpack(revision));
    }

    unsigned int rpi_revision::version()
    {
      return revision_bitfield::unpack(revision);
    }

    bool rpi_revision::turbo()
    {
      return turbo_bitfield::unpack(revision);
    }

    bool rpi_revision::warranty_void()
    {
      return warranty_bitfield::unpack(revision);
    }
  }
}
 