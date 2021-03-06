// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file phymem_ptr.cpp
/// @brief Implementation of physical memory smart pointer via /dev/mem & mmap.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "phymem_ptr.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <system_error>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      static int mem_fd{-1};

      raw_phymem_ptr::raw_phymem_ptr
      ( physical_address_t phy_addr
      , std::size_t mapped_length
      )
      : mem(nullptr)
      , length(0)
      {
        char const * DevMemPath{"/dev/mem"};
        if ( mem_fd<0 )
          { // Attempt to open mem device once and save fd in mem_fd static global
            if ( (mem_fd = open(DevMemPath, O_RDWR|O_SYNC))<0 ) 
              {
                throw std::system_error
                      ( errno
                      , std::system_category()
                      , "open /dev/mem failed. Did you forget to use 'sudo ..'? "
                      );
              }
          }

        mem = mmap( NULL
                  , mapped_length
                  , PROT_READ|PROT_WRITE
                  , MAP_SHARED
                  , mem_fd
                  , phy_addr
                  );
        if ( MAP_FAILED == mem )
          {
            mem = nullptr;
            throw std::system_error( errno
                                   , std::system_category()
                                   , "mmap failed mapping physical memory area."
                                   );
          }
        length = mapped_length;
      }

      raw_phymem_ptr::raw_phymem_ptr(raw_phymem_ptr && tmp)
      : mem{tmp.mem}
      , length{tmp.length}
      {
        tmp.mem = nullptr;
        tmp.length = 0;
      }

      raw_phymem_ptr & raw_phymem_ptr::operator=(raw_phymem_ptr && tmp)
      {
        mem = tmp.mem;
        length = tmp.length;
        tmp.mem = nullptr;
        tmp.length = 0;
        return *this;
      }

      raw_phymem_ptr::~raw_phymem_ptr()
      {
        if ( mem != nullptr )
          {
            munmap( mem, length );
          }
      }
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed

