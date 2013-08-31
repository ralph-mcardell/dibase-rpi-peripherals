// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_init.h 
/// @brief \b Internal : rpi_info initialisation : class definition
///
/// Only used internally and by tests
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_RPI_INTERNAL_INIT_H
 #define DIBASE_RPI_RPI_INTERNAL_INIT_H

 #include <cstdlib>

 namespace dibase { namespace rpi {
 /// @brief Namespace containing Raspberry Pi specific internal details
 namespace internal
 {
 /// @brief Overridable Raspberry Pi information initialisation member functions
 ///
 /// Raspberry Pi platform information is presented by the rpi_info class.
 /// The information in that class will be gathered from the system - such
 /// as data available in the Linux proc file system. However, there is a
 /// need to allow the gathering of such information to be overridden
 /// with mock implementations for testing purposes. This requirement is
 /// provided by the virtual member functions of rpi_init.
    struct rpi_init
    {
    /// @brief Returns 1 based integer major version of Raspberry Pi board
    ///
    /// As of December 2012 there were only 2 major versions of a Raspberry Pi
    /// board. Rev. 1 for hardware revisions <= 3, revision 2 for hardware
    /// revisions > 3 (4,5,6) as indicated by /proc/cpuinfo Revision.
    /// @return 1 for hardware revisions <=3, 2 otherwise.
      virtual std::size_t init_major_version();
     };
  }
}}
#endif // DIBASE_RPI_RPI_INTERNAL_INIT_H
