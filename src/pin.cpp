// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin.cpp 
/// @brief Single GPIO pin I/O class implementations.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin.h"
#include "phymem_ptr.h"
#include "gpio_registers.h"
#include "pin_alloc.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
      struct gpio_ctrl
      {
        phymem_ptr<volatile gpio_registers>   regs; 
        pin_allocator alloc;
        
        static gpio_ctrl & instance()
        {
          static gpio_ctrl gpio_control_area;
          return gpio_control_area;
        }

      private:
        gpio_ctrl()
        : regs(gpio_registers::physical_address, register_block_size)
        {}
        gpio_ctrl( gpio_ctrl const & ) = delete;
      };
    }

    void pin_base::open(pin_id pin, open_mode mode)
    {
      internal::gpio_ctrl::instance().alloc.allocate(pin);
      internal::gpio_ctrl::instance().regs->set_pin_function
                                            ( pin
                                            , (mode==out) ? gpio_pin_fn::output
                                                          : gpio_pin_fn::input
                                            
                                            );
      this->pin = pin;
      this->open_flag = true;
    }

    void pin_base::close()
    {
      if (is_open())
        {
          internal::gpio_ctrl::instance().alloc.deallocate(pin);
          this->open_flag = false;
        }
    }

    void opin::put( bool v )
    {
      if ( is_open() )
      {
        if ( v )
          {
            internal::gpio_ctrl::instance().regs->set_pin(get_pin());
          }
        else
          {
            internal::gpio_ctrl::instance().regs->clear_pin(get_pin());
          }
      }
    }


    bool ipin::get()
    {
      if ( is_open() )
        {
          return internal::gpio_ctrl::instance().regs->pin_level(get_pin());
        }
      else
        {
          return false;
        }
    }
  }
}}
