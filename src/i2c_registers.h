// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_registers.h 
/// @brief \b Internal : low-level (GPIO) I2C control registers type definition.
///
/// @note
/// Broadcom call BCM2835 I2C compliant peripherals BSC (for Broadcom Serial
/// Control - or Controller).
///
/// The details here relate to the BCM2835 BSC master interfaces known as BSC0,
/// BSC1 and BSC2 (which is reserved for use with the HDMI interface).
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 3 BSC for details.
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
        , dlen_mask           = 0xFFFFU ///< DLEN register mask value
        , a_mask              =   0x7FU ///< A register mask value
        , clk_divisor_min     = 2U      ///< Effective minimum clock divisor value
        , clk_divisor_max     = 32768U  ///< Effective maximum clock divisor value, written as 0
        , clk_divisor_mask    = 0x7FFFU ///< CDIV register DIV field mask value
                                        ///< (Low 15-bits of register only, in
                                        ///< line with documented 32768 maximum
                                        ///< divider value rather than 
                                        ///< documented DIV field bits 15:0)
        , del_redl_mask       = 0xFFFFU ///< DEL register REDL field mask value
        , del_fedl_mask   = 0xFFFF0000U ///< DEL register FEDL field mask value
        , del_fedl_bit        =   0x10U ///< DEL register FEDL field start bit number
        , del_max             = 0xFFFFU ///< DEL register REDL & FEDL fields' maximum value
        , clkt_tout_mask      = 0xFFFFU ///< CLKT register TOUT field mask value
        };

      /// @brief Physical address of start of BCM2835 BSC0 control registers
        constexpr static physical_address_t 
                      bsc0_physical_address = peripheral_base_address+0x205000;

      /// @brief Physical address of start of BCM2835 BSC1 control registers
        constexpr static physical_address_t 
                      bsc1_physical_address = peripheral_base_address+0x804000;

      /// @brief Physical address of start of BCM2835 BSC2 control registers
      /// (\e Note: reserved for use with the HDMI interface)
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
      /// @returns  \c true if an interrupt is generated when DONE==\c true,
      ///           \c false if no such interrupt is generated.
        bool get_interrupt_on_done() volatile const
        {
          return control & c_int_on_done_mask;
        }

      /// @brief Return the currently set interrupt on TXW condition value
      ///
      /// @note
      /// TXW is short for Transmit FIFO needs Writing (approaching being empty)
      ///
      /// @returns  \c true if an interrupt is generated on TXW conditions,
      ///           \c false if no such interrupt is generated.
        bool get_interrupt_on_txw() volatile const
        {
          return control & c_int_on_txw_mask;
        }

      /// @brief Return the currently set interrupt on RXR condition value
      ///
      /// @note
      /// RXR is short for Receive FIFO needs Reading (approaching being full)
      ///
      /// @returns  \c true if an interrupt is generated on RXR conditions,
      ///           \c false if no such interrupt is generated.
        bool get_interrupt_on_rxr() volatile const
        {
          return control & c_int_on_rxr_mask;
        }

      /// @brief Return the BSC/I2C controller enable state
      ///
      /// @returns  \c true if I2C/BSC controller is enabled,
      ///           \c false if I2C/BSC controller is disabled.
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
      /// @param[in]  generate Pass \c true to have interrupts generated when 
      ///                      DONE is \c true (get_transfer_done()==\c true)
      ///                      Pass \c false not to generate interrupts when
      ///                      DONE is \c true.
        void set_interrupt_on_done(bool generate) volatile
        {
          control = (control & ~c_int_on_done_mask) 
                   | (generate<<c_int_on_done_bit);
        }

      /// @brief Set generate interrupt on TXW condition
      ///
      /// @note
      /// TXW is short for Transmit FIFO needs Writing (approaching being empty)
      ///
      /// @param[in]  generate Pass \c true to have interrupts generated when 
      ///                      TXW is \c true
      ///                      (get_tx_fifo_needs_writing()==\c true).
      ///                      Pass \c false not to generate interrupts when
      ///                      TXW is \c true.
        void set_interrupt_on_txw(bool generate) volatile
        {
          control = (control & ~c_int_on_txw_mask) 
                   | (generate<<c_int_on_txw_bit);
        }

      /// @brief Set generate interrupt on RXR condition
      ///
      /// @note
      /// RXR is short for Receive FIFO needs Reading (approaching being full)
      ///
      /// @param[in]  generate Pass \c true to have interrupts generated when 
      ///                      RXR is \c true
      ///                      (get_rx_fifo_needs_reading()==\c true).
      ///                      Pass \c false not to generate interrupts when
      ///                      RXR is \c true.
        void set_interrupt_on_rxr(bool generate) volatile
        {
          control = (control & ~c_int_on_rxr_mask) 
                   | (generate<<c_int_on_rxr_bit);
        }

      /// @brief Set enable/disable state of I2C/BSC controller
      ///
      /// @param[in]  enable  Pass \c true to enable the controller, \c false
      ///                     to disable the controller.
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
      /// @returns  \c true if transfer is active,
      ///           \c false if transfer is not active.
        bool get_transfer_active() volatile const
        {
          return status & s_xfer_active_mask;
        }

      /// @brief Return the currently set transfer done value
      ///
      /// @note
      /// This bit field is cleared by clear_transfer_done(), which writes a 1
      /// to the S register DONE field.
      ///
      /// @returns  \c true if transfer is complete,
      ///           \c false if transfer is in progress.
        bool get_transfer_done() volatile const
        {
          return status & s_xfer_done_mask;
        }

      /// @brief Return the currently set FIFO need writing (TXW) flag value
      ///
      /// @note
      /// Has meaning only when a write transaction is in progress
      ///
      /// Cleared by writing sufficient data to the FIFO
      ///
      /// @returns  \c true if the FIFO near to being empty,
      ///           \c false if the FIFO still quite full.
        bool get_tx_fifo_needs_writing() volatile const
        {
          return status & s_xfer_txw_mask;
        }

      /// @brief Return the currently set FIFO need reading (RXR) flag value
      ///
      /// @note
      /// Has meaning only when a read transaction is in progress
      ///
      /// Cleared by reading sufficient data from the FIFO
      ///
      /// @returns  \c true if the FIFO near to being full,
      ///           \c false if the FIFO still quite empty.
        bool get_rx_fifo_needs_reading() volatile const
        {
          return status & s_xfer_rxr_mask;
        }

      /// @brief Return the currently set FIFO can accept data (TXD) flag value
      ///
      /// @note
      /// Has meaning only when a write transaction is in progress
      ///
      /// Cleared when sufficient data clocked out of the FIFO
      ///
      /// @returns  \c true if the FIFO can accept at least 1 byte,
      ///           \c false if the FIFO is full and cannot accept any more
      ///           data.
        bool get_tx_fifo_not_full() volatile const
        {
          return status & s_xfer_txd_mask;
        }

      /// @brief Return the currently set FIFO contains data (RXD) flag value
      ///
      /// @note
      /// Has meaning only when a read transaction is in progress
      ///
      /// Cleared by reading sufficient data from the FIFO
      ///
      /// @returns  \c true if the FIFO contains at least 1 byte,
      ///           \c false if the FIFO empty.
        bool get_rx_fifo_not_empty() volatile const
        {
          return status & s_xfer_rxd_mask;
        }

      /// @brief Return the currently set FIFO empty (TXE) flag value
      ///
      /// @note
      /// Has meaning only when a write transaction is in progress
      ///
      /// Cleared when more data written to the FIFO
      ///
      /// @returns  \c true if the FIFO contains no data to transmit,
      ///           \c false if the FIFO contains data to transmit.
        bool get_tx_fifo_empty() volatile const
        {
          return status & s_xfer_txe_mask;
        }

      /// @brief Return the currently set FIFO full (RXF) flag value
      ///
      /// @note
      /// Has meaning only when a read transaction is in progress
      ///
      /// Cleared by reading sufficient data from the FIFO
      ///
      /// @returns  \c true if the FIFO is full and no further data can be
      ///           received,
      ///           \c false if the FIFO is not full.
        bool get_rx_fifo_full() volatile const
        {
          return status & s_xfer_rxf_mask;
        }

      /// @brief Return the value of the slave acknowledgement error (ERR) flag
      ///
      /// @note
      /// The ERR flag can be cleared by calling clear_slave_ack_error(),
      /// which writes 1 to the S register ERR field.
      ///
      /// @returns  \c true if a slave has not acknowledged its address,
      ///           \c false if no errors detected.
        bool get_slave_ack_error() volatile const
        {
          return status & s_ack_err_mask;
        }

      /// @brief Return the value of the clock stretch time out (CLKT) flag
      ///
      /// @note
      /// The CLKT flag can be cleared by calling clear_clock_timeout(),
      /// which writes 1 to the S register CLKT field.
      ///
      /// @returns  \c true if a slave has held the SCL signal low for longer
      ///           than specified by the CLKT register,
      ///           \c false if no errors detected.
        bool get_clock_timeout() volatile const
        {
          return status & s_clk_timeout_mask;
        }

      /// @brief Clear transfer done state (S register DONE field==1).
      ///
      /// Cleared by writing a 1 to the S register DONE field.
        void clear_transfer_done() volatile
        {
          status |= s_xfer_done_mask;
        }

      /// @brief Clear slave acknowledgement error state 
      ///(S register ERR field==1).
      ///
      /// Cleared by writing a 1 to the S register ERR field.
        void clear_slave_ack_error() volatile
        {
          status |= s_ack_err_mask;
        }

      /// @brief Clear clock stretch time out state (S register CLKT field==1).
      ///
      /// Cleared by writing a 1 to the S register CLKT field.
        void clear_clock_timeout() volatile
        {
          status |= s_clk_timeout_mask;
        }

      /// @brief Get the bytes remaining in the current transfer.
      ///
      /// If called when a transfer is in progress
      /// (get_transfer_active()==\c true) the remaining bytes to transfer is
      /// returned. If called when a transfer has just completed
      /// (get_transfer_done()==\c true), 0 is returned. If called when both
      /// get_transfer_active() and get_transfer_done() return \c false then
      /// the last value written to the DLEN register
      /// (e.g. by calling set_data_length()) is returned.
      ///
      /// @returns Bytes remaining on current I2C data transfer [0,65535].
        register_t get_data_length() volatile const
        {
          return data_length & dlen_mask;
        }

      /// @brief Set the number of bytes for forthcoming I2C transfers.
      ///
      /// Setting a data length value applies until changed, even over
      /// multiple transfers.
      ///
      /// @param[in] len Data transfer byte length [0,65535]
      /// @returns  \c true if the \c len parameter value in the range [0,65535],
      ///           \c false if the \c len parameter value out of range and
      ///           nothing was written to the DLEN register.
        bool set_data_length(register_t len) volatile
        {
          if ( len > dlen_mask )
            {
              return false;
            }
          data_length = len;
          return true;
        }

      /// @brief Get current value of the slave address (A) register.
      ///
      /// @returns I2C slave device address value in the A register [0,127].
        register_t get_slave_address() volatile const
        {
          return slave_addrs & a_mask;
        }

      /// @brief Set the value of the slave address (A) register.
      ///
      /// Setting an I2C slave device address continues to apply even over
      /// multiple transfers.
      ///
      /// @param[in] addrs I2C slave device address to set [0,127]
      /// @returns  \c true if \c addrs parameter value in the range [0,127], 
      ///           \c false if \c addrs parameter value out of range and
      ///           nothing is written to the A register.
        bool set_slave_address(register_t addrs) volatile
        {
          if ( addrs > a_mask )
            {
              return false;
            }
          slave_addrs = addrs;
          return true;
        }

      /// @brief Write 8-bit byte to the FIFO
      ///
      /// @param[in] data   8-bit byte to write to FIFO for transmission.
        void transmit_fifo_write(std::uint8_t data) volatile
        {
          fifo = data;
        }

      /// @brief Read 8-bit byte from the FIFO
      /// @returns 8-bit byte of received data read from the FIFO
        std::uint8_t receive_fifo_read() volatile const
        {
          return fifo;
        }

      /// @brief Return currently set I2C/BSC master clock divisor value
      ///
      /// @note
      /// Values will be even, in the range [0,32768]. A value of 
      /// 32768 represented as a register value of 0.
      ///
      /// @returns BSC master clock register divisor (CDIV) 15-bit field value.
        register_t get_clock_divider() volatile const
        {
          register_t value{clk_div&clk_divisor_mask};
          return value?value:static_cast<register_t>(clk_divisor_max);
        }

      /// @brief Set I2C/BSC master clock divisor value. 
      /// Divides the system APB clock.
      ///
      /// @note
      /// Values should be even, in the range [2,32768]. Odd values will
      /// be rounded down.
      ///
      /// @param[in] divisor  BSC master APB clock divisor value [2,32768]
      /// @returns  \c true if operation performed,
      ///           \c false if operation not performed as divisor out of range.
        bool set_clock_divider(register_t divisor) volatile
        {
          if (clk_divisor_min>divisor || divisor>clk_divisor_max)
            {
              return false;
            }
          clk_div = divisor&clk_divisor_mask;
          return true;
        }

      /// @brief Return the bit-read delay after SCL rising edge (DEL:REDL)
      ///
      /// The returned value is the DEL register REDL field value which is
      /// the delay in core clock cycles after SCL rising edge before reading
      /// a data bit from SDA.   
      ///
      /// @returns SCL Rising Edge delay before writing a bit of data [0,65535].
        register_t get_read_delay() volatile const
        {
          return data_delay&del_redl_mask;
        }
      
      /// @brief Return the bit-write delay after SCL falling edge (DEL:FEDL)
      ///
      /// The returned value is the DEL register FEDL field value which is
      /// the delay in core clock cycles after SCL falling edge before writing
      /// a data bit to SDA.
      ///
      /// @returns SCL Falling Edge delay before reading a bit of data
      ///          [0,65535].
        register_t get_write_delay() volatile const
        {
          return (data_delay&del_fedl_mask)>>del_fedl_bit;
        }

      /// @brief Set the bit-read delay after SCL rising edge (DEL:REDL)
      ///
      /// The \c delay parameter sets the DEL register REDL field value which
      /// is the delay in core clock cycles after SCL rising edge before
      /// reading a data bit from SDA.   
      ///
      /// @param[in] delay  SCL Rising Edge delay before writing a bit of
      ///                   data [0,65535]
      /// @returns  \c true if operation performed,
      ///           \c false if operation not performed as delay out of range.
        bool set_read_delay(register_t delay) volatile
        {
          if (delay>del_max)
            {
              return false;
            }
          data_delay = (data_delay&~del_redl_mask) | delay;
          return true;
        }
      
      /// @brief Set the bit-write delay after SCL falling edge (DEL:FEDL)
      ///
      /// The \c delay parameter sets the DEL register FEDL field value which
      /// is the delay in core clock cycles after SCL falling edge before
      /// writing a data bit to SDA.
      ///
      /// @param[in] delay  SCL Falling Edge delay before reading a bit of
      ///                   data [0,65535]
      /// @returns  \c true if operation performed,
      ///           \c false if operation not performed as delay out of range.
        bool set_write_delay(register_t delay) volatile
        {
          if (delay>del_max)
            {
              return false;
            }
          data_delay = (data_delay&~del_fedl_mask) | (delay<<del_fedl_bit);
          return true;
        }

      /// @brief Return the clock stretch time-out value (CLKT:TOUT)
      ///
      /// The returned value is the CLKT register TOUT field value which
      /// specifies how long the master waits, in SCL clock cycles, for a
      /// slave device to stretch the clock (by keeping SCL low after master
      /// sets it high) before deciding the slave has hung. A zero value 
      /// indicates the feature is disabled.
      ///
      /// @returns Clock-stretch value [0,65535] in SCL ticks.
        register_t get_clock_stretch_timeout() volatile const
        {
          return clk_stretch&clkt_tout_mask;
        }

      /// @brief Set the clock stretch time-out value (CLKT:TOUT)
      ///
      /// The \c clks parameter sets the CLKT register TOUT field value which
      /// specifies how long the master waits, in SCL clock cycles, for a
      /// slave device to stretch the clock (by keeping SCL low after master
      /// sets it high) before deciding the slave has hung. A zero value 
      /// disables the feature.
      ///
      /// @param[in] clks  Clock-stretch value [0,65535] in SCL ticks.
      /// @returns  \c true if operation performed,
      ///           \c false if operation not performed as delay out of range.
        bool set_clock_stretch_timeout(register_t clks) volatile
        {
          if (clks>clkt_tout_mask)
            {
              return false;
            }
          clk_stretch = clks;
          return true;
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_REGISTERS_H
