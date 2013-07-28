// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs.cpp 
/// @brief Linux sys filesystem utilities.
///
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "sysfs.h"
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <fcntl.h>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace internal
    {
      namespace
      {
        char const * gpio_export_pathname{"/sys/class/gpio/export"};
        char const * gpio_unexport_pathname{"/sys/class/gpio/unexport"};
        char const * gpio_pin_dir_basename{"/sys/class/gpio/gpio"};
        char const * gpio_pin_edgemode_filename{"edge"};
        char const * gpio_pin_value_filename{"value"};
        
        static std::string make_gpio_pin_dir_pathname(pin_id pin)
        {
          std::ostringstream ostrstrm;
          ostrstrm << gpio_pin_dir_basename << pin;
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

        std::string event_mode_to_edge_file_string(edge_event_mode mode)
        {
          switch (mode)
          {
          case edge_event_mode::rising:   return "rising";
          case edge_event_mode::falling:  return "falling";
          case edge_event_mode::both:     return "both";
          default: return "";
          };
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
        return write_pin_id_to_file(pin, gpio_export_pathname);
      }

      bool unexport_pin(pin_id pin)
      {
        return write_pin_id_to_file(pin, gpio_unexport_pathname);
      }

      int open_ipin_for_edge_events(pin_id pin, edge_event_mode mode)
      {
        std::string pin_path{make_gpio_pin_dir_pathname(pin)};
        pin_path += '/';
        {
          std::fstream edgestream
                        {pin_path+gpio_pin_edgemode_filename, std::ios::out};
          if (edgestream.is_open())
            {
              edgestream.exceptions(std::ios::badbit|std::ios::failbit);
              auto edge_file_value(event_mode_to_edge_file_string(mode));
              if (edge_file_value.empty())
                {
                  throw std::invalid_argument{"Bad edge_event_mode value."};
                }
              edgestream << edge_file_value;
            }
          else
            {
              throw std::runtime_error{"Open failed for pin sys fs edge file."};
            }
        }
        int fd{::open((pin_path+gpio_pin_value_filename).c_str(), O_RDONLY)};
        if (fd==-1) 
          {
             throw std::system_error
                  ( errno
                  , std::system_category()
                  , "Failed to obtain file descriptor to monitor to pin edge "
                    "events: error from call to open."
                  );
          }
        return fd;
      }

      bool close_ipin_for_edge_events(int pin_fd)
      {
        return ::close(pin_fd)==0;
      }
    }
  }
}} 
