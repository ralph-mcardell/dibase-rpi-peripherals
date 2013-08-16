// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file sysfs_interactivetests.cpp 
/// @brief Interactive tests for sys filesystem utilities.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "sysfs.h"
#include "interactivetests_config.h"
#include <iostream>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{
  unsigned const select_wait_time_secs{1U};
  unsigned const max_wait_time_secs{5U};
}

TEST_CASE( "Interactive_tests/sysfs/0000/initial edge event notifications OK"
         , "Setup for any edge events triggers initial resturn from select"
         )
{
  export_pin(test::in_pin_id);
  REQUIRE(is_exported(test::in_pin_id)==true);
  auto fd(open_ipin_for_edge_events(test::in_pin_id, edge_event_mode::rising));
  REQUIRE(fd!=-1);
  int select_retval{0};
  timeval tv;
  tv.tv_sec = select_wait_time_secs; tv.tv_usec = 0U;
  fd_set xfds;
  FD_ZERO(&xfds);
  FD_SET(fd, &xfds);
  select_retval = ::select(fd+1, NULL, NULL, &xfds, &tv);
  CHECK(select_retval==1);
  if (select_retval)
    {
      bool fd_in_xfds_isset{FD_ISSET(fd,&xfds)};
      CHECK(fd_in_xfds_isset);
    }
  CHECK(close_ipin_for_edge_events(fd)==true);
  unexport_pin(test::in_pin_id);
  CHECK(is_exported(test::in_pin_id)==false);
}

 void wait_on_select(edge_event_mode mode)
 {
  export_pin(test::in_pin_id);
  REQUIRE(is_exported(test::in_pin_id)==true);
  auto fd(open_ipin_for_edge_events(test::in_pin_id, mode));
  REQUIRE(fd!=-1);
  int select_retval{0};
  bool initialisation_notification{true};
  for ( unsigned wt{0U}; wt<max_wait_time_secs; wt += select_wait_time_secs)
    {
      timeval tv;
      tv.tv_sec = select_wait_time_secs; tv.tv_usec = 0U;
      fd_set xfds;
      FD_ZERO(&xfds);
      FD_SET(fd, &xfds);
      select_retval = ::select(fd+1, NULL, NULL, &xfds, &tv);
      CHECK(select_retval!=-1);
      if (select_retval==-1) break;
      if (select_retval)
        {
          bool fd_in_xfds_isset{FD_ISSET(fd,&xfds)};
          CHECK(fd_in_xfds_isset);
          if (initialisation_notification)
            {
              initialisation_notification = false;
              REQUIRE(lseek(fd,0,SEEK_SET)==0);
              char v{'\0'};
              REQUIRE(read(fd,&v,1)==1);
            }
          else
            {
              break;
            }
        }
      std::cout << "\b" << max_wait_time_secs - wt;
      std::cout.flush();
    }
  CHECK(select_retval==1);
  CHECK(close_ipin_for_edge_events(fd)==true);
  unexport_pin(test::in_pin_id);
  CHECK(is_exported(test::in_pin_id)==false);
}

TEST_CASE( "Interactive_tests/sysfs/0010/rising edge event notifications OK"
         , "Can successfully wait for rising edge events only"
         )
{
  std::cout << "sysfs: In pin rising edge notification test:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  std::cout << "Set BCM2835 GPIO" << test::in_pin_id << " low then high... ";
  wait_on_select(edge_event_mode::rising);
}

TEST_CASE( "Interactive_tests/sysfs/0020/falling edge event notifications OK"
         , "Can successfully wait for falling edge events only"
         )
{
  std::cout << "\n\n\nsysfs: In pin falling edge notification test:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  std::cout << "Set BCM2835 GPIO" << test::in_pin_id << " high then low... ";
  wait_on_select(edge_event_mode::falling);
}

TEST_CASE( "Interactive_tests/sysfs/0030/both edge event notifications OK"
         , "Can successfully wait for either edge event"
         )
{
  std::cout << "\n\n\nsysfs: In pin both edges notification test:\n"
               "Connect pin BCM2835 GPIO" << test::in_pin_id 
            << " to allow state changing between high voltage and ground "
               "(switch+resistors etc.)\n\n";
  std::cout << "Set BCM2835 GPIO" << test::in_pin_id << " low then high... ";
  wait_on_select(edge_event_mode::both);
  std::cout << "\nSet BCM2835 GPIO" << test::in_pin_id << " high then low... ";
  wait_on_select(edge_event_mode::both);
}