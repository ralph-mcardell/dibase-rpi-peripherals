// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file phymem_ptr.h 
/// @brief \b Internal : physical memory smart pointer : type definitions.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_PHYMEM_PTR_H
 #define DIBASE_RPI_PERIPHERALS_INTERNAL_PHYMEM_PTR_H

 #include "peridef.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Physical memory smart pointer base class 
    ///
    /// Intended as base class use only. All operations protected.
    ///
    /// Provides access to physical memory by mapping /dev/mem device regions
    /// with mmap.
    ///
    /// Only provides:
    ///   - Constructor that mmaps a region of /dev/mem.
    ///   - Destructor that munmaps the managed region.
    ///   - Untyped (void*) access to start of mapped region
      class raw_phymem_ptr
      {
        void * mem;         ///< pointer to mapped region
        std::size_t length; ///< length of mapped region

      protected:
      /// @brief Construct from physical address and region
      ///
      /// Construct mapped physical memory region from physical memory address
      /// offset and length of region.
      ///
      /// Note: Start of region mapping will be aligned to a page boundary.
      ///       Mapped start address should be page aligned and be a 
      ///       page size multiple in length.
      ///
      /// @param[in]  phy_addr  Physical address to map, page size multiple.
      ///                       Used as offset into /dev/mem device 'file'.
      /// @param[in]  length    Length of mapped address region. Page size
      ///                       multiple.
      /// @exception  std::system_error if /dev/mem cannot be opened or the
      ///             region cannot be mapped.
        raw_phymem_ptr(physical_address_t phy_addr, std::size_t length);

      /// @brief Destructor: unmaps using munmap mapped physical memory region.
        ~raw_phymem_ptr();
        
      /// @brief Accessor. Untyped access to mapped memory region.
      /// @return Untyped pointer to start of mapped memory region
        void * get() 
        {
          return mem;
        }

      public:
        raw_phymem_ptr(raw_phymem_ptr const &) = delete;
        raw_phymem_ptr & operator=(raw_phymem_ptr const &) = delete;

      /// @brief Move construction: copies temporary other's mapped memory
      /// pointer & length values to this's values and nulls/zero temporary's
      /// values 
      ///
      /// @param[in] tmp  Temporary other object to move (steal guts) from
        raw_phymem_ptr(raw_phymem_ptr && tmp);

      /// @brief Move assignment: copies temporary other's mapped memory
      /// pointer & length values to this's values and nulls/zero temporary's
      /// values 
      ///
      /// @param[in] tmp  Temporary other object to move (steal guts) from
        raw_phymem_ptr & operator=(raw_phymem_ptr && tmp);
      };

    /// @brief Typed physical memory smart pointer class template
    ///
    /// Thin sub-class template of \ref raw_phymem_ptr that casts the untyped
    /// void* to the mapped region to the type specified by the template
    /// parameter T. Provides typed get and indexed get into the mapped region
    /// as well as operator overloads for * (dereference) -> and [].
    ///
    /// A primary use case is to access memory mapped peripheral control and
    /// data areas. In such cases the type for T should probably be qualified 
    /// as volatile.
    ///
    /// @tparam T Type of items referenced/pointed to in mapped memory region
      template <typename T>
      class phymem_ptr : public raw_phymem_ptr
      {
      public:
      /// @brief Construct from physical address and region
      ///
      /// Simply passes parameters to the base 
      /// \ref raw_phymem_ptr::raw_phymem_ptr(physical_address_t phy_addr, std::size_t length)
      /// constructor.
      ///
      /// @param[in]  phy_addr  Physical address to map, page size multiple.
      ///                       Used as offset into /dev/mem device 'file'.
      /// @param[in]  length    Length of mapped address region. Page size
      ///                       multiple.
      /// @exception  std::system_error if /dev/mem cannot be opened or the
      ///             region cannot be mapped.
        phymem_ptr(physical_address_t phy_addr, std::size_t length) 
        : raw_phymem_ptr(phy_addr, length)
        {}

      /// @brief Accessor. Typed access to mapped memory region start.
      /// @return Typed pointer to start of mapped memory region
        T* get()
        {
          return reinterpret_cast<T*>(raw_phymem_ptr::get());
        }

      /// @brief Accessor. Typed array access to mapped memory region.
      ///
      /// Note: The idx parameter value is _not_ range checked.
      ///
      /// @param[in]  idx   Zero based index to the required item of type T
      ///                   in the mapped region.
      /// @return Typed pointer to nth T in the mapped memory region (n=idx).
        T* get(std::size_t idx)
        {
          return reinterpret_cast<T*>(raw_phymem_ptr::get())+idx;
        }

      /// @brief Dereference operator.
      /// @return Reference to first item of type T in mapped region.
        T& operator*()
        {
          return *get();
        }

      /// @brief Member access operator.
      /// @return Pointer to first item of type T in mapped region.
        T* operator->()
        {
          return get();
        }

      /// @brief Subscript operator.
      ///
      /// Note: The idx parameter value is _not_ range checked.
      ///
      /// @param[in]  idx   Zero based index to the required item of type T
      ///                   in the mapped region.
      /// @return Reference to nth item of type T in the mapped region; n=idx.
        T& operator[](std::size_t idx) 
        {
          return *get(idx);
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_PHYMEM_PTR_H
