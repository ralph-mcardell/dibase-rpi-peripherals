// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_registers.h 
/// @brief Low-level GPIO control registers type definition.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
/// I/O (GPIO) for details.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H
 #define DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H

 #include "peridef.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Strongly typed enumeration of GPIO pin function values.
  /// 
  /// Each GPIO pin can be set for use as an input pin, an output pin or one of
  /// up to six alternative functions. Which pins support which alternative
  /// functions is detailed in section 6.2 of the 
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a>.
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

  /// @brief Strongly typed enumeration of GPIO pull up/down control values.
  ///
  /// When setting up pull up/down for GPIO pins, pins can be set to have pull
  /// up, pull down or neither (off). The off value is also used when
  /// completing the pull up/down setup sequence. See section 6.1 GPPUD and
  /// GPPUDCLKn sections of the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a>.
    enum class gpio_pud_mode : register_t
    { off                       = 0
    , enable_pull_down_control  = 1
    , enable_pull_up_control    = 2
    };

  /// @brief Type representing register pairs for 1 bit per pin field groups
  ///
  /// There are 54 GPIO pins and many control registers have 1 bit per GPIO pin
  /// and thus come in pairs (e.g. GPSET0, GPSET1 and GPCLR0, GPCLR1) with the
  /// single bit fields for GPIO pins 0..31 in GPxxx0 and those for pins 32..53
  /// in the lower bits of GPxxx1. The one_bit_field_register type presents a
  /// united interface for such register pairs and provides commonly use
  /// functions that are required to perform various control functions of the
  /// various registers.
  ///
    class one_bit_field_register
    {
      register_t reg[2];
    
    public:
    /// @brief Subscript operator. Allows direct access to register pair array
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  index   Index of register in pair (0..1, not range checked).
    /// @return reference to the 32-bit register indicated by index. Undefined
    ///         if index not 0 or 1.
      register_t volatile & operator[](std::size_t index) volatile
      {
        return reg[index];
      }
      
    /// @brief Set a single bit to 1, leaving other bits as they were.
    ///
    /// Bits are in the range 0...63, but for GPIO pins only 0...53 should be
    /// used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  bitnumber Bit number in the register pair looked at as a
    ///                       single value (0..63, not range checked).
      void set_bit( unsigned int bitnumber ) volatile
      {
        reg[bitnumber/register_width] |= 1U<<(bitnumber%register_width);
      }

    /// @brief Clear a single bit to 0, leaving other bits as they were.
    ///
    /// Bits are in the range 0...63, but for GPIO pins only 0...53 should be
    /// used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  bitnumber Bit number in the register pair looked at as a
    ///                       single value (0..63, not range checked).
      void clear_bit( unsigned int bitnumber ) volatile
      {
        reg[bitnumber/register_width] &= ~(1U<<(bitnumber%register_width));
      }

    /// @brief Set a single bit to 1, with other bits in the same word set to 0
    ///
    /// Overwrites the one word of the pair containing the single bit field for
    /// the passed bit number with a value having just the requested bit set to
    /// 1. The other word is left untouched. This is useful for registers such
    /// as GPSET0,1 and GPCLR0,1 where a zero bit means no change and only one
    /// word would need to be written to to change a single pin's state.
    ///
    /// Bits are in the range 0...63, but for GPIO pins only 0...53 should be
    /// used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  bitnumber Bit number in the register pair looked at as a
    ///                       single value (0..63, not range checked).
      void set_just_bit( unsigned int bitnumber ) volatile
      {
        reg[bitnumber/register_width] = 1U<<(bitnumber%register_width);
      }
      
    /// @brief Returns single bit's state.
    ///
    /// Bits are in the range 0...63, but for GPIO pins only 0...53 should be
    /// used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  bitnumber Bit number in the register pair looked at as a
    ///                       single value (0..63, not range checked).
    /// @return Zero if the bit is 0 or a non-zero value if it is 1.
      register_t get_bit( unsigned int bitnumber ) volatile
      {
         return reg[bitnumber/register_width] & (1U<<(bitnumber%register_width));
      }
    
    /// @brief Clear all bits in both register words to zero.
    ///
    /// It may be useful if composing register values off to one side in main
    /// memory to clear a one_bit_field_register instance before starting. This
    /// is not done by default - i.e. on construction - as it may not always be
    /// needed.
    ///
    /// Note: this is a volatile function as access will possibly be through a
    /// pointer to volatile data.
      void clear_all_bits() volatile
      {
        reg[0] = 0;
        reg[1] = 0;
      }
    };

  /// @brief Represents layout of GPIO control registers with operations.
  ///
  /// Permits access to BCM2835 GPIO control registers when an instance is
  /// mapped to the correct physical memory location.
  ///
  /// See the
  /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
  /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 6 for details
  ///
    struct gpio_registers
    {
    /// @brief Physical address of start of BCM2835 GPIO control registers
      static const physical_address_t physical_address
                                          = peripheral_base_address + 0x200000;

      register_t gpfsel[6];   ///< GPIO pins function select (R/W)
      register_t reserved_do_not_use_0;     ///< Reserved, currently unused
      one_bit_field_register gpset;    ///< GPIO pins output set high (W)
      register_t reserved_do_not_use_1;     ///< Reserved, currently unused
      one_bit_field_register gpclr;    ///< GPIO pins output clear low (W)
      register_t reserved_do_not_use_2;     ///< Reserved, currently unused
      one_bit_field_register gplev;    ///< GPIO pins input level (R)
      register_t reserved_do_not_use_3;     ///< Reserved, currently unused
      one_bit_field_register gpeds;    ///< GPIO pins event detect status (R/W)
      register_t reserved_do_not_use_4;     ///< Reserved, currently unused
      one_bit_field_register gpren;    ///< GPIO pins rising edge detect enable (R/W)
      register_t reserved_do_not_use_5;     ///< Reserved, currently unused
      one_bit_field_register gpfen;    ///< GPIO pins falling edge detect enable (R/W)
      register_t reserved_do_not_use_6;     ///< Reserved, currently unused
      one_bit_field_register gphen;    ///< GPIO pins high detect enable (R/W)
      register_t reserved_do_not_use_7;     ///< Reserved, currently unused
      one_bit_field_register gplen;    ///< GPIO pins low detect enable (R/W)
      register_t reserved_do_not_use_8;     ///< Reserved, currently unused
      one_bit_field_register gparen;   ///< GPIO pins async. rising edge detect (R/W)
      register_t reserved_do_not_use_9;     ///< Reserved, currently unused
      one_bit_field_register gpafen;   ///< GPIO pins async. falling edge detect (R/W)
      register_t reserved_do_not_use_a;     ///< Reserved, currently unused
      gpio_pud_mode gppud;       ///< GPIO pins pull-up/down enable (R/W)
      one_bit_field_register gppudclk; ///< GPIO pins pull-up/down enable clock (R/W)
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
        register_t const BitsPerPin(3);  // number of bits used for each pin
        register_t const PinsPerReg(register_width/BitsPerPin);
        register_t const MaxFnValue((1U<<BitsPerPin)-1);
        
        register_t fn_value( static_cast<register_t>(fn) );
        gpfsel[pinid/PinsPerReg] &= ~(MaxFnValue<<((pinid%PinsPerReg)*BitsPerPin));
        gpfsel[pinid/PinsPerReg] |=  fn_value<<((pinid%PinsPerReg)*BitsPerPin);
      }

    /// @brief Sets the single specified pin to a high (1, true, on) value.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin to set high
    ///                     (0..53) - not range checked.
      void set_pin( unsigned int pinid ) volatile
      {
        gpset.set_just_bit( pinid );
      }

    /// @brief Clear the single specified pin to a low (0, false, off) value.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin to set low
    ///                     (0..53) - not range checked.
      void clear_pin( unsigned int pinid ) volatile
      {
        gpclr.set_just_bit( pinid );
      }

    /// @brief Return the low/high level of the single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin to get level of
    ///                     (0..53) - not range checked.
    /// @return Zero if the pin level is low or a non-zero value if it is high.
      register_t pin_level( unsigned int pinid ) volatile
      {
        return gplev.get_bit(pinid);
      }

    /// @brief Return the event detection status of the single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin to get event status of
    ///                     (0..53) - not range checked.
    /// @return Zero if no event detected for the pin or a non-zero value if
    ///         an event was detected for the pin.
      register_t pin_event( unsigned int pinid ) volatile
      {
        return gpeds.get_bit(pinid);
      }

    /// @brief Clear a single specified pin's event notification.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin whose event notification
    ///                     to clear. (0..53) - not range checked.
      void clear_pin_event( unsigned int pinid ) volatile
      {
        gpeds.set_just_bit( pinid );
      }

    /// @brief Enable rising edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which rising edge
    ///                     events will be detected. (0..53) - not range checked
      void pin_rising_edge_detect_enable( unsigned int pinid ) volatile
      {
        gpren.set_bit( pinid );
      }

    /// @brief Disable rising edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which rising edge
    ///                     events won't be detected. (0..53), not range checked
      void pin_rising_edge_detect_disable( unsigned int pinid ) volatile
      {
        gpren.clear_bit( pinid );
      }

    /// @brief Enable falling edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which falling edge
    ///                     events will be detected. (0..53) - not range checked
      void pin_falling_edge_detect_enable( unsigned int pinid ) volatile
      {
        gpfen.set_bit( pinid );
      }

    /// @brief Disable falling edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which falling edge
    ///                     events won't be detected. (0..53), not range checked
      void pin_falling_edge_detect_disable( unsigned int pinid ) volatile
      {
        gpfen.clear_bit( pinid );
      }

    /// @brief Enable high detect events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which high detect 
    ///                     events will be detected. (0..53) - not range checked
      void pin_high_detect_enable( unsigned int pinid ) volatile
      {
        gphen.set_bit( pinid );
      }

    /// @brief Disable high detect events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which high detect
    ///                     events won't be detected. (0..53), not range checked
      void pin_high_detect_disable( unsigned int pinid ) volatile
      {
        gphen.clear_bit( pinid );
      }

    /// @brief Enable low detect events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which low detect 
    ///                     events will be detected. (0..53) - not range checked
      void pin_low_detect_enable( unsigned int pinid ) volatile
      {
        gplen.set_bit( pinid );
      }

    /// @brief Disable low detect events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin for which low detect
    ///                     events won't be detected. (0..53), not range checked
      void pin_low_detect_disable( unsigned int pinid ) volatile
      {
        gplen.clear_bit( pinid );
      }

    /// @brief Enable async rising edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid Id number of the GPIO pin for which async rising edge
    ///                   events will be detected. (0..53) - not range checked
      void pin_async_rising_edge_detect_enable( unsigned int pinid ) volatile
      {
        gparen.set_bit( pinid );
      }

    /// @brief Disable async rising edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid Id number of the GPIO pin for which async rising edge
    ///                   events won't be detected. (0..53), not range checked
      void pin_async_rising_edge_detect_disable( unsigned int pinid ) volatile
      {
        gparen.clear_bit( pinid );
      }

    /// @brief Enable async falling edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid Id number of the GPIO pin for which async falling edge
    ///                   events will be detected. (0..53) - not range checked
      void pin_async_falling_edge_detect_enable( unsigned int pinid ) volatile
      {
        gpafen.set_bit( pinid );
      }

    /// @brief Disable async falling edge events for a single specified pin.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid Id number of the GPIO pin for which async falling edge
    ///                   events won't be detected. (0..53), not range checked
      void pin_async_falling_edge_detect_disable( unsigned int pinid ) volatile
      {
        gpafen.clear_bit( pinid );
      }

    /// @brief Set the pull up/down actualisation control mode.
    ///
    /// set_pull_up_down_mode has to be used in conjuction with
    /// \ref assert_pin_pull_up_down_clock and
    /// \ref remove_all_pin_pull_up_down_clocks. See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> section 6.1, GPPUD and
    /// GPPUDCLKn descriptions for details. The latter in particular specifies
    /// the underlying sequence that needs to be used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  mode  the mode to set gppud to.
      void set_pull_up_down_mode( gpio_pud_mode mode ) volatile
      {
        gppud = mode;
      }

    /// @brief Assert a single pin's pull up/down clock.
    ///
    /// assert_pin_pull_up_down_clock has to be used in conjuction with
    /// \ref set_pull_up_down_mode and \ref remove_all_pin_pull_up_down_clocks.
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> section 6.1, GPPUD and
    /// GPPUDCLKn descriptions for details. The latter in particular specifies
    /// the underlying sequence that needs to be used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
    ///
    /// @param[in]  pinid   Id number of the GPIO pin to assert pull up/down
    ///                     clock for. (0..53) - not range checked.
      void assert_pin_pull_up_down_clock( unsigned int pinid ) volatile
      {
        gppudclk.set_just_bit( pinid );
      }

    /// @brief Remove all pin's pull up/down clock assertions.
    ///
    /// remove_all_pin_pull_up_down_clocks has to be used in conjuction with
    /// \ref set_pull_up_down_mode and \ref assert_pin_pull_up_down_clock.
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> section 6.1, GPPUD and
    /// GPPUDCLKn descriptions for details. The latter in particular specifies
    /// the underlying sequence that needs to be used.
    ///
    /// Note: this is a volatile function as access will probably be through a
    /// pointer to volatile data.
      void remove_all_pin_pull_up_down_clocks() volatile
      {
        gppudclk.clear_all_bits();
      }
    };
  }
}}
#endif // DIBASE_RPI_PERIPHERALS_GPIO_REGISTERS_H
