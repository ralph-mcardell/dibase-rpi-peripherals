// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_alloc.h 
/// @brief \b Internal : GPIO pin allocation : type definitions
///
/// GPIO pins may be considered a resource access to which will usually be
/// exclusive to one use in one process at a time with the possible exception
/// of reading of GPIO pins in input mode. Many GPIO pins have alternate
/// functions and the overloaded relationships between functions and GPIO
/// pins could lead users to accidentally over-subscribe a pin by mistake.
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
/// (see /sys/classes/gpio). If it is not then it is exported so any other
/// processes using the same assumption will not try to also use the GPIO pin.
/// As mentioned this is by no means a watertight solution and may be
/// revisited in the future.
///
/// To allow testing the intra-process pin allocation logic as a unit the
/// implementation is split into two parts:
///   - a class template that implements the intra-process allocation logic
///   - a separate allocation class that performs the inter-process sys file
///     system checking which is used as a type passed to the intra-process
///     allocator template to provide the inter-process logic.
///
/// This allows the intra-process allocation logic to be tested on its own
/// by mocking the inter-process allocator class. 
///
/// GPIO pin allocator types should provide the following three functions:
///
///   void allocate( pin_id pin )   : allocate a pin or throw a
///                                   bad_peripheral_alloc exception if the pin
///                                   is in use. Inter process pin allocators
///                                   may throw other exceptions such as
///                                   std::runtime_error.\n
///   void deallocate( pin_id pin ) : deallocate a previously allocated pin.
///                                   Throw a std::logic error if the pin is
///                                   not allocated locally or some other 
///                                   exception such as std::runtime_error if 
///                                   a pin is not allocated globally or its
///                                   status cannot be determined due to some
///                                   unexpected condition.\n
///   bool is_in_use( pin_id pin )  : Returns true if the pin is in use at the
///                                   time of the query, false if it is not.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_PIN_ALLOC_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_PIN_ALLOC_H

