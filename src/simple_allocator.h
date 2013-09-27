// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file simple_allocator.h 
/// @brief \b Internal : Simple resource allocator : class definition
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_SIMPLE_ALLOCATOR_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_SIMPLE_ALLOCATOR_H

# include <bitset>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      /// @brief Simple resource allocation class template.
      ///
      /// Supports allocate, de-allocate and usage query operations for
      /// resources specified using resource index values
      ///
      /// @tparam NumRes  Number of resources supported 
        template <unsigned NumRes>
        class simple_allocator
        {
          std::bitset<NumRes> allocated;

        public:
        /// @brief Construct with all resources available for allocation
          simple_allocator() = default;

        /// @brief Return whether a resource is marked as in use or not
        /// @param res_idx    0 based resource index value of resource to check
        ///                   allocation state of. Partially range checked.
        /// @returns true if resource marked as allocated; false if it is not
        ///               OR res_idx is out of range.
          bool is_in_use(unsigned res_idx)
          {
            return (res_idx<NumRes) && allocated.test(res_idx);
          }

        /// @brief Return whether any resource is marked as in use
        /// @returns true if any resource marked as allocated; false if none are
          bool any_in_use()
          {
            return allocated.any();
          }

        /// @brief Allocate a resource marking it as in use
        /// @param res_idx    0 based resource index of resource to allocate.
        ///                   Range checked.
        /// @returns true if resource was allocated by call; false if it was
        ///               not as already allocated or res_idx out of range.
          bool allocate(unsigned res_idx)
          {
            if ( (res_idx<NumRes) && !allocated.test(res_idx) )
              {
                allocated.set(res_idx);
                return true;
              }
            return false;
          }

        /// @brief De-allocate a resource marking it as free for use
        /// @param res_idx    0 based resource index of resource to de-allocate.
        ///                   Range checked.
        /// @returns true if resource was de-allocated by call; false if it
        ///               was not as already free or res_idx out of range.
          bool deallocate(unsigned res_idx)
          {
            if ( is_in_use(res_idx) )
              {
                allocated.reset(res_idx);
                return true;
              }
            return false;
          }
        };
   } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_SIMPLE_ALLOCATOR_H
