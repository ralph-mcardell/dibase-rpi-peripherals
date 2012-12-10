// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc.h 
/// @brief Specification of GPIO pin allocation types.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell


#ifndef DIBASE_RPI_PERIPHERALS_PIN_ALLOC_H
 #define DIBASE_RPI_PERIPHERALS_PIN_ALLOC_H

 #include "pin_id.h"
 #include <stdexcept>
 #include <cstring>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    struct bad_pin_alloc : std::runtime_error
    {
      bad_pin_alloc( char const * what )
      : std::runtime_error( what )
      {}
    };

  /// @brief Passes (de-)allocation requests to other allocator & caches results
  ///
  /// Class template taking another pin allocator type parameter which is
  /// assumed to implement allocate, deallocate and is_in_use methods and whose
  /// instances are default constructable.
  ///
  /// GPIO pin allocation and deallocation requests are first checked against
  /// cached results local to the pin_cache_allocator specialisation instance
  /// and if the cached value indicates a pin is free for allocation or in use
  /// and may be deallocated the request is passed to the allocate or
  /// deallocate member function of the contained allocator and the cache
  /// updated as appropriate. The is_in_use query member function first checks
  /// the local cache and, if _not_ locally in use, passes the query onto the
  /// contained allocator.
  ///
  /// Note: the rationale behind splitting pin allocation into a local caching
  /// part with a parametersable alternate part is to allow unit testing of the
  /// caching functionality using simple mock contained allocator types.
  ///
  /// @param PIN_ALLOC_T  Type of pin allocator to pass requests onto if 
  ///                     cached results indicate so.
    template <class PIN_ALLOC_T>
    class pin_cache_allocator
    {
      bool pat[pin_id::number_of_pins]; ///< GPIO Pin Availability Table
      PIN_ALLOC_T allocator;            ///< Contained pin allocator

    public:
      pin_cache_allocator()
      {
        std::memset(pat, 0, sizeof(pat));
      }

    /// @brief allocate a GPIO pin for use
    ///
    /// If a pin has already been allocated using this allocator then throws
    /// a bad_pin_alloc exception, otherwise passes the pin_id to the allocate
    /// member function of the contained allocator, which is assumed to also
    /// throw on allocation failure. If the call returns then the pin is marked
    /// as in use in the per-instance Pin Allocation Table.
    ///
    /// @param[in] pin    GPIO pin id for the GPIO pin to allocate for use.
    /// @exception  bad_pin_alloc is raised if the requested pin is already in
    ///             use.
      void allocate( pin_id pin )
      {
        if (pat[pin])
          {
            throw bad_pin_alloc( "GPIO pin allocate: pin is already being "
                                 "used locally."
                               );
          }
        allocator.allocate( pin );
        pat[pin] = true;
      }

    /// @brief deallocate a GPIO pin from use
    ///
    /// If a pin has not been allocated using this allocator then throws a
    /// std::logic_error exception, otherwise passes the pin_id to the
    /// deallocate member function of the contained allocator, which is assumed
    /// to also throw on deallocation failure. If the call returns then the pin
    /// is marked as free in the per-instance Pin Allocation Table.
    ///
    /// @param[in] pin    GPIO pin id for the GPIO pin to deallocate from use.
    /// @exception  std::logic_error is raised if the requested pin is not in
    ///             use.
      void deallocate( pin_id pin )
      {
        if (!pat[pin])
          {
            throw std::logic_error( "GPIO pin deallocate: pin is not in use "
                                    "locally."
                                  );
          }
        allocator.deallocate( pin );
        pat[pin] = false;
      }

    /// @brief Returns whether a GPIO pin is availble _now_ for use
    ///
    /// If the pin is marked in use in the per-instance Pin Allocation Table
    /// returns true, otherwise returns the value returned from passing the
    /// pin_id to the conatined allocator's is_in_use member function.
    ///
    /// @param[in] pin    GPIO pin id of GPIO pin to see if it is in use.
    /// @return true if the pin is in use at this moment or false otherwise
      bool is_in_use( pin_id pin )
      {
        return pin[pat] ? true : allocator.is_in_use( pin );
      }
    };
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_PIN_ALLOC_H