# include "pin_id.h"
# include "periexcept.h"
# include "simple_allocator.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Passes allocator requests to other allocator & caches results
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
    /// @tparam PinAllocT Type of pin allocator to pass requests onto if 
    ///                   cached results indicate so. Dictates inter-process
    ///                   pin allocation policy.
      template <class PinAllocT>
      class pin_cache_allocator
      {
        simple_allocator<pin_id::number_of_pins> cache_alloc;
        PinAllocT allocator;

      public:
      /// @brief Construct with all GPIO pins potentially available (they may
      /// not be free by the standards of the inter-process allocator
      /// specified by PinAllocT).
        pin_cache_allocator() = default;

      /// @brief Allocate a GPIO pin for use
      ///
      /// If a pin has already been allocated using this allocator then throws
      /// a bad_peripheral_alloc exception, otherwise passes the pin_id to the
      /// allocate member function of the contained allocator, which is assumed
      /// to also throw on allocation failure. If the call returns then the pin
      /// is marked as in use in the per-instance Pin Allocation Table.
      ///
      /// @param[in]  pin   GPIO pin id for the GPIO pin to allocate for use.
      /// @exception  bad_peripheral_alloc is raised if the requested pin is
      ///             already in use.
      /// @exception  std::runtime_error (or other exception) is raised if
      ///             a passed on allocation request should fail.
      void allocate(pin_id pin);

      /// @brief Deallocate a GPIO pin from use
      ///
      /// If a pin has not been allocated using this allocator then throws a
      /// std::logic_error exception, otherwise passes the pin_id to the
      /// deallocate member function of the contained allocator, which is also
      /// assumed to throw on deallocation failure. If the call returns then
      /// the pin is marked as free in the per-instance Pin Allocation Table.
      ///
      /// @param[in]  pin   GPIO pin id for the GPIO pin to deallocate from use.
      /// @exception  std::logic_error is raised if the requested pin is not in
      ///             use locally by this process.
      /// @exception  std::runtime_error (or other exception) is raised if a
      ///             passed on deallocation request should fail or an attempt
      ///             is made to deallocate a pin the contained allocator deems 
      ///             free - which is probably not due to a logic error in the
      ///             current process' program.
        void deallocate(pin_id pin);

      /// @brief Returns whether a GPIO pin is available _now_ for use
      ///
      /// If the pin is marked in use in the per-instance Pin Allocation Table
      /// returns true, otherwise returns the value returned from passing the
      /// pin_id to the contained allocator's is_in_use member function.
      ///
      /// @param[in]  pin     GPIO pin id of GPIO pin to see if it is in use.
      /// @return true if the pin is in use at this moment or false otherwise
      /// @exception  std::system_error (or other exception) is raised if a
      ///             passed on request to the contained allocator should fail.
        bool is_in_use(pin_id pin)
        {
          return cache_alloc.is_in_use(pin) ? true : allocator.is_in_use(pin);
        }
      };

      template <class PinAllocT>
      void pin_cache_allocator<PinAllocT>::allocate(pin_id pin)
      {
        if (cache_alloc.is_in_use(pin))
          {
            throw bad_peripheral_alloc( "GPIO pin allocate: pin is already "
                                        "being used locally."
                                      );
          }
        allocator.allocate(pin);
        cache_alloc.allocate(pin);
      }

      template <class PinAllocT>
      void pin_cache_allocator<PinAllocT>::deallocate(pin_id pin)
      {
        if (!cache_alloc.is_in_use(pin))
          {
            throw std::logic_error( "GPIO pin deallocate: pin is not in use "
                                    "locally."
                                  );
          }
        allocator.deallocate(pin);
        cache_alloc.deallocate(pin);
      }

    /// @brief Allocator using sys filesystem gpio export/unexport for allocation
      class pin_export_allocator
      {
      public:
      /// @brief Determines if a GPIO pin is in use possibly externally
      ///
      /// Tries to determine if a GPIO pin is in use external to the current
      /// process by seeing if it is exported in the sys filesystem GPIO support
      /// area.
      ///
      /// Note that this is not an airtight check as proceses can use GPIO
      /// by methods other than the user space sys filesystem and so are not
      /// required to export the GPIO pins they use in the sys filesystem. Or
      /// may export/unexport on an operation by operation basis.
      ///
      /// @param[in]  pin     GPIO pin id of GPIO pin to see if it is in use.
      /// @return true if the pin is in use at this moment or false otherwise
      /// @exception  std::system_error is raised if an unexpected error is
      ///             returned from a system call.
        bool is_in_use(pin_id pin);

      /// @brief Allocates a GPIO pin by exporting it in the sys filesystem
      ///
      /// @param[in]  pin   GPIO pin id for the GPIO pin to allocate for use.
      /// @exception  bad_peripheral_alloc is raised if the requested pin is
      ///             already exported.
      /// @exception  std::runtime_error is raised if the export file cannot
      ///             be opened.
        void allocate(pin_id pin);

      /// @brief Deallocates a GPIO pin by unexporting it in the sys filesystem
      ///
      /// @param[in]  pin   GPIO pin id for the GPIO pin to deallocate from use.
      /// @exception  std::runtime_error is raised if the requested pin is not
      ///             exported or the unexport file could not be opened.
        void deallocate(pin_id pin);
      };

    /// @brief Standard GPIO pin allocator type alias
    ///
    /// The standard GPIO pin allocator is a pin_cache_allocator specialised
    /// with pin_export_allocator as the contained allocator type used to 
    /// pass on requests to when necessary, that is:
    ///
    /// - is_in_use for a pin that is currently free in the local process
    ///   but might be in use by some other process.
    /// - allocate if the pin is currently free in the local process then the
    ///   allocation request is passed on for more global allocation.
    /// - deallocate if the pin is currently in use locally then it should
    ///   also be in use globally and so needs to be globally freed.
      typedef pin_cache_allocator<pin_export_allocator> pin_allocator;
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_PIN_ALLOC_H
