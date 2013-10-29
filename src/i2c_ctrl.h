// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_ctrl.h 
/// @brief \b Internal GPIO I2C (BSC) control type & supporting definitions.
///
/// Note: Broadcom refers to its I2C supporting peripherals as BSC (Broadcom
/// Serial Controllers).
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_CTRL_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_CTRL_H

# include "phymem_ptr.h"
# include "i2c_registers.h"
# include "simple_allocator.h"
# include <cstddef>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Number of BSC (I2C) master peripherals supported by the BCM2835
    ///  Note though that BSC2 is reserved for use with the HDMI device.
      constexpr std::size_t number_of_bsc_masters{3};

    /// @brief I2C control type. There is only 1 (yes it's a singleton!)
    ///
    /// Maps BCM2708/2835 BSC peripherals' registers into the requisite physical
    /// memory mapped area, provides simple allocator for in-process resource
    /// use tracking.
      struct i2c_ctrl
      {
      /// @brief Type alias for (smart) pointers to BSC master control blocks
        typedef phymem_ptr<volatile i2c_registers>  reg_ptr;

      /// @brief Function returning (smart) pointer to BSC control registers
      ///
      /// Note: Register memory is only mapped into a process' memory on the
      ///       first access request for a specific BSC master's registers.
      ///
      /// @param[in] idx    Index of the BSC master peripheral to return 
      ///                   pointer to control register block to: 0, 1 or 2.
      ///                   The value is NOT range checked.
      /// @returns Smart pointer to mapped block of physical memory of
      ///          BSC (I2C) master control memory mapped registers.
        reg_ptr & regs(std::size_t idx);

      /// @brief I2C BSC master peripheral allocator instance
        simple_allocator<number_of_bsc_masters>  alloc;

      /// @brief Singleton instance getter
      /// @returns THE instance of the I2C control object.
        static i2c_ctrl & instance();

      private:
      /// @brief Pointer to BCM2708 / BCM2835 BSC control registers instances
        reg_ptr register_blocks[number_of_bsc_masters];

      /// @brief Construct: do default initialisation only at this time
        i2c_ctrl() = default;

        i2c_ctrl(i2c_ctrl const &) = delete;
        i2c_ctrl(i2c_ctrl &&) = delete;
        i2c_ctrl & operator=(i2c_ctrl const &) = delete;
        i2c_ctrl & operator=(i2c_ctrl &&) = delete;
      };
   } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_I2C_CTRL_H
