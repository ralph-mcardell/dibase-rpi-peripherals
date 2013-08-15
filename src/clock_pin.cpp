// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_pin.cpp
/// @brief GPIO clock classes' implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "clock_parameters.h"
#include "clock_ctrl.h"
#include "gpio_ctrl.h"
#include "gpio_alt_fn.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    
    using internal::clock_ctrl;
    namespace
    {
      constexpr std::size_t number_of_clocks{4U};
      constexpr unsigned gpclk0{0U};
      constexpr unsigned gpclk1{1U};
      constexpr unsigned gpclk2{2U};
      constexpr unsigned pwmclk{3U};

      static clock_id index_to_clock_id(unsigned i)
      {
        static clock_id clocks[number_of_clocks] = { gp0_clk_id, gp1_clk_id
                                                   , gp2_clk_id, pwm_clk_id
                                                   };
        return clocks[i];
      }

      class clock_allocator
      {
        unsigned allocated;

      public:
        clock_allocator() : allocated{0U} {}

        bool is_in_use(unsigned clock_idx)
        {
          return (clock_idx<number_of_clocks) && (allocated&(1<<clock_idx));
        }

        bool allocate(unsigned clock_idx)
        {
          if ( (clock_idx<number_of_clocks) && !is_in_use(clock_idx) )
            {
              allocated |= (1<<clock_idx);
              return true;
            }
          return false;
        }

        bool deallocate(unsigned clock_idx)
        {
          if ( is_in_use(clock_idx) )
            {
              allocated &= ~(1<<clock_idx);
              return true;
            }
          return false;
        }

      };

      static clock_allocator & the_clock_allocator()
      {
        static clock_allocator a;
        return a;
      }

      static void initialise_clock
      ( clock_id clk
      , internal::clock_parameters const & cp
      )
      {
        clock_ctrl::instance().regs->set_source(clk,cp.clk_source());
        clock_ctrl::instance().regs->set_mash(clk,cp.clk_mash());
        clock_ctrl::instance().regs->set_divi(clk,cp.clk_divi());
        clock_ctrl::instance().regs->set_divf(clk,cp.clk_divf());
      }
    }

    bool clock_pin::is_running() const
    {
      return clock_ctrl::instance().regs->is_busy(index_to_clock_id(clk));
    }

    void clock_pin::start() const
    {
      if (!is_running())
        {
          clock_ctrl::instance().regs->set_enable(index_to_clock_id(clk), true);
        }
    }

    void clock_pin::stop() const
    {
      if (is_running())
        {
          clock_ctrl::instance().regs->set_enable(index_to_clock_id(clk),false);
        }
    }

    using internal::gpio_ctrl;

    clock_pin::~clock_pin()
    {
      stop();
      gpio_ctrl::instance().alloc.deallocate(pin);
      the_clock_allocator().deallocate(clk);
    }

    unsigned clock_pin::construct
    ( pin_id pin
    , hertz src_freq
    , clock_source src_type
    , clock_frequency const & freq
    )
    { // Select alt function decsriptors for pin for GPCLKn special functions
      using pin_alt_fn::result_set;
      using pin_alt_fn::select;
      using pin_alt_fn::gpio_special_fn;
      auto clk_fn_info=result_set(select( pin
                                        , { gpio_special_fn::gpclk0
                                          , gpio_special_fn::gpclk1 
                                          , gpio_special_fn::gpclk2 
                                          }
                                        )
                                  );
      if (clk_fn_info.empty())
        {
          throw std::invalid_argument
                  {"clock_pin::clock_pin: Pin has no supported clock function"};
        }
      if (clk_fn_info.size()!=1)
        {
          throw std::range_error // NO pin has >1 clock function
                  {"clock_pin::clock_pin: Internal data error: more than one "
                   "pin alt function selected that supports a clock function."
                  };
        }
      auto clk_fn = clk_fn_info[0].special_fn();
      if ( clk_fn!=gpio_special_fn::gpclk0 
        && clk_fn!=gpio_special_fn::gpclk1 && clk_fn!=gpio_special_fn::gpclk2 )
        {
          throw std::range_error // Expect one of special fns passed to select!
                  {"clock_pin::clock_pin: Internal data error: Unexpected "
                   "pin alt-fn special function - does not represent a "
                   "GPCLKn special function."
                  };
        }
      gpio_ctrl::instance().alloc.allocate(pin);
      auto clk_idx( clk_fn==gpio_special_fn::gpclk0 ? gpclk0 :
                    clk_fn==gpio_special_fn::gpclk1 ? gpclk1 :
                                                      gpclk2 
                  );
      try
      {
        using internal::clock_parameters;
        clock_parameters cp(src_type, src_freq, freq); // CAN THROW!!!
        clock_id clk_id{index_to_clock_id(clk_idx)};
        if ( !the_clock_allocator().allocate(clk_idx) )
          {
            throw bad_pin_alloc( "initialise_clock: clock is already being "
                                 "used locally."
                               );
          }
        initialise_clock(clk_id, cp);
        freq_min = cp.frequency_min();
        freq_avg = cp.frequency_avg();
        freq_max = cp.frequency_max();
      }
      catch (...)
      {
        gpio_ctrl::instance().alloc.deallocate(pin);
        throw; 
      }
      gpio_ctrl::instance().regs->set_pin_function(pin,clk_fn_info[0].alt_fn());
      return clk_idx;
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
