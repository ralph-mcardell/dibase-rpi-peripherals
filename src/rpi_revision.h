// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_revision.h 
/// @brief Raspberry Pi revision value abstraction : class definition
///
/// @copyright Copyright (c) Dibase Limited 2015
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_RPI_REVISION_H
 #define DIBASE_RPI_RPI_REVISION_H

 #include <cstdlib>

 namespace dibase {
  namespace rpi
  {
    enum class rpi_model
    { a
    , b
    , a_plus
    , b_plus
    , pi_2_b
    , alpha
    , compute_module
    };

    enum class rpi_maker
    { sony
    , egoman
    , embest
    , unknown
    , embest2
    , qisda
    };

    enum class rpi_processor
    { bcm2835
    , bcm2836
    };

    enum class rpi_ram
    { mb256
    , mb512
    , mb1024
    };

    class rpi_revision
    {
      std::size_t revision;

    public:
      explicit rpi_revision(size_t rev);
      rpi_revision
      ( rpi_model model
      , rpi_processor processor
      , rpi_ram ram
      , rpi_maker maker
      , unsigned int version // 0..15
      , bool turbo = false
      , bool warranty_void = false
      );
      
      rpi_model model();
      rpi_processor processor();
      rpi_ram ram();
      unsigned int ram_MB();
      unsigned int ram_B() { return ram_MB()*1024*1024; }
      rpi_maker maker();
      unsigned int version();
      bool turbo();
      bool warranty_void();
      std::size_t raw_value() { return revision; }
    };
  }
 }
 #endif // DIBASE_RPI_RPI_REVISION_H
 