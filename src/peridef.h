// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file peridef.h 
/// @brief \b Internal : common BCM2835 peripheral declarations and definitions
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PERIDEF_H
# define DIBASE_RPI_PERIPHERALS_PERIDEF_H

# include <sys/types.h>
# include <cstdint>
# include <limits>

/// @brief Namespace for all Dibase / R. E. McArdell entities
namespace dibase 
{
/// @brief Namespace containing Raspberry Pi specific entities
namespace rpi 
{
/// @brief Namespace containing Raspberry Pi peripheral library entities
  namespace peripherals
  {
  /// @brief Represents physical addresses. These will be offsets into /dev/mem.
    typedef off_t         physical_address_t;

  /// @brief Represents BCM2835 peripheral control registers
    typedef std::uint32_t register_t;

  /// @brief Number of bits in a BCM2835 peripheral control register.
    std::size_t const register_width{std::numeric_limits<register_t>::digits};

  /// @brief BCM2835 peripheral control block byte size as a page size multiple.
    std::size_t const register_block_size{4096};

  /// @brief Physical address of BCM2835 peripheral control blocks.
    physical_address_t const peripheral_base_address{0x20000000};
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_PERIDEF_H
