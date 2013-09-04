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

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Represents layout of SPI0 control registers with operations.
    ///
    /// Permits access to BCM2835 SPI master SPI0 interface registers when an 
    /// instance is mapped to the correct physical memory location.
    ///
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 10 for published
    /// details. Note that there are two chip select polarity control field
    /// sets:
    ///
    /// - CS register CSPOL field, sets the polarity of all chip select lines
    /// - CS register CSPOL0, CSPOL1 and CSPOL2 fields set the polarity for
    /// individual control lines.
    ///
    /// There is no definitive information on how these 2 sets of fields
    /// interact given in the data sheet but reference to existing code
    /// (specifically the Linux BCM2835 SPI driver code in spi-bcm2835.c)
    /// indicates that both CSPOL and one CSPOL{0,1,2] depending on which
    /// chip is selected should be set to the required polarity.
    ///
    /// Member function operations are provided to query and set the various
    /// fields and flags for SPI0 channel control.
      struct spi0_registers
      {
      private:
        enum 
        { cs_max_chip_id  = 2         // Maximum chip identifier value
        , cs_chip_select_mask = 3     // CS register chip select field bit-mask
        , cs_clock_phase_mask = 4     // CS register clock phase field bit-mask
        , cs_clock_polarity_mask = 8  // CS register clock polarity field bit-mask
        , cs_cs_polarity_mask = 0x40  // CS register chip select polarity field bit-mask
        , cs_xfer_active_mask = 0x80  // CS register transfer active field bit-mask
        , cs_dma_enable_mask = 0x100  // CS register DMA enable field bit-mask
        , cs_int_on_done_mask =0x200  // CS register interrupt on done field bit-mask
        , cs_int_on_rxr_mask = 0x400  // CS register interrupt on RXR field bit-mask
        , cs_auto_deassert_cs_mask=0x800// CS register ADCS field bit-mask
        , cs_read_enable_mask=0x1000  // CS register read enable field bit-mask
        , cs_lossi_enable_mask=0x2000 // CS register LoSSI enable field bit-mask
        , cs_xfer_done_mask = 0x10000 // CS register transfer done field bit-mask
        , cs_rxd_mask       = 0x20000 // CS register RX FIFO has data field bit-mask
        , cs_txd_mask       = 0x40000 // CS register TX FIFO has space field bit-mask
        , cs_rxr_mask       = 0x80000 // CS register RX FIFO needs reading field bit-mask
        , cs_rxf_mask       = 0x100000// CS register RX FIFO full field bit-mask
        , cs_lossi_dma_enable_mask=0x1000000// CS register LoSSI DMA mode enable field bit-mask
        , cs_lossi_long_mask=0x2000000// CS register LoSSI long word field bit-mask
        , cs_csline_polarity_base_mask=0x200000// CS register CSPOL0 field bit-mask
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
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_REGISTERS_H