// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file spi0_ctrl.h 
/// @brief \b Internal : SPI0 control type & supporting definitions.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_CTRL_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_CTRL_H

# include "phymem_ptr.h"
# include "spi0_registers.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief SPI0 control type. There is only 1 (yes it's a singleton!)
    ///
    /// Maps BCM2708 / 2835 SPI0 registers into the requisite physical memory
    /// mapped area and provides a simple allocated flag for in-process SPI0
    /// use tracking.
    ///
    /// Note that not only is there only one control area, the area controls
    /// only one SPI channel.
      struct spi0_ctrl
      {
      /// @brief Pointer to BCM2708 / BCM2835 SPI0 control registers instance
        phymem_ptr<volatile spi0_registers>        regs;

      /// @brief SPI0 channel allocation flag
        bool  allocated;

      /// @brief Singleton instance getter
      /// @returns \e The instance of the SPI0 control object.
        static spi0_ctrl & instance();

      private:
        spi0_ctrl();

        spi0_ctrl(spi0_ctrl const &) = delete;
        spi0_ctrl(spi0_ctrl &&) = delete;
        spi0_ctrl & operator=(spi0_ctrl const &) = delete;
        spi0_ctrl & operator=(spi0_ctrl &&) = delete;
      };
   } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SPI0_CTRL_H
