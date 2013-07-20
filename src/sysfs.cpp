// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs.cpp 
/// @brief Linux sys filesystem utilities.
///
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "sysfs.h"
#include <string>
#include <sstream>
#include <fstream>
#include <system_error>
#include <unistd.h>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
      namespace
      {
        char const * sysfs_gpio_export_pathname{"/sys/class/gpio/export"};
        char const * sysfs_gpio_unexport_pathname{"/sys/class/gpio/unexport"};
        char const * sysfs_gpio_pin_dir_basename{"/sys/class/gpio/gpio"};
        
        static std::string make_gpio_pin_dir_pathname(pin_id pin)
        {
          std::ostringstream ostrstrm;
          ostrstrm << sysfs_gpio_pin_dir_basename << pin;
          return ostrstrm.str();
        }

        static bool write_pin_id_to_file( pin_id pin, char const * path)
        {
          std::ofstream pin_id_stream(path);
          if (pin_id_stream.is_open())
            {
              pin_id_stream << pin;
              return  pin_id_stream.good();
            }
          return false;
        }
      }

      bool is_exported(pin_id pin)
      {
        bool in_use{access(make_gpio_pin_dir_pathname(pin).c_str(), F_OK)==0};
        if (!in_use&&errno!=ENOENT)
          {
            throw std::system_error
                  ( errno
                  , std::system_category()
                  , "Existence check for sys filesystem exported pin directory"
                    " failed: unexpected error from call to access."
                  );
          }
        return in_use;
      }

      bool export_pin(pin_id pin)
      {
        return write_pin_id_to_file(pin, sysfs_gpio_export_pathname);
      }

      bool unexport_pin(pin_id pin)
      {
        return write_pin_id_to_file(pin, sysfs_gpio_unexport_pathname);
      }
    }
  }
}} 
