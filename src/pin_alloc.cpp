// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc.cpp
/// @brief Implementation of GPIO pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin_alloc.h"
#include <string>
#include <sstream>
#include <fstream>
#include <system_error>
#include <stdexcept>
#include <unistd.h>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace
    {
      char const * sysfs_gpio_export_pathname{"/sys/class/gpio/export"};
      char const * sysfs_gpio_unexport_pathname{"/sys/class/gpio/unexport"};
      char const * sysfs_gpio_pin_dir_basename{"/sys/class/gpio/gpio"};
      
      static std::string make_gpio_pin_dir_pathname( pin_id pin )
      {
        std::ostringstream ostrstrm;
        ostrstrm << sysfs_gpio_pin_dir_basename << pin;
        return ostrstrm.str();
      }

      static void write_pin_id_to_file
      ( pin_id pin
      , char const * path
      , char const * fail_msg
      )
      {
        std::ofstream pin_id_stream(path);
        if (pin_id_stream.is_open())
          {
            pin_id_stream << pin;
          }
        else
          {
            throw std::runtime_error( fail_msg );
          }
      }
    }

    bool pin_export_allocator::is_in_use( pin_id pin )
    {
      bool in_use{access(make_gpio_pin_dir_pathname(pin).c_str(), F_OK)==0};
      if (!in_use&&errno!=ENOENT)
        {
          throw std::system_error
                ( errno
                , std::system_category()
                , "pin_export_allocator::is_in_use: Existence check "
                  "call to access failed with unexpected error."
                );
        }
      return in_use;
    }

    void pin_export_allocator::allocate( pin_id pin )
    {
      if ( is_in_use(pin) )
        {
          throw bad_pin_alloc(  "GPIO pin allocate: "
                                "pin is in use by another process"
                             );
        }
      write_pin_id_to_file
      ( pin
      , sysfs_gpio_export_pathname
      , "GPIO pin allocate: "
        "Unable to open sys fs export file to export GPIO pin for use."
      );
    }

    void pin_export_allocator::deallocate( pin_id pin )
    {
      if ( ! is_in_use(pin) )
        {
          throw std::runtime_error( "GPIO pin deallocate: pin is NOT in use! "
                                    "Was it unexported by another process?"
                                  );
        }
      write_pin_id_to_file
      ( pin
      , sysfs_gpio_unexport_pathname
      , "GPIO pin deallocate: "
        "Unable to open sys fs unexport file to unexport GPIO pin from use."
      );
    }
  }
}} 
