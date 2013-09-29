// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.cpp
/// @brief Single GPIO pin edge event class implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pin_edge_event.h"
#include "sysfs.h"
#include "pin_alloc.h"
#include <system_error>
#include <unistd.h>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace
    {
      struct trivial_pin_allocator
      {
        bool is_in_use(pin_id)  {return false;}
        void allocate(pin_id)   {}
        void deallocate(pin_id) {}
      };

      typedef internal::pin_cache_allocator<trivial_pin_allocator> 
                                                      pin_edge_event_allocator;

      static pin_edge_event_allocator & allocator()
      {
        static pin_edge_event_allocator alloc;
        return alloc;
      }

      static int open_for_edge_event(pin_id id, pin_edge_event::edge_mode mode)
      {
        try 
          {
            allocator().allocate(id);
          }
        catch (bad_peripheral_alloc const &)
          {
            throw bad_peripheral_alloc{"pin_edge_event: pin already has "
                                       "pin_edge_event."};
          }
        try
          {
            if (! internal::is_exported(id))
              {
                throw bad_pin_export_state{"pin_edge_event: GPIO pin "
                                           "not exported in sys file system."};
              }
            using internal::edge_event_mode;
            edge_event_mode 
            internal_mode
                        { mode==pin_edge_event::rising ?edge_event_mode::rising
                        : mode==pin_edge_event::falling?edge_event_mode::falling
                        : mode==pin_edge_event::both   ?edge_event_mode::both
                        : edge_event_mode::bad_mode
                        };
            return internal::open_ipin_for_edge_events(id, internal_mode);
          }
        catch (std::exception const &)
          {
             allocator().deallocate(id);
             throw;
          }
      }

      static int wait_for_event(int fd, timespec * pts)
      {
        fd_set xfds;
        FD_ZERO(&xfds);
        FD_SET(fd, &xfds);
        int rv{::pselect(fd+1, nullptr, nullptr, &xfds, pts, nullptr)};
        if (rv==-1)
          {
            throw std::system_error
                  ( errno
                  , std::system_category()
                  , "pin_edge_event: waiting/checking for pin edge event"
                    " failed with error from call to pselect."
                  );
          }
        return rv;
      }
    }

    pin_edge_event::pin_edge_event(ipin const & in, edge_mode mode)
    : pin_event_fd{open_for_edge_event(in.get_pin(),mode)}
    , id{in.get_pin()}
    {}
    
    pin_edge_event::~pin_edge_event()
    { // Allow no exceptions to escape.
      try
      {
        internal::close_ipin_for_edge_events(pin_event_fd); // should not throw
        allocator().deallocate(id);  // should only throw if id not allocated!
      } 
      catch (...) {}
    }

    bool pin_edge_event::signalled() const
    {
      timespec ts;
      ts.tv_sec = 0L;
      ts.tv_nsec = 0L;
      return wait_for_event(pin_event_fd, &ts)==1;
    }

    void pin_edge_event::clear() const
    {
      if (::lseek(pin_event_fd,0,SEEK_SET)==-1)
        {
            throw std::system_error
                  ( errno
                  , std::system_category()
                  , "pin_edge_event: clearing pin edge event"
                    " failed with error from call to lseek."
                  );
        }
      char v{'\0'};
      if (::read(pin_event_fd,&v,1)==-1)
        {
            throw std::system_error
                  ( errno
                  , std::system_category()
                  , "pin_edge_event: clearing pin edge event"
                    " failed with error from call to read."
                  );
        }
     }

    void pin_edge_event::wait() const
    {
      wait_for_event(pin_event_fd, nullptr);
    }

    bool pin_edge_event::wait_(long t_rel_secs, long t_rel_ns) const
    {
      timespec ts;
      ts.tv_sec = t_rel_secs;
      ts.tv_nsec = t_rel_ns;
      return wait_for_event(pin_event_fd, &ts)==1;
    }
  }
}}
