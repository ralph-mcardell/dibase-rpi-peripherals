// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_info.h 
/// @brief Raspberry Pi platform information : class definition
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_RPI_INFO_H
 #define DIBASE_RPI_RPI_INFO_H

 #include <cstdlib>

 namespace dibase {
  namespace rpi
  {
  /// @brief Provides Raspberry Pi platform information
  ///
  /// Raspberry Pi platform information is presented by rpi_info from
  /// information gathered from the system - e.g. data available in the
  /// Linux proc file system.
  ///
  /// Note: In general this information is static so once gathered by an
  ///       initial instance is usually cached for subsequent instances.
    class rpi_info
    {
    public:
    /// @brief Default construct: initialises Raspberry Pi information values.
      rpi_info();

    /// @brief Returns 1 based integer major version of Raspberry Pi board
    ///
    /// As of December 2012 there were only 2 major versions of a Raspberry Pi
    /// board. Rev. 1 for hardware revisions <= 3, revision 2 for hardware
    /// revisions > 3 (4,5,6, ...) as indicated by /proc/cpuinfo Revision.
    /// @return 1 for hardware revisions <=3, 2 otherwise.
      std::size_t major_version()
      {
        return major_version_value;
      }

    /// @brief Returns 0 based integer major version of Raspberry Pi board
    ///
    /// @return rpi_info::major_version() - 1 so that the value may be easily
    /// used as an index into a 0 based array.
      std::size_t index_version()
      {
        return major_version_value - 1;
      }

    private:
      std::size_t   major_version_value;
    };
  }
 }
#endif // DIBASE_RPI_RPI_INFO_H
