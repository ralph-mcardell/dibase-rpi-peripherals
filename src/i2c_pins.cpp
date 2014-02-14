// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_pins.cpp
/// @brief Use a pair of GPIO pins for use with I2C/BSC: implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "i2c_pins.h"
#include "gpio_alt_fn.h"
#include "gpio_ctrl.h"
#include "i2c_ctrl.h"
#include "periexcept.h"
#include <chrono>
#include <thread>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    using internal::i2c_ctrl;
    using internal::gpio_ctrl;
    using internal::pin_alt_fn::descriptor;
    using internal::pin_alt_fn::gpio_special_fn;
    using internal::gpio_pin_fn;

    namespace
    {
    // Repeated starts need to wait for the initial single byte write to start
    // before continuing and it is conceivable that this might be missed if the
    // process is swapped out at the wrong moment. To prevent infinite looping
    // a loop count maximum is imposed.
    // At 400KHz SCLK this count has been found to be in single decimal digit
    // range, with a 10kHz SCLK it may be up to around 150.
      std::uint32_t repeat_start_write_start_wait_count_max{10000U};

      descriptor get_alt_fn_descriptor
      ( pin_id pin
      , std::initializer_list<gpio_special_fn> special_fns
      )
      {
        using internal::pin_alt_fn::result_set;
        using internal::pin_alt_fn::select;
        auto pin_fn_info( select(pin,special_fns) );
        if (pin_fn_info.empty())
          {
            throw std::invalid_argument
                  { "i2c_pins::i2c_pins: Pin does not support "
                    "requested I2C/BSC special function."
                  };
          } 
        if (pin_fn_info.size()!=1)
          {
            throw std::range_error // this is possible if wrong ctor/pins combo
                  {"i2c_pins::i2c_pins: More than one pin alt function "
                   "selected that supports the requested I2C/BSC special "
                   "function."
                  };
          }
        return pin_fn_info[0];
      }

      void construct_common
      ( pin_id        sda_pin
      , pin_id        scl_pin
      , int           bsc_num   
      , hertz         f
      , std::uint16_t tout
      , std::uint16_t fedl
      , std::uint16_t redl
      , hertz         fc
      , gpio_pin_fn   sda_alt_fn
      , gpio_pin_fn   scl_alt_fn
      )
      {
        using internal::i2c_registers;
        using internal::register_t;

        i2c_registers ctx_builder;
        ctx_builder.control = 0U;
        ctx_builder.clk_div = 0U;
        ctx_builder.data_delay = 0U;
        ctx_builder.clk_stretch = 0U;

        register_t cdiv{fc.count()/f.count()};
        if (!ctx_builder.set_clock_divider(cdiv))
          {
            throw std::out_of_range( "i2c_pins::i2c_pins: f parameter "
                                     "not in the range [fc/32768,fc/2]."
                                   );
          }
        if (redl>(cdiv/2))
          {
            throw std::out_of_range( "i2c_pins::i2c_pins: redl parameter "
                                     "exceeds (fc/f)/2 (CDIV/2)."
                                   );          
          }
        if (fedl>(cdiv/2))
          {
            throw std::out_of_range( "i2c_pins::i2c_pins: fedl parameter "
                                     "exceeds (fc/f)/2 (CDIV/2)."
                                   );          
          }
        ctx_builder.set_read_delay(redl);
        ctx_builder.set_write_delay(fedl);
        ctx_builder.set_clock_stretch_timeout(tout);
        ctx_builder.set_enable(true);
        ctx_builder.clear_fifo();
        if (i2c_ctrl::instance().alloc.is_in_use(bsc_num))
          {
            throw bad_peripheral_alloc( "i2c_pins::i2c_pins: BSC peripheral is "
                                        "already being used locally."
                                      );
          }
        i2c_ctrl::instance().alloc.allocate(bsc_num);
        int pin_alloc_count{0};
        try
        {
          gpio_ctrl::instance().alloc.allocate(sda_pin); // CAN THROW
          ++pin_alloc_count;
          gpio_ctrl::instance().alloc.allocate(scl_pin); // CAN THROW
          ++pin_alloc_count;
        }
        catch (...)
        { // Oops - failed to complete resource acquisition and initialisation;
        // Release resources allocated so far and re-throw
          if (pin_alloc_count==1)
            {
              gpio_ctrl::instance().alloc.deallocate(sda_pin);
            }
          i2c_ctrl::instance().alloc.deallocate(bsc_num);
          throw;
        }
        
        gpio_ctrl::instance().regs->set_pin_function(sda_pin, sda_alt_fn);
        gpio_ctrl::instance().regs->set_pin_function(scl_pin, scl_alt_fn);

        i2c_ctrl::instance().regs(bsc_num)->clk_div = ctx_builder.clk_div;
        i2c_ctrl::instance().regs(bsc_num)->data_delay = ctx_builder.data_delay;
        i2c_ctrl::instance().regs(bsc_num)->clk_stretch = ctx_builder.clk_stretch;
        i2c_ctrl::instance().regs(bsc_num)->control = ctx_builder.control;
        i2c_ctrl::instance().regs(bsc_num)->clear_clock_timeout();
        i2c_ctrl::instance().regs(bsc_num)->clear_slave_ack_error();
      }
    }

    bool i2c_pins::is_busy() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_transfer_active();
    }

    bool i2c_pins::write_fifo_is_empty() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_tx_fifo_empty();
    }

    bool i2c_pins::write_fifo_has_space() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_tx_fifo_not_full();
    }
 
    bool i2c_pins::write_fifo_needs_writing() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_tx_fifo_needs_writing();
    }

    bool i2c_pins::read_fifo_is_full() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_rx_fifo_full();
    }

    bool i2c_pins::read_fifo_has_data() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_rx_fifo_not_empty();
    }

    bool i2c_pins::read_fifo_needs_reading() const
    {
      return i2c_ctrl::instance().regs(bsc_idx)->get_rx_fifo_needs_reading();
    }
    
    int i2c_pins::error_state()
    {
      int state{goodbit};
      if (i2c_ctrl::instance().regs(bsc_idx)->get_clock_timeout())
        {
          state = timeoutbit;
        }
      if (i2c_ctrl::instance().regs(bsc_idx)->get_slave_ack_error())
        {
          state |= noackowledgebit;
        }
      return state;
    }

    void i2c_pins::clear()
    {
      i2c_ctrl::instance().regs(bsc_idx)->clear_clock_timeout();
      i2c_ctrl::instance().regs(bsc_idx)->clear_slave_ack_error();
    }

    void i2c_pins::clear(state statebit)
    {
      if (statebit==timeoutbit)
        {
          i2c_ctrl::instance().regs(bsc_idx)->clear_clock_timeout();
        }
      if (statebit==noackowledgebit)
        {
          i2c_ctrl::instance().regs(bsc_idx)->clear_slave_ack_error();
        }
    }

    void i2c_pins::abort()
    {
      i2c_ctrl::instance().regs(bsc_idx)->clear_fifo();
    }
 
    constexpr auto sda_idx(0U);
    constexpr auto scl_idx(1U);
    constexpr pin_id_int_t pin_not_used{53U};

    i2c_pins::~i2c_pins()
    {
      clear(); // Clear any error conditions
      i2c_ctrl::instance().regs(bsc_idx)->clear_fifo(); // also aborts transfer
      i2c_ctrl::instance().regs(bsc_idx)->set_enable(false);
      i2c_ctrl::instance().alloc.deallocate(bsc_idx);
      gpio_ctrl::instance().alloc.deallocate(pin_id(pins[sda_idx]));
      gpio_ctrl::instance().alloc.deallocate(pin_id(pins[scl_idx]));
    }

    i2c_pins::i2c_pins
    ( pin_id        sda_pin
    , pin_id        scl_pin
    , hertz         f
    , std::uint16_t tout
    , std::uint16_t fedl
    , std::uint16_t redl
    , hertz         fc
    )
    {
      pins.fill(pin_id(pin_not_used)); 
    
    // Get each pin's alt function for its BSC/I2C special function.
    // Note: any of these can throw - but nothing allocated yet so OK
      descriptor alt_fn_info[number_of_pins];
      alt_fn_info[sda_idx] = get_alt_fn_descriptor( sda_pin
                                                  , { gpio_special_fn::sda0
                                                    , gpio_special_fn::sda1
                                                    }
                                                  );
      alt_fn_info[scl_idx] = get_alt_fn_descriptor( scl_pin
                                                  , { gpio_special_fn::scl0
                                                    , gpio_special_fn::scl1
                                                    }
                                                  );
      if (  (   alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda0
            &&  alt_fn_info[scl_idx].special_fn()!=gpio_special_fn::scl0
            )
         || (   alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda1
            &&  alt_fn_info[scl_idx].special_fn()!=gpio_special_fn::scl1
            )
         )
        {
          throw std::invalid_argument
                { "i2c_pins::i2c_pins: Pins do not support I2C/BSC "
                  "SDA and SCL functions for the same BSC peripheral."
                };
        }
      unsigned bsc_num{alt_fn_info[sda_idx].special_fn()==gpio_special_fn::sda0?0U:1U};

      construct_common( sda_pin, scl_pin, bsc_num, f, tout, fedl, redl, fc
                      , alt_fn_info[sda_idx].alt_fn()
                      , alt_fn_info[scl_idx].alt_fn()
                      );
      
      bsc_idx = bsc_num;
      pins[sda_idx] = sda_pin;
      pins[scl_idx] = scl_pin;
    }

    i2c_pins::i2c_pins
    ( pin_id        sda_pin
    , pin_id        scl_pin
    , int           bsc_num   
    , hertz         f
    , std::uint16_t tout
    , std::uint16_t fedl
    , std::uint16_t redl
    , hertz         fc
    )
    {
      pins.fill(pin_id(pin_not_used)); 
    
      if (bsc_num!=0 && bsc_num!=1)
        {
          throw std::out_of_range
                { "i2c_pins::i2c_pins: bsc_num parameter is not 0 or 1." };
        }
 
      gpio_special_fn sda_fn
                      {bsc_num==0?gpio_special_fn::sda0:gpio_special_fn::sda1};
      gpio_special_fn scl_fn
                      {bsc_num==0?gpio_special_fn::scl0:gpio_special_fn::scl1};

    // Get each pin's alt function for its BSC/I2C special function.
    // Note: any of these can throw - but nothing allocated yet so OK
      descriptor alt_fn_info[number_of_pins];
      alt_fn_info[sda_idx] = get_alt_fn_descriptor(sda_pin, {sda_fn});
      alt_fn_info[scl_idx] = get_alt_fn_descriptor(scl_pin, {scl_fn});

      construct_common( sda_pin, scl_pin, bsc_num, f, tout, fedl, redl, fc
                      , alt_fn_info[sda_idx].alt_fn()
                      , alt_fn_info[scl_idx].alt_fn()
                      );

      bsc_idx = bsc_num;
      pins[sda_idx] = sda_pin;
      pins[scl_idx] = scl_pin;
    }

    using internal::i2c_transfer_type;

    std::size_t i2c_pins::start_write
    ( std::uint32_t addrs
    , std::uint32_t dlen
    , std::uint8_t const * pdata
    , std::size_t count
    )
    {
      if (is_busy())
        {
          throw std::logic_error
                  { "i2c_pins::start_write: Unable to start write transaction,"
                    " BSC/I2C peripheral is busy with an ongoing transaction." 
                  };
        }
      if (!i2c_ctrl::instance().regs(bsc_idx)->set_slave_address(addrs))
        {
            throw std::out_of_range
                    { "i2c_pins::start_write: "
                      "Slave address not in the range [0,127]." 
                    };
        }
      if (!i2c_ctrl::instance().regs(bsc_idx)->set_data_length(dlen))
        {
            throw std::out_of_range
                    { "i2c_pins::start_write: Transaction "
                      "data length not in the range [0,65535]." 
                    };
        }
      std::size_t bytes_written{0U};
      i2c_ctrl::instance().regs(bsc_idx)->set_transfer_type
                                          (i2c_transfer_type::write);
      i2c_ctrl::instance().regs(bsc_idx)->clear_transfer_done();
      if (pdata && count)
        {
          while (count-- && write_fifo_has_space() && bytes_written < dlen)
            {
              i2c_ctrl::instance().regs(bsc_idx)->transmit_fifo_write(*pdata++);
              ++bytes_written;
            }
        }
      i2c_ctrl::instance().regs(bsc_idx)->start_transfer();
      return bytes_written;
    }

    std::size_t i2c_pins::write
    ( std::uint8_t const * pdata
    , std::size_t count
    )
    {
      std::size_t bytes_written{0U};
      if (is_busy())
        {
          while (count-- && write_fifo_has_space())
            {
              i2c_ctrl::instance().regs(bsc_idx)->transmit_fifo_write(*pdata++);
              ++bytes_written;
            }
        }
      return bytes_written;
    }

    void i2c_pins::start_read
    ( std::uint32_t addrs
    , std::uint32_t dlen
    )
    {
      if (is_busy())
        {
          throw std::logic_error
                  { "i2c_pins::start_read: Unable to start read transaction,"
                    " BSC/I2C peripheral is busy with an ongoing transaction." 
                  };
        }
      if (!i2c_ctrl::instance().regs(bsc_idx)->set_slave_address(addrs))
        {
            throw std::out_of_range
                    { "i2c_pins::start_read: "
                      "Slave address not in the range [0,127]." 
                    };
        }
      if (!i2c_ctrl::instance().regs(bsc_idx)->set_data_length(dlen))
        {
            throw std::out_of_range
                    { "i2c_pins::start_read: Transaction "
                      "data length not in the range [0,65535]." 
                    };
        }
      i2c_ctrl::instance().regs(bsc_idx)->set_transfer_type
                                          (i2c_transfer_type::read);
      i2c_ctrl::instance().regs(bsc_idx)->clear_transfer_done();
      i2c_ctrl::instance().regs(bsc_idx)->start_transfer();
    }

    bool i2c_pins::start_read
    ( std::uint32_t addrs
    , std::uint8_t desc
    , std::uint32_t dlen
    )
    {
      using internal::i2c_registers;

      if (is_busy())
        {
          throw std::logic_error
                  { "i2c_pins::start_read: Unable to start read transaction,"
                    " BSC/I2C peripheral is busy with an ongoing transaction." 
                  };
        }
      if (dlen>i2c_registers::dlen_mask)
        {
            throw std::out_of_range
                    { "i2c_pins::start_read: Transaction "
                      "data length not in the range [0,65535]." 
                    };
        }
      if (!i2c_ctrl::instance().regs(bsc_idx)->set_slave_address(addrs))
        {
            throw std::out_of_range
                    { "i2c_pins::start_read: "
                      "Slave address not in the range [0,127]." 
                    };
        }
      i2c_ctrl::instance().regs(bsc_idx)->set_data_length(1U);
      i2c_ctrl::instance().regs(bsc_idx)->set_transfer_type
                                          (i2c_transfer_type::write);
      i2c_ctrl::instance().regs(bsc_idx)->clear_transfer_done();
      i2c_ctrl::instance().regs(bsc_idx)->transmit_fifo_write(desc);
      i2c_ctrl::instance().regs(bsc_idx)->start_transfer();
      
      uint32_t count{0U};
      while (!is_busy())
        {
          if (++count>repeat_start_write_start_wait_count_max)
            {
              return false;
            }
        }
      i2c_ctrl::instance().regs(bsc_idx)->set_data_length(dlen);
      i2c_ctrl::instance().regs(bsc_idx)->set_transfer_type
                                          (i2c_transfer_type::read);
      i2c_ctrl::instance().regs(bsc_idx)->start_transfer();
      return true;
    }

    std::size_t i2c_pins::read
    ( std::uint8_t * pdata
    , std::size_t count
    )
    {
      std::size_t bytes_read{0U};
      if (is_busy() || read_fifo_has_data())
        {
          while (count-- && read_fifo_has_data())
            {
              *pdata++ = i2c_ctrl::instance().regs(bsc_idx)->receive_fifo_read();
              ++bytes_read;
            }
        }
      return bytes_read;
    }
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
