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
#include <unistd.h>

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

      useconds_t const pud_wait_us{10};
      static void apply_pull(pin_id pin, unsigned mode)
      {
        if ( mode&ipin::pull_up && mode&ipin::pull_down )
          {
            throw std::invalid_argument
                  ("Cannot open ipin with both pull up and down enabled!");
          }

        gpio_ctrl::instance().regs->set_pull_up_down_mode
                                    ( mode&ipin::pull_up 
                                        ? gpio_pud_mode::enable_pull_up_control
                                    : mode&ipin::pull_down 
                                        ? gpio_pud_mode::enable_pull_down_control
                                        : gpio_pud_mode::off
                                    );
        ::usleep(pud_wait_us);
        gpio_ctrl::instance().regs->assert_pin_pull_up_down_clock( pin );
        ::usleep(pud_wait_us);
        gpio_ctrl::instance().regs->set_pull_up_down_mode(gpio_pud_mode::off);
        gpio_ctrl::instance().regs->remove_all_pin_pull_up_down_clocks();
      }
    }

    void pin_base::open(pin_id pin, direction_mode dir)
    {
      internal::gpio_ctrl::instance().alloc.allocate(pin);
      internal::gpio_ctrl::instance().regs->set_pin_function
                                            ( pin
                                            , (dir==out) ? gpio_pin_fn::output
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

    ipin::~ipin()
    {
      internal::apply_pull(get_pin(), pull_disable);
      pin_base::close();
    }

    void ipin::open(pin_id pin, unsigned mode)
    {
      pin_base::open(pin, in);
      internal::apply_pull(pin, mode);
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
