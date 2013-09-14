// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_registers.h 
/// @brief \b Internal : low-level (GPIO) SPI0 control registers type definition.
///
/// The details here relate to the BCM2835 SPI master interface known as SPI0,
/// SPI channel 0, and _not_  the two axillary mini-SPI interfaces SPI1 and
/// SPI2 (whose functions cannot be accessed from Raspberry Pi connector pins).
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 10 SPI for details
/// along with additional information on SPI found in the Gertboard source code.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_REGISTERS_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_REGISTERS_H

# include "peridef.h"
# include <cstdint>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Enumerated action values for SPI FIFO clearing operation
    enum class spi0_fifo_clear_action : register_t
    { clear_tx    = 0x10U ///< Clear transmit FIFO (bit 4 set)
    , clear_rx    = 0x20U ///< Clear receive FIFO (bit 5 set)
    , clear_tx_rx = 0x30U ///< Clear both transmit and receive FIFOs
    };

    /// @brief Represents layout of SPI0 control registers with operations.
    ///
    /// Permits access to BCM2835 SPI master SPI0 interface registers when an 
    /// instance is mapped to the correct physical memory location.
    ///
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 10 for published
    /// details. Note that the information on the chip select and the various
    /// chip select polarity control fields is confusing:
    ///
    /// - CS register chip select 2-bit CS field accepts values 0 , 1 and 2
    ///   but SPI0 only has 2 chip enable lines: CE0, CE1
    /// - CS register has an unnumbered chip select polarity field CSPOL
    /// - CS register has numbered chip select polarity fields CSPOL0, CSPOL1
    ///   and CSPOL2 fields.
    ///
    /// The information in the datasheet table detailing the CS Register (pages
    /// 153 - 155) gives no clear explanation on how the single unnumbered
    /// CSPOL field combines with the numbered CSPOLn fields and what is
    /// meant by references to chip select 2. Empirical results (i.e. trying
    /// the various combinations of CSPOL and CSPOLn field values with
    /// Transfer Active 0 (not active), and 1 with CS field values 0, 1 and 2
    /// and observing the low/high states of SPI0_CE0_N and SPI0_CE1_N) lead
    /// to the conclusion that the states of the unnumbered CSPOL field and
    /// the CSPOL2 field have no effect on CE0 and CE1, and that a CS field
    /// value of 2 (with TA 1, active) gave the same CE0, CE1 state as for TA
    /// 0 (transfers inactive).
    ///
    /// Thus:
    ///   - TA=0 (inactive)     : de-asserts both CE0 and CE1
    ///   - TA=1 (active), CS=0 : asserts CE0, de-asserts CE1
    ///   - TA=1 (active), CS=1 : de-asserts CE0, asserts CE1
    ///   - TA=1 (active), CS=1 : de-asserts both CE0 and CE1
    ///
    /// and:
    ///   - CSPOL0=0 : CE0 asserted when low
    ///   - CSPOL0=1 : CE0 asserted when high
    ///   - CSPOL1=0 : CE1 asserted when low
    ///   - CSPOL1=1 : CE1 asserted when high
    ///
    /// Member function operations are provided to query and set the various
    /// fields and flags for SPI0 channel control.
      struct spi0_registers
      {
      private:
        enum 
        { cs_max_chip_id  = 2U        // Maximum chip identifier value
        , cs_chip_select_mask = 3U    // CS register chip select field bit-mask
        , cs_clock_phase_mask = 4U    // CS register clock phase field bit-mask
        , cs_clock_phase_bit = 2U     // CS register clock phase field bit number
        , cs_clock_polarity_mask = 8U // CS register clock polarity field bit-mask
        , cs_clock_polarity_bit = 3U  // CS register clock polarity field bit number
        , cs_cs_polarity_mask = 0x40U // CS register chip select polarity field bit-mask
        , cs_cs_polarity_bit = 6U     // CS register chip select polarity field bit number
        , cs_xfer_active_mask = 0x80U // CS register transfer active field bit-mask
        , cs_xfer_active_bit = 7U     // CS register transfer active field bit number
        , cs_dma_enable_mask = 0x100U // CS register DMA enable field bit-mask
        , cs_dma_enable_bit = 8U      // CS register DMA enable field bit number
        , cs_int_on_done_mask =0x200U // CS register interrupt on done field bit-mask
        , cs_int_on_done_bit = 9U     // CS register interrupt on done field bit number
        , cs_int_on_rxr_mask = 0x400U // CS register interrupt on RXR field bit-mask
        , cs_int_on_rxr_bit = 10U     // CS register interrupt on RXR field bit number
        , cs_auto_deassert_cs_mask=0x800U// CS register ADCS field bit-mask
        , cs_auto_deassert_cs_bit=11U // CS register ADCS field bit number
        , cs_read_enable_mask=0x1000U // CS register read enable field bit-mask
        , cs_read_enable_bit = 12U    // CS register read enable field bit number
        , cs_lossi_enable_mask=0x2000U// CS register LoSSI enable field bit-mask
        , cs_lossi_enable_bit = 13U   // CS register LoSSI enable field bit number
        , cs_xfer_done_mask = 0x10000U// CS register transfer done field bit-mask
        , cs_rxd_mask     = 0x20000U  // CS register RX FIFO has data field bit-mask
        , cs_txd_mask     = 0x40000U  // CS register TX FIFO has space field bit-mask
        , cs_rxr_mask     = 0x80000U  // CS register RX FIFO needs reading field bit-mask
        , cs_rxf_mask     = 0x100000U // CS register RX FIFO full field bit-mask
        , cs_lossi_dma_enable_mask=0x1000000U// CS register LoSSI DMA mode enable field bit-mask
        , cs_lossi_dma_enable_bit=24U // CS register LoSSI DMA mode enable field bit number
        , cs_lossi_long_mask=0x2000000U// CS register LoSSI long word field bit-mask
        , cs_lossi_long_bit = 25U     // CS register LoSSI long word field bit number
        , cs_csline_polarity_base_mask=0x200000U// CS register CSPOL0 field bit-mask
        , clk_divisor_min = 1U        // Effective minimum clock divisor value
        , clk_divisor_max = 65536U    // Effective maximum clock divisor value, written as 0
        , clk_divisor_mask = 0xffffU  // Low 16-bits of register only
        , dlen_len_min = 0U           // Minimum DMA transfer byte length
        , dlen_len_max = 0xffffU      // Maximum DMA transfer byte length
        , dlen_len_mask = 0xffffU     // Low 16-bits of register only
        , ltoh_toh_min = 1U           // Minimum LoSSI mode o/p hold delay
        , ltoh_toh_max = 15U          // Maximum LoSSI mode o/p hold delay
        , ltoh_toh_mask = 15U         // Low 4 bits of register only
        , dc_tdreq_min = 0U           // Minimum DMA write request threshold
        , dc_tdreq_max = 255U         // Maximum DMA write request threshold
        , dc_tdreq_mask = 255U        // Low 8 bits of register only
        , dc_tpanic_min = 0U          // Minimum DMA write panic threshold
        , dc_tpanic_max = 255U        // Maximum DMA write panic threshold
        , dc_tpanic_mask = 0xff00U    // Bits [15,8] of register only
        , dc_tpanic_bit = 8U          // Start bit of write panic threshold field
        , dc_rdreq_min = 0U           // Minimum DMA read request threshold
        , dc_rdreq_max = 255U         // Maximum DMA read request threshold
        , dc_rdreq_mask = 0xff0000U   // Bits [23:16] of register only
        , dc_rdreq_bit = 16U          // Start bit of read request threshold field
        , dc_rpanic_min = 0U          // Minimum DMA read panic threshold
        , dc_rpanic_max = 255U        // Maximum DMA read panic threshold
        , dc_rpanic_mask = 0xff000000U// Upper 8 bits of register only
        , dc_rpanic_bit = 24U         // Start bit of read panic threshold field
        };

      public:
      /// @brief Physical address of start of BCM2835 SPI0 control registers
        constexpr static physical_address_t 
                            physical_address = peripheral_base_address+0x204000;

        register_t  control_and_status; ///< SPI Master Control and Status, CS
        register_t  fifo;               ///< SPI Master TX and RX FIFOs, FIFO
        register_t  clock;              ///< SPI Master Clock Divider, CLK
        register_t  data_length;        ///< SPI Master Data Length, DLEN
        register_t  lossi_mode_toh;     ///< SPI LOSSI mode TOH, LTOH
        register_t  dma_controls;       ///< SPI DMA DREQ Controls, DC

      /// @brief Return the currently selected chip id.
      /// @returns The selected chip id value in the range [0,2].
        register_t get_chip_select() volatile const
        {
          return control_and_status & cs_chip_select_mask;
        }

      /// @brief Return the currently set clock phase
      /// @returns  true if first SCLK transition at beginning of data bit.
      ///           false if first SCLK transition at middle of data bit.
        bool get_clock_phase() volatile const
        {
          return control_and_status & cs_clock_phase_mask;
        }

      /// @brief Return the currently set clock polarity
      /// @returns  true if rest state of clock is high.
      ///           false if rest state of clock is low.
        bool get_clock_polarity() volatile const
        {
          return control_and_status & cs_clock_polarity_mask;
        }

      /// @brief Return the currently set chip select polarity for all chip
      /// select lines
      ///
      /// Returns value of CSPOL field for all chip select lines 
      ///
      /// @returns  true if chip select lines are active high.
      ///           false if chip select lines are active low.
        bool get_chip_select_polarity() volatile const
        {
          return control_and_status & cs_cs_polarity_mask;
        }

      /// @brief Return the currently set chip select polarity for a
      /// specific line
      ///
      /// Returns the value of one of the CSPOL0, CSPOL1 or CSPOL2 fields
      /// depending on the cs_line parameter value.
      ///
      /// @param[in] cs_line  Chip select line to return specific polarity
      ///                     field value for [0,2].
      /// @returns  true if chip select line is active high.
      ///           false if chip select line is active low OR cs_line is out
      ///           of range (not 0, 1 or 2). 
        bool get_chip_select_polarity(register_t cs_line) volatile const
        {
          return (cs_line<=cs_max_chip_id) 
              && (control_and_status & (cs_csline_polarity_base_mask<<cs_line));
        }

      /// @brief Return the currently set transfer active value
      /// @returns  true if transfer is active.
      ///           false if transfer is not active.
        bool get_transfer_active() volatile const
        {
          return control_and_status & cs_xfer_active_mask;
        }

      /// @brief Return the currently set DMA enable value
      /// @returns  true if DMA operation enabled and requested are generated.
      ///           false if DMA operation disabled and no requests are issued.
        bool get_dma_enable() volatile const
        {
          return control_and_status & cs_dma_enable_mask;
        }

      /// @brief Return the currently set interrupt on done value
      /// @returns  true if an interrupt is generated when DONE==true.
      ///           false if no such interrupt is generated.
        bool get_interrupt_on_done() volatile const
        {
          return control_and_status & cs_int_on_done_mask;
        }

      /// @brief Return the currently set interrupt on RXR value
      ///
      /// RXR is short for receive FIFO needs reading
      ///
      /// @returns  true if an interrupt is generated when RXR==true.
      ///           false if no such interrupt is generated.
        bool get_interrupt_on_rxr() volatile const
        {
          return control_and_status & cs_int_on_rxr_mask;
        }

      /// @brief Return the currently set automatic de-assert chip select value
      ///
      /// @returns  true if chip select de-asserted at the end of a DMA transfer
      ///           false if it is not.
        bool get_auto_deassert_chip_select() volatile const
        {
          return control_and_status & cs_auto_deassert_cs_mask;
        }

      /// @brief Return the currently set read enable value
      ///
      /// This bit field determines the data direction when in
      /// bidirectional mode. When set a device can send data to the BCM2835.
      ///
      /// @returns  true if going to read from an SPI device
      ///           false if going to write to an SPI device
        bool get_read_enable() volatile const
        {
          return control_and_status & cs_read_enable_mask;
        }

      /// @brief Return the currently set LoSSI enable value
      ///
      /// This bit field determines the whether the SPI interface behaves as
      /// a LoSSI master or SPI master.
      ///
      /// @returns  true if serial interface behaves as a LoSSI master
      ///           false if serial interface behaves as an SPI master
        bool get_lossi_enable() volatile const
        {
          return control_and_status & cs_lossi_enable_mask;
        }

      /// @brief Return the currently set transfer done value
      ///
      /// This bit field is cleared by writing more data to the
      /// transmit FIFO or by setting transfer active (TA) to 0
      /// [set_tranfer_active(false)].
      ///
      /// @returns  true if transfer is complete
      ///           false if transfer is in progress or transfers not active
        bool get_transfer_done() volatile const
        {
          return control_and_status & cs_xfer_done_mask;
        }

      /// @brief Return the currently set receive FIFO has data (RXD) flag value
      ///
      /// @returns  true if receive FIFO contains at least 1 byte
      ///           false if receive FIFO is empty
        bool get_rx_fifo_not_empty() volatile const
        {
          return control_and_status & cs_rxd_mask;
        }

      /// @brief Return the currently set transmit FIFO has space (TXD)
      /// flag value
      ///
      /// @returns  true if transmit FIFO has space for at least 1 byte
      ///           false if transmit FIFO is full
        bool get_tx_fifo_not_full() volatile const
        {
          return control_and_status & cs_txd_mask;
        }

      /// @brief Return the currently set receive FIFO needs reading (RXR)
      /// flag value
      ///
      /// This bit is reset by reading enough data from the receive FIFO or
      /// by setting the transfer active (TA) flag to 0 
      /// [set_tranfer_active(false)].
      ///
      /// @returns  true if receive FIFO near to being full
      ///           false if receive FIFO is not full or transfers not active 
        bool get_rx_fifo_needs_reading() volatile const
        {
          return control_and_status & cs_rxr_mask;
        }

      /// @brief Return the currently set receive FIFO is full (RXF)
      /// flag value
      ///
      /// @returns  true if receive FIFO is full. No serial data will be
      ///           transferred until data is read from the receive FIFO.
      ///           false if receive FIFO is _not_ full
        bool get_rx_fifo_full() volatile const
        {
          return control_and_status & cs_rxf_mask;
        }

      /// @brief Return the currently set DMA mode in LoSSI mode enable value
      /// @returns  true if DMA mode enabled in LoSSI mode
      ///           false if not
        bool get_lossi_dma_enable() volatile const
        {
          return control_and_status & cs_lossi_dma_enable_mask;
        }

      /// @brief Return the currently set LoSSI long data word mode value
      /// @returns  true if writing to the FIFO writes a 32-bit word
      ///           false if writing to the FIFO writes a single byte
        bool get_lossi_long_word() volatile const
        {
          return control_and_status & cs_lossi_long_mask;
        }

      /// @brief Set the chip select line value.
      /// @param[in] chp_id Id of chip to select. In the range [0,2].
      /// @returns  true if the operation was performed; false if it was not
      ///           because chip_id value was not in the range [0,2].
        bool set_chip_select(register_t chip_id) volatile
        {
          if (chip_id>cs_max_chip_id)
            {
              return false;
            }
          control_and_status = (control_and_status & ~cs_chip_select_mask)
                              | chip_id;
          return true;
        }

      /// @brief Set the set clock phase value
      /// @param[in]  at_start  Pass true for first SCLK transition at
      ///                       beginning of data bit or false for first
      ///                       SCLK transition at middle of data bit.
        void set_clock_phase(bool at_start) volatile
        {
          control_and_status = (control_and_status & ~cs_clock_phase_mask)
                              | (at_start<<cs_clock_phase_bit);
        }

      /// @brief Set the clock polarity value
      /// @param[in] is_high   Pass true for rest state of clock to be high.
      ///                      Pass false for rest state of clock to be low.
        void set_clock_polarity(bool is_high) volatile
        {
          control_and_status = (control_and_status & ~cs_clock_polarity_mask)
                              | (is_high<<cs_clock_polarity_bit);
        }

      /// @brief Set the chip select polarity for all chip select lines
      ///
      /// Sets the value of CSPOL field for all chip select lines 
      ///
      /// @param[in] is_high  Pass true for chip select lines active high.
      ///                     Pass false for chip select lines active low.
        void set_chip_select_polarity(bool is_high) volatile
        {
          control_and_status = (control_and_status & ~cs_cs_polarity_mask)
                              | (is_high<<cs_cs_polarity_bit);
        }

      /// @brief Set the chip select polarity for a single specific line
      ///
      /// Set the value of one of the CSPOL0, CSPOL1 or CSPOL2 fields
      /// depending on the cs_line parameter value. Other field bits are
      /// unchanged.
      ///
      /// @param[in] cs_line  Chip select line to set specific polarity
      ///                     field value for [0,2].
      /// @param[in] is_high  Pass true for chip select line active high.
      ///                     Pass false for chip select line active low.
      /// @returns  true if operation performed;
      ///           false if operation not performed as cs_line is out
      ///           of range (not 0, 1 or 2). 
        bool set_chip_select_polarity(register_t cs_line, bool is_high) volatile
        {
          if (cs_line>cs_max_chip_id)
            {
              return false;
            }
          control_and_status = is_high? control_and_status 
                                      | (cs_csline_polarity_base_mask<<cs_line)
                                      : control_and_status 
                                      & ~(cs_csline_polarity_base_mask<<cs_line)
                                      ;
          return true;
        }

      /// @brief Clear one or both of the SPI0 data transmit/receive FIFOs
      ///
      /// @param[in] action  Which FIFOs to clear: receive, transmit or both
        void clear_fifo(spi0_fifo_clear_action action) volatile
        {
          control_and_status |= static_cast<register_t>(action);
        }

      /// @brief Set transfer active state
      /// @param[in] is_active  Pass true to place in transfer active state
      ///                       Pass false to place in transfer not active state
        void set_transfer_active(bool is_active) volatile
        {
          control_and_status = (control_and_status & ~cs_xfer_active_mask)
                              | (is_active<<cs_xfer_active_bit);
        }

      /// @brief Set DMA enable state
      /// @param[in]  enabled Pass true to enable DMA operation and requests
      ///                     Pass false to disable DMA operations
        void set_dma_enable(bool enabled) volatile
        {
          control_and_status = (control_and_status & ~cs_dma_enable_mask)
                              | (enabled<<cs_dma_enable_bit);
        }

      /// @brief Set generate interrupt on done state
      /// @param[in]  generate Pass true to have interrupts generated when 
      ///                      DONE is true (get_transfer_done()==true)
      ///                      Pass false not to generate interrupts when DONE
      ///                      is true
        void set_interrupt_on_done(bool generate) volatile
        {
          control_and_status = (control_and_status & ~cs_int_on_done_mask)
                              | (generate<<cs_int_on_done_bit);
        }

      /// @brief Set generate interrupt on RXR state
      ///
      /// RXR is short for receive FIFO needs reading
      ///
      /// @param[in]  generate Pass true to have interrupts generated when 
      ///                      RXR is true (get_rx_fifo_needs_reading()==true)
      ///                      Pass false not to generate interrupts when RXR
      ///                      is true
        void set_interrupt_on_rxr(bool generate) volatile
        {
          control_and_status = (control_and_status & ~cs_int_on_rxr_mask)
                              | (generate<<cs_int_on_rxr_bit);
        }

      /// @brief Set automatic de-assert chip select state
      /// @param[in]  deassert  Pass true to have chip select de-asserted at
      ///                       the end of a DMA transfer
      ///                       Pass false not to automatically de-assert chip
      ///                       select at the end of a DMA transfer
        void set_auto_deassert_chip_select(bool deassert) volatile
        {
          control_and_status = (control_and_status & ~cs_auto_deassert_cs_mask)
                              | (deassert<<cs_auto_deassert_cs_bit);
        }

      /// @brief Set the read enable state for bidirectional SPI mode
      /// @param[in]  enable  Pass true indicate intention to read from a device
      ///                     Pass false to indicate intention to write to a
      ///                     device
        void set_read_enable(bool enable) volatile
        {
          control_and_status = (control_and_status & ~cs_read_enable_mask)
                              | (enable<<cs_read_enable_bit);
        }

      /// @brief Set LoSSI enable value
      ///
      /// This bit field determines the whether the SPI interface behaves as
      /// a LoSSI master or SPI master.
      ///
      /// @param[in]  enable  Pass true for the serial interface to behave as
      ///                     a LoSSI master
      ///                     Pass false for the serial interface to behave as
      ///                     an SPI master
        void set_lossi_enable(bool enable) volatile
        {
          control_and_status = (control_and_status & ~cs_lossi_enable_mask)
                              | (enable<<cs_lossi_enable_bit);
        }

      /// @brief Set DMA mode in LoSSI mode enabled state
      /// @param[in]  enable  Pass true to enable DMA mode in LoSSI mode
      ///                     Pass false to disable DMA mode in LoSSI mode
        void set_lossi_dma_enable(bool enable) volatile
        {
          control_and_status = (control_and_status & ~cs_lossi_dma_enable_mask)
                              | (enable<<cs_lossi_dma_enable_bit);
        }

      /// @brief Set LoSSI long data word mode state
      /// @param[in]  enable  Pass true to enable FIFO writing 32 bit word
      ///                     Pass false and FIFO writes single byte
        void set_lossi_long_word(bool enable) volatile
        {
          control_and_status = (control_and_status & ~cs_lossi_long_mask)
                              | (enable<<cs_lossi_long_bit);
        }


      /// @brief Write 8-bit byte to transmit FIFO
      ///
      /// Note: Only for poll / interrupt modes. DMA mode uses a DMA write
      /// channel to implicitly write data in 32-bit words to the SPI0 FIFO
      /// register.
      ///
      /// @param[in] data   8-bit byte to write to transmit FIFO.
        void transmit_fifo_write(std::uint8_t data)
        {
          fifo = data;
        }

      /// @brief Read 8-bit byte from receive FIFO
      ///
      /// Note: Only for poll / interrupt modes. DMA mode uses a DMA read
      /// channel to implicitly read data in 32-bit words from the SPI0 FIFO
      /// register.
      ///
      /// @returns 8-bit byte read from the receive FIFO
        std::uint8_t receive_fifo_read()
        {
          return fifo;
        }


      /// @brief Return currently set SPI0 clock divisor value
      ///
      /// Note: values will be even, in the range [0,65536]. A value of 
      /// 65536 represented as a register value of 0
      ///
      /// @returns SPI0 clock register divisor (CDIV) 16-bit field value
        register_t get_clock_divider()
        {
          register_t value{clock&clk_divisor_mask};
          return value?value:static_cast<register_t>(clk_divisor_max);
        }

      /// @brief Set SPI0 clock divisor value. Divides the system APB clock.
      ///
      /// Note: Values should be even, in the range [2,65536]. Odd values will
      /// be rounded down.
      ///
      /// @param[in] divisor  SPI0 APB clock divisor value [2,65536]
      /// @returns  true if operation performed,
      ///           false if operation not performed as divisor out of range
        bool set_clock_divider(register_t divisor)
        {
          if (clk_divisor_min>divisor || divisor>clk_divisor_max)
            {
              return false;
            }
          clock = divisor&clk_divisor_mask;
          return true;
        }


      /// @brief Return currently set SPI0 DMA data length value (bytes).
      ///
      /// Note: Relevant in DMA mode only (get_dma_enable()==true). Value is
      /// for both transmit and receive.
      ///
      /// @returns Number of _bytes_ to transfer in DMA operation [0,0xffff]
        register_t get_dma_data_length()
        {
          return data_length&dlen_len_mask;
        }

      /// @brief Set the SPI0 DMA data length value (bytes).
      ///
      /// Note: Valid only in DMA mode (get_dma_enable()==true). Value is
      /// for both transmit and receive.
      ///
      /// @param[in] len  SPI0 DMA data transfer length [0,65535]
      /// @returns  true if operation performed,
      ///           false if operation not performed as length out of range
        bool set_dma_data_length(register_t len)
        {
          if (/*dlen_len_min>len ||*/ len>dlen_len_max)
            {
              return false;
            }
          data_length = len&dlen_len_mask;
          return true;
        }


      /// @brief Return currently set SPI0 LoSSI mode output hold delay.
      ///
      /// Note: Relevant in LoSSI mode only (get_lossi_enable()==true). Value
      /// is the number of APB clocks to hold the output for.
      ///
      /// @returns Number of APB clocks to hold output for [1,15]
        register_t get_lossi_output_hold_delay()
        {
          return lossi_mode_toh&ltoh_toh_mask;
        }

      /// @brief Set LoSSI mode output hold delay.
      ///
      /// Note: Relevant in LoSSI mode only (get_lossi_enable()==true). Value
      /// is the number of APB clocks to hold the output for.
      ///
      /// @param[in] delay  SPI0 LoSSI mode output hold delay [1,15] APB clocks
      /// @returns  true if operation performed,
      ///           false if operation not performed as delay out of range
        bool set_lossi_output_hold_delay(register_t delay)
        {
          if (ltoh_toh_min>delay || delay>ltoh_toh_max)
            {
              return false;
            }
          lossi_mode_toh = delay&ltoh_toh_mask;
          return true;
        }


      /// @brief Return currently set SPI0 DMA write request threshold value
      ///
      /// @returns  Transmit FIFO level at or below which a DREQ signal is 
      ///           issued to the transmit DMA engine
        register_t get_dma_write_request_threshold()
        {
          return dma_controls&dc_tdreq_mask;
        }

      /// @brief Set SPI0 DMA write request threshold value
      ///
      /// @param[in] threshold  Transmit FIFO level at or below which a DREQ
      ///                       signal is issued to the transmit DMA engine
      ///                       [0..255]
      /// @returns  true if operation performed,
      ///           false if operation not performed as threshold out of range
        bool set_dma_write_request_threshold(register_t threshold)
        {
          if (/*dc_tdreq_min>threshold ||*/ threshold>dc_tdreq_max)
            {
              return false;
            }
          dma_controls = (dma_controls&~dc_tdreq_mask)|threshold;
          return true;
        }

      /// @brief Return currently set SPI0 DMA write panic threshold value
      ///
      /// @returns  Transmit FIFO level at or below which a Panic signal is 
      ///           issued to the transmit DMA engine
        register_t get_dma_write_panic_threshold()
        {
          return (dma_controls&dc_tpanic_mask)>>dc_tpanic_bit;
        }

      /// @brief Set SPI0 DMA write panic threshold value
      ///
      /// @param[in] threshold  Transmit FIFO level at or below which a Panic
      ///                       signal is issued to the transmit DMA engine
      ///                       [0..255]
      /// @returns  true if operation performed,
      ///           false if operation not performed as threshold out of range
        bool set_dma_write_panic_threshold(register_t threshold)
        {
          if (/*dc_tpanic_min>threshold ||*/ threshold>dc_tpanic_max)
            {
              return false;
            }
          dma_controls =  (dma_controls&~dc_tpanic_mask)
                        | (threshold<<dc_tpanic_bit);
          return true;
        }

      /// @brief Return currently set SPI0 DMA read request threshold value
      ///
      /// @returns  Receive FIFO level at or below which a DREQ signal is 
      ///           issued to the receive DMA engine
        register_t get_dma_read_request_threshold()
        {
          return (dma_controls&dc_rdreq_mask)>>dc_rdreq_bit;
        }

      /// @brief Set SPI0 DMA read request threshold value
      ///
      /// @param[in] threshold  Receive FIFO level at or below which a DREQ
      ///                       signal is issued to the receive DMA engine
      ///                       [0..255]
      /// @returns  true if operation performed,
      ///           false if operation not performed as threshold out of range
        bool set_dma_read_request_threshold(register_t threshold)
        {
          if (/*dc_rdreq_min>threshold ||*/ threshold>dc_rdreq_max)
            {
              return false;
            }
          dma_controls =  (dma_controls&~dc_rdreq_mask)
                        | (threshold<<dc_rdreq_bit);
          return true;
        }

      /// @brief Return currently set SPI0 DMA read panic threshold value
      ///
      /// @returns  Receive FIFO level at or below which a Panic signal is 
      ///           issued to the receive DMA engine
        register_t get_dma_read_panic_threshold()
        {
          return (dma_controls&dc_rpanic_mask)>>dc_rpanic_bit;
        }

      /// @brief Set SPI0 DMA read panic threshold value
      ///
      /// @param[in] threshold  Receive FIFO level at or below which a Panic
      ///                       signal is issued to the receive DMA engine
      ///                       [0..255]
      /// @returns  true if operation performed,
      ///           false if operation not performed as threshold out of range
        bool set_dma_read_panic_threshold(register_t threshold)
        {
          if (/*dc_rpanic_min>threshold ||*/ threshold>dc_rpanic_max)
            {
              return false;
            }
          dma_controls =  (dma_controls&~dc_rpanic_mask)
                        | (threshold<<dc_rpanic_bit);
          return true;
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_REGISTERS_H
