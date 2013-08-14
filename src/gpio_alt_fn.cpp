// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_alt_fn.cpp 
/// @brief GPIO pin alternative special function support implementation.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
/// I/O (GPIO) for details.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "gpio_alt_fn.h"
#include <algorithm>

namespace dibase { namespace rpi {
  namespace peripherals
  { 
    namespace
    {
      constexpr std::size_t number_of_alt_fns_per_pin{6};
      constexpr std::size_t number_of_gpio_pins{pin_id::max_id-pin_id::min_id+1};

    // gpio_alt_fn_table entries are taken from table 6-31, pp 102,103 of the 
    // BCM2835 ARM Peripherals datasheet PDF.
      static gpio_special_fn const 
      gpio_alt_fn_table[number_of_gpio_pins][number_of_alt_fns_per_pin]
/*00*/{ {gpio_special_fn::sda0          , gpio_special_fn::sa5           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*01*/, {gpio_special_fn::scl0          , gpio_special_fn::sa4           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*02*/, {gpio_special_fn::sda1          , gpio_special_fn::sa3           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*03*/, {gpio_special_fn::scl1          , gpio_special_fn::sa2           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*04*/, {gpio_special_fn::gpclk0        , gpio_special_fn::sa1           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::arm_tdi       }  
/*05*/, {gpio_special_fn::gpclk1        , gpio_special_fn::sa0           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::arm_tdo       }  
/*06*/, {gpio_special_fn::gpclk2        , gpio_special_fn::soe_n_se      , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::arm_rtck      }  
/*07*/, {gpio_special_fn::spi0_ce1_n    , gpio_special_fn::swe_n_srw_n   , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*08*/, {gpio_special_fn::spi0_ce0_n    , gpio_special_fn::sd0           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*09*/, {gpio_special_fn::spi0_miso     , gpio_special_fn::sd1           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*10*/, {gpio_special_fn::spi0_mosi     , gpio_special_fn::sd2           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*11*/, {gpio_special_fn::spi0_sclk     , gpio_special_fn::sd3           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*12*/, {gpio_special_fn::pwm0          , gpio_special_fn::sd4           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::arm_tms       }  
/*13*/, {gpio_special_fn::pwm1          , gpio_special_fn::sd5           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::arm_tck       }  
/*14*/, {gpio_special_fn::txd0          , gpio_special_fn::sd6           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::txd1          }  
/*15*/, {gpio_special_fn::rxd0          , gpio_special_fn::sd7           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::rxd1          }  
/*16*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd8           , gpio_special_fn::no_fn         ,
         gpio_special_fn::cts0          , gpio_special_fn::spi1_ce2_n    , gpio_special_fn::cts1          }  
/*17*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd9           , gpio_special_fn::no_fn         ,
         gpio_special_fn::rts0          , gpio_special_fn::spi1_ce1_n    , gpio_special_fn::rts1          }  
/*18*/, {gpio_special_fn::pcm_clk       , gpio_special_fn::sd10          , gpio_special_fn::no_fn         ,
         gpio_special_fn::bscsl_sda_mosi, gpio_special_fn::spi1_ce0_n    , gpio_special_fn::pwm0          }  
/*19*/, {gpio_special_fn::pcm_fs        , gpio_special_fn::sd11          , gpio_special_fn::no_fn         ,
         gpio_special_fn::bscsl_scl_sclk, gpio_special_fn::spi1_miso     , gpio_special_fn::pwm1          }  
/*20*/, {gpio_special_fn::pcm_din       , gpio_special_fn::sd12          , gpio_special_fn::no_fn         ,
         gpio_special_fn::bscsl_miso    , gpio_special_fn::spi1_mosi     , gpio_special_fn::gpclk0        }  
/*21*/, {gpio_special_fn::pcm_dout      , gpio_special_fn::sd13          , gpio_special_fn::no_fn         ,
         gpio_special_fn::bscsl_ce_n    , gpio_special_fn::spi1_sclk     , gpio_special_fn::gpclk1        }  
/*22*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd14          , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_clk       , gpio_special_fn::arm_trst      , gpio_special_fn::no_fn         }  
/*23*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd15          , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_cmd       , gpio_special_fn::arm_rtck      , gpio_special_fn::no_fn         }  
/*24*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd16          , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_dat0      , gpio_special_fn::arm_tdo       , gpio_special_fn::no_fn         }  
/*25*/, {gpio_special_fn::no_fn         , gpio_special_fn::sd17          , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_dat1      , gpio_special_fn::arm_tck       , gpio_special_fn::no_fn         }  
/*26*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_dat2      , gpio_special_fn::arm_tdi       , gpio_special_fn::no_fn         }  
/*27*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::sd1_dat3      , gpio_special_fn::arm_tms       , gpio_special_fn::no_fn         }  
/*28*/, {gpio_special_fn::sda0          , gpio_special_fn::sa5           , gpio_special_fn::pcm_clk       ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*29*/, {gpio_special_fn::scl0          , gpio_special_fn::sa4           , gpio_special_fn::pcm_fs        ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*30*/, {gpio_special_fn::no_fn         , gpio_special_fn::sa3           , gpio_special_fn::pcm_din       ,
         gpio_special_fn::cts0          , gpio_special_fn::no_fn         , gpio_special_fn::cts1          }  
/*31*/, {gpio_special_fn::no_fn         , gpio_special_fn::sa2           , gpio_special_fn::pcm_dout      ,
         gpio_special_fn::rts0          , gpio_special_fn::no_fn         , gpio_special_fn::rts1          }  
/*32*/, {gpio_special_fn::gpclk0        , gpio_special_fn::sa1           , gpio_special_fn::no_fn         ,
         gpio_special_fn::txd0          , gpio_special_fn::no_fn         , gpio_special_fn::txd1          }  
/*33*/, {gpio_special_fn::no_fn         , gpio_special_fn::sa0           , gpio_special_fn::no_fn         ,
         gpio_special_fn::rxd0          , gpio_special_fn::no_fn         , gpio_special_fn::rxd1          }  
/*34*/, {gpio_special_fn::gpclk0        , gpio_special_fn::soe_n_se      , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*35*/, {gpio_special_fn::spi0_ce1_n    , gpio_special_fn::swe_n_srw_n   , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*36*/, {gpio_special_fn::spi0_ce0_n    , gpio_special_fn::sd0           , gpio_special_fn::txd0          ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*37*/, {gpio_special_fn::spi0_miso     , gpio_special_fn::sd1           , gpio_special_fn::rxd0          ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*38*/, {gpio_special_fn::spi0_mosi     , gpio_special_fn::sd2           , gpio_special_fn::rts0          ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*39*/, {gpio_special_fn::spi0_sclk     , gpio_special_fn::sd3           , gpio_special_fn::cts0          ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*40*/, {gpio_special_fn::pwm0          , gpio_special_fn::sd4           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_miso     , gpio_special_fn::txd1          }  
/*41*/, {gpio_special_fn::pwm1          , gpio_special_fn::sd5           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_mosi     , gpio_special_fn::rxd1          }  
/*42*/, {gpio_special_fn::gpclk1        , gpio_special_fn::sd6           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_sclk     , gpio_special_fn::rts1          }  
/*43*/, {gpio_special_fn::gpclk2        , gpio_special_fn::sd7           , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_ce0_n    , gpio_special_fn::cts1          }  
/*44*/, {gpio_special_fn::gpclk1        , gpio_special_fn::sda0          , gpio_special_fn::sda1          ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_ce1_n    , gpio_special_fn::no_fn         }  
/*45*/, {gpio_special_fn::pwm1          , gpio_special_fn::scl0          , gpio_special_fn::scl1          ,
         gpio_special_fn::no_fn         , gpio_special_fn::spi2_ce2_n    , gpio_special_fn::no_fn         }  
/*46*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*47*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*48*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*49*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*50*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*51*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*52*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
/*53*/, {gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         ,
         gpio_special_fn::no_fn         , gpio_special_fn::no_fn         , gpio_special_fn::no_fn         }  
      };
 
      static gpio_pin_fn const idx_to_alt_fn[] =
                                { gpio_pin_fn::alt0, gpio_pin_fn::alt1
                                , gpio_pin_fn::alt2, gpio_pin_fn::alt3
                                , gpio_pin_fn::alt4, gpio_pin_fn::alt5
                                };

      template <class PinSeqT, class Predicate>
      pin_alt_fn_result_set
      make_results(PinSeqT pin_seq, Predicate add_pred)
      {
        pin_alt_fn_result_set_builder results;
        for (auto p : pin_seq)
          {
            for (std::size_t fn_idx{0U};fn_idx!=number_of_alt_fns_per_pin;++fn_idx)
              {
                gpio_special_fn specl_fn{gpio_alt_fn_table[p][fn_idx]};
                if ( add_pred(specl_fn) )
                  {
                    results.emplace_add(p, idx_to_alt_fn[fn_idx], specl_fn);
                  }
              }
          }
        return pin_alt_fn_result_set{results};
      }

      class pin_id_range_iterator
      {
        pin_id        value;
        pin_id_int_t  end;
        bool          is_end;
      public:
        pin_id_range_iterator() 
        : value{pin_id::min_id}, end{pin_id::max_id}, is_end{true}
        {}
        pin_id_range_iterator(pin_id first, pin_id last=pin_id{pin_id::max_id})
        : value{first}, end{last}, is_end{false}
        {}
        pin_id & operator*()  { return value; }
        pin_id*  operator->() { return &value;}
        pin_id_range_iterator & operator++()
        {
          is_end = value==end;
          if (!is_end) value = pin_id{value+1};
          return *this;
        }
        pin_id_range_iterator operator++(int)
        {
          auto tmp(*this);
          ++*this;
          return tmp;
        }

        bool operator==(pin_id_range_iterator const & rhs)
        {
          return is_end==rhs.is_end && (is_end==true || value==rhs.value); 
        }
        bool operator!=(pin_id_range_iterator const & rhs)
        {
          return !(*this==rhs);
        }
      };
      
      class pin_id_range
      {
        pin_id first;
        pin_id last;
      public:
        explicit pin_id_range
        ( pin_id f=pin_id{pin_id::min_id}
        , pin_id l=pin_id{pin_id::max_id}
        )
        : first{f}, last{l} {}

        pin_id_range_iterator begin() const
        {
          return pin_id_range_iterator{first, last};
        }
        pin_id_range_iterator end() const { return pin_id_range_iterator{}; }
      };
    }
 
    pin_alt_fn_result_set pin_alt_fn_select(select_options opt)
    { return  make_results( pin_id_range{}
                          , [opt](gpio_special_fn spl_fn)
                            { return opt==select_options::include_no_fn 
                                  || spl_fn!=gpio_special_fn::no_fn;
                            }
                          );
    } 

    pin_alt_fn_result_set pin_alt_fn_select(pin_id p, select_options opt)
    { return  make_results( pin_id_range{p,p}
                          , [opt](gpio_special_fn spl_fn)
                            { return opt==select_options::include_no_fn 
                                  || spl_fn!=gpio_special_fn::no_fn;
                            }
                          );
    } 

    pin_alt_fn_result_set pin_alt_fn_select(gpio_special_fn s)
    { return  make_results( pin_id_range{}
                          , [s](gpio_special_fn spl_fn){return spl_fn==s;}
                          );
    } 

    pin_alt_fn_result_set pin_alt_fn_select(pin_id p, gpio_special_fn s)
    { return  make_results( pin_id_range{p,p}
                          , [s](gpio_special_fn spl_fn){return spl_fn==s;}
                          );
    } 

    pin_alt_fn_result_set pin_alt_fn_select
    ( std::initializer_list<pin_id> ps
    , select_options opt
    )
    { return  make_results( ps
                          , [opt](gpio_special_fn spl_fn)
                            { return opt==select_options::include_no_fn 
                                  || spl_fn!=gpio_special_fn::no_fn;
                            }
                          );
    } 

    pin_alt_fn_result_set pin_alt_fn_select
    ( std::initializer_list<gpio_special_fn> ss
    )
    { return  make_results
              ( pin_id_range{}
              , [ss](gpio_special_fn spl_fn)
                { return std::find(ss.begin(),ss.end(),spl_fn)!=ss.end();
                }
              );
    } 

    pin_alt_fn_result_set pin_alt_fn_select
    ( pin_id p
    , std::initializer_list<gpio_special_fn> ss
    )
    { return  make_results
              ( pin_id_range{p,p}
              , [ss](gpio_special_fn spl_fn)
                { return std::find(ss.begin(),ss.end(),spl_fn)!=ss.end();
                }
              );
    } 

    pin_alt_fn_result_set pin_alt_fn_select
    ( std::initializer_list<pin_id> ps
    , gpio_special_fn s
    )
    { return make_results(ps, [s](gpio_special_fn spl_fn){return spl_fn==s;}); } 

    pin_alt_fn_result_set pin_alt_fn_select
    ( std::initializer_list<pin_id> ps
    , std::initializer_list<gpio_special_fn> ss
    )
    { return  make_results
                ( ps
                , [ss](gpio_special_fn spl_fn)
                  {return std::find(ss.begin(),ss.end(),spl_fn)!=ss.end();}
                );
    } 
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
