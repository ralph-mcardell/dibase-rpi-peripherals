// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc.cpp
/// @brief Implementation of GPIO pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "pin_alloc.h"
#include "sysfs.h"
#include <stdexcept>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      bool pin_export_allocator::is_in_use( pin_id pin )
      {
        return internal::is_exported(pin);
      }

      void pin_export_allocator::allocate( pin_id pin )
      {
        if (internal::is_exported(pin))
          {
            throw bad_peripheral_alloc{"GPIO pin allocate: "
                                       "pin is in use by another process"
                                      };
          }
        if (!internal::export_pin(pin))
          {
            throw std::runtime_error
                  {"GPIO pin allocate: Unable to open sys fs "
                   "export file to export GPIO pin for use."
                  };
          }
      }

      void pin_export_allocator::deallocate( pin_id pin )
      {
        if (!internal::is_exported(pin))
          {
            throw std::runtime_error( "GPIO pin deallocate: pin is NOT in use! "
                                      "Was it unexported by another process?"
                                    );
          }
        if (!internal::unexport_pin(pin))
          {
            throw std::runtime_error
                  {"GPIO pin deallocate: Unable to open sys fs "
                   "unexport file to unexport GPIO pin from use."
                  };
          }
      }
    }
  }
}} 
