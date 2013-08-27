// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin.cpp 
/// @brief Internal PWM control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pwm_pin.h"
#include "pwm_ctrl.h"
#include "gpio_ctrl.h"
#include "gpio_alt_fn.h"
#include "clock_parameters.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { 
    namespace
    {
    // See http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
    // Section 9.1 Pulse Width Modulator Overview, final bullet point
      constexpr hertz default_pwm_clock_frequency(megahertz(100U));
    }
    hertz pwm_pin::freq_min{default_pwm_clock_frequency};
    hertz pwm_pin::freq_avg{default_pwm_clock_frequency};
    hertz pwm_pin::freq_max{default_pwm_clock_frequency};

    using namespace internal;
    void pwm_pin::do_set_clock
    ( hertz src_freq
    , clock_source src_type
    , clock_frequency const & freq
    )
    {
      clock_parameters cp(src_type, src_freq, freq); // CAN THROW!!!
      pwm_ctrl::instance().set_clock(cp); // CAN THROW!!!
      freq_min = cp.frequency_min();
      freq_avg = cp.frequency_avg();
      freq_max = cp.frequency_max();
    }

    void pwm_pin::start() const
    {
      pwm_channel pwm_ch{static_cast<pwm_channel>(pwm)};
      pwm_ctrl::instance().regs->set_enable(pwm_ch, true);
    }

    void pwm_pin::stop() const
    {
      pwm_channel pwm_ch{static_cast<pwm_channel>(pwm)};
      pwm_ctrl::instance().regs->set_enable(pwm_ch, false);
    }

    bool pwm_pin::is_running() const
    {
      pwm_channel pwm_ch{static_cast<pwm_channel>(pwm)};
      return pwm_ctrl::instance().regs->get_enable(pwm_ch);
    }

    void pwm_pin::set_ratio(double r)
    {
      if (r<0.0 || r>1.0)
        {
          throw std::out_of_range{"pwm_pin::set_ratio: r parameter value is "
                                  "outside the range [0.0, 1.0]."
                                 };
        }
      set_data(range*r+0.5);
    }

    void pwm_pin::set_data(unsigned data)
    {
      pwm_channel pwm_ch{static_cast<pwm_channel>(pwm)};
      pwm_ctrl::instance().regs->set_data(pwm_ch, data);  
    }

    pwm_pin::~pwm_pin()
    {
      stop();
      pwm_ctrl::instance().alloc.deallocate(pwm);
      gpio_ctrl::instance().alloc.deallocate(pin);
    }

    pwm_pin::pwm_pin(pin_id p, unsigned range)
    : pin(p)
    , range(range)
    {
      if (range<range_minimum)
      {
        throw std::out_of_range{"pwm_pin::pwm_pin: range parameter value is "
                                "too small."
                               };
      }
    // Select alt function descriptors for pin for PWM0|1 special functions
      using pin_alt_fn::result_set;
      using pin_alt_fn::select;
      using pin_alt_fn::gpio_special_fn;
      auto pwm_fn_info=result_set(select( pin
                                        , { gpio_special_fn::pwm0
                                          , gpio_special_fn::pwm1 
                                          }
                                        )
                                  );
      if (pwm_fn_info.empty())
        {
          throw std::invalid_argument
                  {"pwm_pin::pwm_pin: Pin has no supported PWM function"};
        }
      if (pwm_fn_info.size()!=1)
        {
          throw std::range_error // NO pin has >1 PWM function
                  {"pwm_pin::pwm_pin: Internal data error: more than one "
                   "pin alt function selected that supports a PWM function."
                  };
        }
      gpio_ctrl::instance().alloc.allocate(pin); // CAN THROW
      auto pwm_ch(  pwm_fn_info[0].special_fn()==gpio_special_fn::pwm0
                                                ? pwm_channel::gpio_pwm0 :
                                                  pwm_channel::gpio_pwm1
                 );
      this->pwm = static_cast<unsigned>(pwm_ch);
      try
      {
        if ( !pwm_ctrl::instance().alloc.allocate(pwm) )
          {
            throw bad_peripheral_alloc( "pwm_pin::pwm_pin: PWM channel "
                                        "is already being used locally."
                                      );
          }
        pwm_ctrl::instance().regs->set_enable(pwm_ch, false);
        pwm_ctrl::instance().regs->set_mode(pwm_ch, pwm_mode::pwm);
        pwm_ctrl::instance().regs->set_ms_enabled(pwm_ch, false);
        pwm_ctrl::instance().regs->set_repeat_last_data(pwm_ch, false);
        pwm_ctrl::instance().regs->set_silence(pwm_ch, false);
        pwm_ctrl::instance().regs->set_polarity_inverted(pwm_ch, false);
        pwm_ctrl::instance().regs->set_use_fifo(pwm_ch, false);
        pwm_ctrl::instance().regs->set_range(pwm_ch, range);
        pwm_ctrl::instance().regs->set_data(pwm_ch, 0U); // 100% zero output
      }
      catch (...)
      {
        gpio_ctrl::instance().alloc.deallocate(pin);
        throw; 
      }
      gpio_ctrl::instance().regs->set_pin_function(pin,pwm_fn_info[0].alt_fn());
    }
  }
}}
