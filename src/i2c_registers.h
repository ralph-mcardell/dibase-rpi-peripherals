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
        enum 
        { c_read_mask         = 1U
        , c_int_on_done_mask  = 0x80U
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

      /// @brief Return the type of I2C data transfer
      /// @returns  i2c_transfer_type::read for a read packet transfer, 
      ///           i2c_transfer_type::write for a write packet transfer.
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
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H
