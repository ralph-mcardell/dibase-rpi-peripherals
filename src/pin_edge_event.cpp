// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin_edge_event.cpp
/// @brief Single GPIO pin edge event class implementation.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "pin_edge_event.h"
#include "sysfs.h"
#include "pinexcept.h"

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
  }
}}
