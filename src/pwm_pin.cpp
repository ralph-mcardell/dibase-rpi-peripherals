// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_pin.cpp 
/// @brief Internal PWM control type implantation and definition.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pwm_pin.h"
#include "pwm_ctrl.h"
#include "clock_parameters.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { 
    namespace
    {
    // See http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
    // Section 9.1 Pulse Width Modulator Overview, final bullet point
      constexpr hertz default_pwm_clock_frequency(megahertz(100));
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
  }
}}
