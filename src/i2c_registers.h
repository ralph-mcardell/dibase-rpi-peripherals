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
        , s_xfer_active_mask  =      1U ///< S register TA field mask value
        , s_xfer_done_mask    =      2U ///< S register DONE field mask value
        , s_xfer_txw_mask     =      4U ///< S register TXW field mask value
        , s_xfer_rxr_mask     =      8U ///< S register RXR field mask value
        , s_xfer_txd_mask     =   0x10U ///< S register TXD field mask value
        , s_xfer_rxd_mask     =   0x20U ///< S register RXD field mask value
        , s_xfer_txe_mask     =   0x40U ///< S register TXE field mask value
        , s_xfer_rxf_mask     =   0x80U ///< S register RXF field mask value
        , s_ack_err_mask      =  0x100U ///< S register ERR field mask value
        , s_clk_timeout_mask  =  0x200U ///< S register CLKT field mask value
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

      /// @brief Return the currently set transfer active value
      /// @returns  true if transfer is active.
      ///           false if transfer is not active.
        bool get_transfer_active() volatile const
        {
          return status & s_xfer_active_mask;
        }

      /// @brief Return the currently set transfer done value
      ///
      /// This bit field is cleared by clear_transfer_done, which writes a 1
      /// to the S register DONE field.
      ///
      /// @returns  true if transfer is complete
      ///           false if transfer is in progress
        bool get_transfer_done() volatile const
        {
          return status & s_xfer_done_mask;
        }

      /// @brief Return the currently set FIFO need writing (TXW) flag value
      ///
      /// Note: has meaning only when write is under way
      ///
      /// Cleared by writing sufficient data to FIFO
      ///
      /// @returns  true if the FIFO near to being empty
      ///           false if the FIFO still quite full
        bool get_tx_fifo_needs_writing() volatile const
        {
          return status & s_xfer_txw_mask;
        }

      /// @brief Return the currently set FIFO need reading (RXR) flag value
      ///
      /// Note: has meaning only when read is under way
      ///
      /// Cleared by reading sufficient data from FIFO
      ///
      /// @returns  true if the FIFO near to being full
      ///           false if the FIFO still quite empty
        bool get_rx_fifo_needs_reading() volatile const
        {
          return status & s_xfer_rxr_mask;
        }

      /// @brief Return the currently set FIFO can accept data (TXD) flag value
      ///
      /// Note: has meaning only when write is under way
      ///
      /// Cleared when sufficient data clocked out of FIFO
      ///
      /// @returns  true if the FIFO can accept at least 1 byte
      ///           false if the FIFO is full and cannot accept any more data
        bool get_tx_fifo_not_full() volatile const
        {
          return status & s_xfer_txd_mask;
        }

      /// @brief Return the currently set FIFO contains data (RXD) flag value
      ///
      /// Note: has meaning only when read is under way
      ///
      /// Cleared by reading sufficient data from FIFO
      ///
      /// @returns  true if the FIFO contains at least 1 byte
      ///           false if the FIFO empty
        bool get_rx_fifo_not_empty() volatile const
        {
          return status & s_xfer_rxd_mask;
        }

      /// @brief Return the currently set FIFO empty (TXE) flag value
      ///
      /// Note: has meaning only when write is under way
      ///
      /// Cleared when more data written to FIFO
      ///
      /// @returns  true if the FIFO contains no data to transmit
      ///           false if the FIFO contains data to transmit
        bool get_tx_fifo_empty() volatile const
        {
          return status & s_xfer_txe_mask;
        }

      /// @brief Return the currently set FIFO full (RXF) flag value
      ///
      /// Note: has meaning only when read is under way
      ///
      /// Cleared by reading sufficient data from FIFO
      ///
      /// @returns  true if the FIFO full and no further data can be received
      ///           false if the FIFO is not full
        bool get_rx_fifo_full() volatile const
        {
          return status & s_xfer_rxf_mask;
        }

      /// @brief Return the value of the slave acknowledgement error (ERR) flag
      ///
      /// Cleared by clear_slave_ack_error, which write 1 to the S register ERR
      /// field.
      ///
      /// @returns  true if a slave has not acknowledged its address
      ///           false no errors detected
        bool get_slave_ack_error() volatile const
        {
          return status & s_ack_err_mask;
        }

      /// @brief Return the value of the clock stretch time out (CLKT) flag
      ///
      /// Cleared by clear_clock_timeout, which write 1 to the S register CLKT
      /// field.
      ///
      /// @returns  true if a slave has held the SCL signal low for longer than
      ///           specified by the CLKT register.
      ///           false no errors detected
        bool get_clock_timeout() volatile const
        {
          return status & s_clk_timeout_mask;
        }

      /// @brief Clear transfer done state (S register DONE field==1).
      ///
      /// Cleared by writing a 1 the S register DONE field.
        void clear_transfer_done() volatile
        {
          status |= s_xfer_done_mask;
        }

      /// @brief Clear slave acknowledgement error state 
      ///(S register ERR field==1).
      ///
      /// Cleared by writing a 1 the S register ERR field.
        void clear_slave_ack_error() volatile
        {
          status |= s_ack_err_mask;
        }

      /// @brief Clear clock stretch time out state (S register CLKT field==1).
      ///
      /// Cleared by writing a 1 the S register CLKT field.
        void clear_clock_timeout() volatile
        {
          status |= s_clk_timeout_mask;
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H
