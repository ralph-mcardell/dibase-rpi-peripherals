// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.cpp
/// @brief Single GPIO pin edge event class implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pin_edge_event.h"
#include "sysfs.h"
#include "pinexcept.h"
#include <system_error>
#include <unistd.h>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    namespace
    {
      static int open_for_edge_event(pin_id id, pin_edge_event::edge_mode mode)
      {
        if (! internal::is_exported(id))
          {
            throw bad_pin_export_state{"pin_edge_event: GPIO pin "
                                       "not exported in sys file system."};
          }
        using internal::edge_event_mode;
        edge_event_mode 
        internal_mode
                    { mode==pin_edge_event::rising  ? edge_event_mode::rising
                    : mode==pin_edge_event::falling ? edge_event_mode::falling
                    : mode==pin_edge_event::both    ? edge_event_mode::both
                    : edge_event_mode::bad_mode
                    };
        return internal::open_ipin_for_edge_events(id, internal_mode);
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

    pin_edge_event::pin_edge_event(pin_id id, edge_mode mode)
    : pin_event_fd{open_for_edge_event(id,mode)}
    {}

    pin_edge_event::pin_edge_event(ipin const & in, edge_mode mode)
    : pin_event_fd{ [](ipin const & in, edge_mode mode)->int
                    { 
                      if (! in.is_open())
                        {
                          throw std::invalid_argument
                                            {"pin_edge_event: ipin not open."};
                        }
                      return open_for_edge_event(in.get_pin(),mode);
                    }(in,mode)
                  }
    {}
    
    pin_edge_event::~pin_edge_event()
    {
      internal::close_ipin_for_edge_events(pin_event_fd);
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
