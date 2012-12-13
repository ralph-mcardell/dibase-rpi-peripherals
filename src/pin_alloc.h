// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc.h 
/// @brief Specification of GPIO pin allocation types.
///
/// GPIO pins may be considered a resource access to which will usually be
/// exclusive to one use in one process at a time with the possible exception
/// of reading of GPIO pins in input mode. Many GPIO pins have alternate
/// functions and the overloaded relationships between functions and GPIO
/// pins could lead users to accidentially over-subscribe a pin by mistake.
///
/// Additionally, other processes might be running that use some GPIO pins.
/// Unfortunately there is no fool-proof way to determine if this is so or
/// prevent other processes from access GPIO pins supposedly in use by
/// another process.
///
/// The pin allocation types try to tackle both the problem of requesting the
/// same pin for more than one use within a process and prevent using GPIO
/// pins that might be in use by other processes. The former relies on noting
/// successfully allocated pins as in use and then as not in use when
/// deallocated.
///
/// The more fuzzy problem of determining if a GPIO pin is being used by
/// another process is only a partial solution. A GPIO pin is deemed to be in
/// use if it has been previously exported in the Linux sys file system 
/// (see /sys/classes/gpio). If is not then it is exported so any other
/// processes using the same assumption will not try to also use the GPIO pin.
/// As mentioned this is by no means a watertight solution and may be
/// revisited in ther future.
///
/// To alloe testing the intra-process pin allocation logic as a unit the
/// implementation is split into two parts:
///   - a class template that implements the intra-process allocation logic
///   - a separate allocation class that performs the inter-process sys file
///     system chacking which is used as a type passed to the intra-process
///     allocator template to provide the inter-process logic.
///
/// This allows the intra-process allocation logic to be tested on its own
/// by mocking the inter-process allocator class. 
///
/// GPIO pin allocator types should provide the following three functions:
///
///   void allocate( pin_id pin )   : allocate a pin or throw a bad_pin_alloc
///                                   exception if the pin is in use.
///   void deallocate( pin_id pin ) : deallocate a previously allocated pin
///                                   or thow a std::logic error if the pin is
///                                   not allocated.
///   bool is_in_use( pin_id pin )  : Returns true if the pin is in use at the
///                                   time of the query, false if it is not.
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
  /// @brief Exception type raised if a pin cannot be allcoated.
    struct bad_pin_alloc : std::runtime_error
    {
    /// @brief Construct from an explainatory C string.
    /// @param[in]  what  C string expaining allocation failure
      bad_pin_alloc( char const * what )
      : std::runtime_error( what )
      {}
    };

  /// @brief Passes (de-)allocation requests to other allocator & caches results
  ///
  /// Class template providing intra-process allocation logic and taking 
  /// a pin allocator type parameter used to provide the inter-process pin
  /// allocation logic.
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
    /// @param[in]  pin   GPIO pin id for the GPIO pin to allocate for use.
    /// @exception  bad_pin_alloc is raised if the requested pin is already in
    ///             use.
    /// @exception  std::runtime_error (or other exception) is rasied if
    ///             a passed on allocation request should fail.
    void allocate( pin_id pin );

    /// @brief deallocate a GPIO pin from use
    ///
    /// If a pin has not been allocated using this allocator then throws a
    /// std::logic_error exception, otherwise passes the pin_id to the
    /// deallocate member function of the contained allocator, which is assumed
    /// to also throw on deallocation failure. If the call returns then the pin
    /// is marked as free in the per-instance Pin Allocation Table.
    ///
    /// @param[in]  pin   GPIO pin id for the GPIO pin to deallocate from use.
    /// @exception  std::logic_error is raised if the requested pin is not in
    ///             use locally by this process.
    /// @exception  std::runtime_error (or other exception) is raised if a
    ///             passed on deallocation request should fail or an attempt
    ///             is made to deallocate a pin the contained allocator deems 
    ///             free - which is probably not due to a logic error in the
    ///             current process' program.
      void deallocate( pin_id pin );

    /// @brief Returns whether a GPIO pin is availble _now_ for use
    ///
    /// If the pin is marked in use in the per-instance Pin Allocation Table
    /// returns true, otherwise returns the value returned from passing the
    /// pin_id to the contained allocator's is_in_use member function.
    ///
    /// @param[in]  pin     GPIO pin id of GPIO pin to see if it is in use.
    /// @return true if the pin is in use at this moment or false otherwise
    /// @exception  std::system_error (or other exception) is raised if a
    ///             passed on  request to the contained allocator should fail.
      bool is_in_use( pin_id pin )
      {
        return pat[pin] ? true : allocator.is_in_use( pin );
      }
    };

    template <class PIN_ALLOC_T>
    void pin_cache_allocator<PIN_ALLOC_T>::allocate( pin_id pin )
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

    template <class PIN_ALLOC_T>
    void pin_cache_allocator<PIN_ALLOC_T>::deallocate( pin_id pin )
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

  /// @brief Allocator using sys filesystem gpio export / unexport for allocation
    class pin_export_allocator
    {
    public:
    /// @brief Determines if a GPIO pin is in use possibly externally
    ///
    /// Tries to determine if a GPIO pin is in use external to the current
    /// process by seeing if it is exported in the sys filesysten GPIO support
    /// area.
    ///
    /// Note that this is not an airtight check as proceses can use GPIO
    /// by methods other than the user space sys filesystem and so are not
    /// required to export the GPIO pins they use in teh sys filesystem. Or
    /// may export/unexport on an operation by operation application basis.
    ///
    /// @param[in]  pin     GPIO pin id of GPIO pin to see if it is in use.
    /// @return true if the pin is in use at this moment or false otherwise
    /// @exception  std::system_error is raised if an unexpected error is
    ///             returned from a system call.
      bool is_in_use( pin_id pin );

    /// @brief Allocates a GPIO pin by exporting it in the sys filesystem
    ///
    /// @param[in]  pin   GPIO pin id for the GPIO pin to allocate for use.
    /// @exception  bad_pin_alloc is raised if the requested pin is already
    ///             exported.
    /// @exception  std::runtime_error is raised if the export file cannot
    ///             be opened.
      void allocate( pin_id pin );

    /// @brief Deallocates a GPIO pin by unexporting it in the sys filesystem
    ///
    /// @param[in]  pin   GPIO pin id for the GPIO pin to deallocate from use.
    /// @exception  std::runtime_error is raised if requested pin is not
    ///             exported or the unexport file could not be opened.
      void deallocate( pin_id pin );
    };
  }
}} 
#endif // DIBASE_RPI_PERIPHERALS_PIN_ALLOC_H

