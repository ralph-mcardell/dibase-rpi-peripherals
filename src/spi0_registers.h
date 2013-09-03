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
      struct spi0_registers
      {
      private:
        enum 
        {
        };

      public:
      /// @brief Physical address of start of BCM2835 SPI0 control registers
        constexpr static physical_address_t 
                            physical_address = peripheral_base_address+0x204000;

        regsiter_t  control_and_status; ///< SPI Master Control and Status, CS
        register_t  fifo;               ///< SPI Master TX and RX FIFOs, FIFO
        register_t  clock;              ///< SPI Master Clock Divider, CLK
        register_t  data_length         ///< SPI Master Data Length, DLEN
        register_t  lossi_mode_toh      ///< SPI LOSSI mode TOH, LTOH
        register_t  dma_controls;       ///< SPI DMA DREQ Controls, DC
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_REGISTERS_H
