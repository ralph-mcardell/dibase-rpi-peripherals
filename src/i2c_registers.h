// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_registers.h 
/// @brief \b Internal : low-level (GPIO) I2C control registers type definition.
///        \i Note : Broadcom call BCM2835 I2C compliant peripherals BSC 
///       (for Broadcom Serial Control - or Controller).
///
/// The details here relate to the BCM2835 BSC master interfaces known as BSC0,
/// BSC1 and BSC2 (which is reserved for use with the HDMI interface).
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 3 BSC for details.de.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H

# include "peridef.h"
# include <cstdint>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Enumeration specifying types of I2C/BSC data transfer.
    ///
    /// As set in the BSC C (control) register READ field.
      enum class i2c_transfer_type : register_t
      { write = 0U  ///< Transfer is a write packet transfer
      , read  = 1U  ///< Transfer is a read packet transfer
      };
      
    /// @brief Represents layout of I2C supporting BSC control registers
    /// with operations.
    ///
    /// Permits access to BCM2835 BSC masters BSC0, BSC1 and BSC2 interface
    /// registers when an instance is mapped to the appropriate physical memory
    /// location.
    ///
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 3 for published
    /// details.
    ///
    /// Member function operations are provided to query and set the various
    /// fields and flags for BSC channels' control.
      struct i2c_registers
      {
        enum : register_t
        { c_read_mask         =      1U ///< C register READ field mask value
        , c_clear_fifo        =   0x30U ///< C register CLEAR field value 11b
        , c_start_transfer    =   0x80U ///< C register ST field value 1b
        , c_int_on_done_mask  =  0x100U ///< C register INTD field mask value
        , c_int_on_done_bit   =      8U ///< C register INTD field bit number
        , c_int_on_txw_mask   =  0x200U ///< C register INTT field mask value
        , c_int_on_txw_bit    =      9U ///< C register INTT field bit number
        , c_int_on_rxr_mask   =  0x400U ///< C register INTR field mask value
        , c_int_on_rxr_bit    =     10U ///< C register INTR field bit number
        , c_enable_mask       = 0x8000U ///< C register I2CEN field mask value
        , c_enable_bit        =     15U ///< C register I2CEN field bit number
        };

      /// @brief Physical address of start of BCM2835 BSC0 control registers
        constexpr static physical_address_t 
                      bsc0_physical_address = peripheral_base_address+0x205000;

      /// @brief Physical address of start of BCM2835 BSC1 control registers
        constexpr static physical_address_t 
                      bsc1_physical_address = peripheral_base_address+0x804000;

      /// @brief Physical address of start of BCM2835 BSC2 control registers
      /// (\i Note: reserved for use with the HDMI interface)
        constexpr static physical_address_t 
                      bsc2_physical_address = peripheral_base_address+0x805000;

        register_t  control;      ///< BSC Master Control, C
        register_t  status;       ///< BSC Master Status, S
        register_t  data_length;  ///< BSC Master Data Length, DLEN
        register_t  slave_addrs;  ///< BSC Master Slave Address, A
        register_t  fifo;         ///< BSC Master TX and RX FIFOs, FIFO
        register_t  clk_div;      ///< BSC Master Clock Divider, DIV
        register_t  data_delay;   ///< BSC Master Data Delay, DEL
        register_t  clk_stretch;  ///< BSC Clock Stretch Time out, CLKT

      /// @brief Return the I2C read/write packet transfer type setting
      /// @returns  i2c_transfer_type::read for read packet transfer, 
      ///           i2c_transfer_type::write for write packet transfer.
        i2c_transfer_type  get_transfer_type() const volatile
        {
          return static_cast<i2c_transfer_type>(control&c_read_mask);
        }

      /// @brief Return the currently set interrupt on done value
      /// @returns  true if an interrupt is generated when DONE==true.
      ///           false if no such interrupt is generated.
        bool get_interrupt_on_done() volatile const
        {
          return control & c_int_on_done_mask;
        }

      /// @brief Return the currently set interrupt on TXW condition value
      ///
      /// Note: TXW is short for Transmit FIFO needs Writing 
      ///(approaching being full)
      ///
      /// @returns  true if an interrupt is generated on TXW conditions.
      ///           false if no such interrupt is generated.
        bool get_interrupt_on_txw() volatile const
        {
          return control & c_int_on_txw_mask;
        }

      /// @brief Return the currently set interrupt on RXR condition value
      ///
      /// Note: RXR is short for Receive FIFO needs Reading 
      ///(approaching being empty)
      ///
      /// @returns  true if an interrupt is generated on RXR conditions.
      ///           false if no such interrupt is generated.
        bool get_interrupt_on_rxr() volatile const
        {
          return control & c_int_on_rxr_mask;
        }

      /// @brief Return the BSC/I2C controller enable state
      ///
      /// @returns  true if I2C/BSC controller is enabled.
      ///           false if I2C/BSC controller is disabled.
        bool get_enable() volatile const
        {
          return control & c_enable_mask;
        }

      /// @brief Set the I2C read/write packet transfer type
      /// @param type   i2c_transfer_type::read for read packet transfer, 
      ///               i2c_transfer_type::write for write packet transfer.
        void set_transfer_type(i2c_transfer_type type) volatile
        {
          control = (control & ~c_read_mask) | static_cast<register_t>(type);
        }

      /// @brief Set generate interrupt on done state
      /// @param[in]  generate Pass true to have interrupts generated when 
      ///                      DONE is true (get_transfer_done()==true)
      ///                      Pass false not to generate interrupts when DONE
      ///                      is true
        void set_interrupt_on_done(bool generate) volatile
        {
          control = (control & ~c_int_on_done_mask) 
                   | (generate<<c_int_on_done_bit);
        }

      /// @brief Set generate interrupt on TXW condition
      ///
      /// Note: TXW is short for Transmit FIFO needs Writing 
      ///(approaching being full)
      ///
      /// @param[in]  generate Pass true to have interrupts generated when 
      ///                      TXW is true (get_tx_fifo_needs_writing()==true)
      ///                      Pass false not to generate interrupts when TXW
      ///                      is true
        void set_interrupt_on_txw(bool generate) volatile
        {
          control = (control & ~c_int_on_txw_mask) 
                   | (generate<<c_int_on_txw_bit);
        }

      /// @brief Set generate interrupt on RXR condition
      ///
      /// Note: RXR is short for Receive FIFO needs Reading 
      ///(approaching being empty)
      ///
      /// @param[in]  generate Pass true to have interrupts generated when 
      ///                      RXR is true (get_rx_fifo_needs_reading()==true)
      ///                      Pass false not to generate interrupts when RXR
      ///                      is true
        void set_interrupt_on_rxr(bool generate) volatile
        {
          control = (control & ~c_int_on_rxr_mask) 
                   | (generate<<c_int_on_rxr_bit);
        }

      /// @brief Set enable/disable state if I2C/BSC controller
      ///
      /// @param[in]  enable  Pass true to enable the controller, false to
      ///                     disable the controller.
        void set_enable(bool enable) volatile
        {
          control = (control & ~c_enable_mask) 
                   | (enable<<c_enable_bit);
        }

      /// @brief Clear I2C/BSC FIFO
        void clear_fifo() volatile
        {
          control |= c_clear_fifo;
        }

      /// @brief Start a new I2C data transfer
        void start_transfer() volatile
        {
          control |= c_start_transfer;
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H
