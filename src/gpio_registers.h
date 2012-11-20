// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_registers.h 
/// @brief Low-level GPIO control registers type definition.
///
/// Refer to the Broadcom BCM2835 Peripherals Datasheet PDF file for details:
///
/// http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
///
/// Chapter 6 General Purpose I/O (GPIO) 
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H
 #define DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H

 #include "peridef.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Strongly typoed enumeration of GPIO pin function values.
    enum class gpio_pin_fn : register_t
    { input   = 0
    , output  = 1
    , alt0    = 4
    , alt1    = 5
    , alt2    = 6
    , alt3    = 7
    , alt4    = 3
    , alt5    = 2
    };
    
  /// @brief Represents layout of GPIO control registers with operations.
  ///
  /// Permits access to BCM2835 GPIO control registers when an instance is
  /// mapped to the correct physical memory location
  ///
    struct gpio_registers
    {
      static const physical_address_t physical_address
                                          = peripheral_base_address + 0x200000;

      register_t gpfsel[6];   ///< GPIO pins function select (R/W)
      register_t reserved_do_not_use_0;     ///< Reserved, currently unused
      register_t gpset[2];    ///< GPIO pins output set high (W)
      register_t reserved_do_not_use_1;     ///< Reserved, currently unused
      register_t gpclr[2];    ///< GPIO pins output clear low (W)
      register_t reserved_do_not_use_2;     ///< Reserved, currently unused
      register_t gplev[2];    ///< GPIO pins input level (R)
      register_t reserved_do_not_use_3;     ///< Reserved, currently unused
      register_t gpeds[2];    ///< GPIO pins event detect status (R/W)
      register_t reserved_do_not_use_4;     ///< Reserved, currently unused
      register_t gpren[2];    ///< GPIO pins rising edge detect enable (R/W)
      register_t reserved_do_not_use_5;     ///< Reserved, currently unused
      register_t gpfen[2];    ///< GPIO pins falling edge detect enable (R/W)
      register_t reserved_do_not_use_6;     ///< Reserved, currently unused
      register_t gphen[2];    ///< GPIO pins high detect enable (R/W)
      register_t reserved_do_not_use_7;     ///< Reserved, currently unused
      register_t gplen[2];    ///< GPIO pins low detect enable (R/W)
      register_t reserved_do_not_use_8;     ///< Reserved, currently unused
      register_t gparen[2];   ///< GPIO pins async. rising edge detect (R/W)
      register_t reserved_do_not_use_9;     ///< Reserved, currently unused
      register_t gpafen[2];   ///< GPIO pins async. falling edge detect (R/W)
      register_t reserved_do_not_use_a;     ///< Reserved, currently unused
      register_t gppud;       ///< GPIO pins pull-up/down enable (R/W)
      register_t gppudclk[2]; ///< GPIO pins pull-up/down enable clock (R/W)
      register_t reserved_do_not_use_b[4];  ///< Reserved, currently unused
      register_t test;        ///< Test Note: Only 4 bits wide (R/W)
      
    /// @brief Set a GPIO pin's function.
    ///
    /// GPIO pins may be set to be either input or output or one of upto
    /// five alternate functions. How many and which alternative functions
    /// are available varies. They are described in the Broadcom BCM2835
    /// Peripherals datasheet, section 6.2.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin whose function is to be
    ///                     set (0..53) - not range checked.
    /// @param[in]  fn      Scoped enumeration of the required function.
      void set_pin_function( unsigned int pinid, gpio_pin_fn fn ) volatile
      {
        unsigned int fn_value( static_cast<register_t>(fn) );
        gpfsel[pinid/10] &= ~(7<<((pinid%10)*3));
        gpfsel[pinid/10] |=  fn_value<<((pinid%10)*3);
      }
    };
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H
