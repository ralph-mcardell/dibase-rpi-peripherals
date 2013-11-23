// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file i2c_pins_interactivetests.cpp 
/// @brief Platform tests for i2c_pins and related types.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#include "catch.hpp"

#include "i2c_pins.h"
#include "i2c_ctrl.h"
#include "periexcept.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <future>
#include <chrono>
#include <cstdint>
#include <sched.h>
#include <string.h>
#include <random>

using namespace dibase::rpi::peripherals;
using namespace dibase::rpi::peripherals::internal;

namespace
{

  class one_shot_timer
  {
    std::chrono::system_clock::time_point t_end;

  public:
    one_shot_timer(std::chrono::system_clock::duration t)
    : t_end{std::chrono::system_clock::now()+t}
    {}
 
    operator void*() 
    {
      return reinterpret_cast<void*>(t_end<std::chrono::system_clock::now());
    }
  };

  class random_byte
  {
    std::random_device  rnd_dev;

  public:
    std::uint8_t operator()()
    {
      return rnd_dev();
    }
  };
  
// Slave device values, change as required:
  constexpr std::uint8_t  slave_address{0x50};
  constexpr hertz         sclk_frequency{kilohertz{400}};
  constexpr int           wrap_length{512}; //bytes

// BCM2835 BSC I2C master peripheral values:
  constexpr std::size_t   fifo_depth{16}; 

  void welcome()
  {
    struct write_text
    {
      write_text()
      {
        std::cout << "\nBSC/I2C Interactive tests\n"
                     "These tests use a I2C memory device connected to the\n"
                     "Raspberry Pi P1 connector pins 3 & 5 with support for:\n" 
                     "   - auto address increment\n"
                     "   - address wrapping from highest to 0\n"
                     "   - write to specific address\n"
                     "   - read from specific address, using repeat start\n"
                     "   - read/write from next (auto incremented) address\n"
                     "Connect device SDA to P1 pin 3 and SCL to P1 pin 5"
                     "\nDevice settings (using in-code constants):"
                     "\n   Slave address: 0x" << std::hex 
                                            << (std::uint32_t)slave_address
                  << "\n  SCLK frequency: " << std::dec 
                  << sclk_frequency.count() << "Hz"
                  << "\n     Wrap-length: "  << wrap_length << " bytes\n"
                     "Device protocol details (<>:written data, {}:read data):"
                     "\n           Write: <slave address+write><memory address>"
                     "<data><data>..."
                     "\n                  (device auto increments address)" 
                     "\n       Read-next: <slave address+read>{data}{data}"
                     "{data}..."
                     "\n                  (start at last address with auto "
                     "increment)" 
                     "\n     Read-random: <slave address+write><memory address>"
                     "\n                  <slave address+read>{data}{data}"
                     "{data}..."
                     "\n                  (with auto increment, requires "
                     "repeated start)" 
                     "\n\n";
      }
      void dummy() {}
    };
    static write_text t;
    t.dummy(); // Keeps compiler quiet
  }
}

TEST_CASE( "Interactive_tests/i2c_pins/0000/write_read_test, no repeated start"
         , "Write data to a memory device then read it back using address "
           "increment wrap around"
         )
{
  welcome();
  std::cout << "\nI2C write-read test: address auto increment wrap, "
               "no repeated start:\n";
  std::uint8_t write_buffer[wrap_length] = {};
  std::uint8_t read_buffer[wrap_length] = {};
  for (int v=0; v!=wrap_length; ++v)
    {
      write_buffer[v] = v;
    }
  std::string dummy;
  std::cout << "Press <Enter> to continue..." << std::endl;
  std::getline(std::cin, dummy);
 
  i2c_pins iic(pin_id(0),pin_id(1),sclk_frequency);
  iic.clear();
  CHECK(iic.good());

  std::uint8_t zero{0}; // memory write start address

  std::uint8_t * xfer_ptr(write_buffer);
  std::size_t remaining_cnt{wrap_length};
  std::size_t xfer_cnt
              = iic.start_write(slave_address,wrap_length+1, &zero,1);
  CHECK(xfer_cnt==1);

  std::this_thread::sleep_for(std::chrono::microseconds(50));
  CHECK(iic.is_busy());

  one_shot_timer write_timeout{std::chrono::seconds(2)};
  while (remaining_cnt && !write_timeout)
    {
      CHECK_FALSE(iic.no_acknowledge());
      CHECK_FALSE(iic.clock_timeout());
      CHECK(iic.good());
      REQUIRE(iic.error_state()==0);
      if (iic.write_fifo_has_space())
        {
          CHECK(iic.is_busy());
          xfer_cnt = iic.write(xfer_ptr, remaining_cnt);
          remaining_cnt -= xfer_cnt;
          xfer_ptr += xfer_cnt;
        }
       else
        {
          std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
  REQUIRE(!write_timeout);
   
  one_shot_timer fifo_clear_timeout{std::chrono::seconds(1)};
  while (!iic.write_fifo_is_empty() && !fifo_clear_timeout)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  REQUIRE_FALSE(iic.is_busy());
  REQUIRE(!fifo_clear_timeout);

  iic.start_read(slave_address,wrap_length);
  remaining_cnt = wrap_length;
  xfer_ptr = read_buffer;

  one_shot_timer read_timeout{std::chrono::seconds(2)};
  while (remaining_cnt && !read_timeout)
    {
      CHECK_FALSE(iic.no_acknowledge());
      CHECK_FALSE(iic.clock_timeout());
      CHECK(iic.good());
      REQUIRE(iic.error_state()==0);
      if (iic.read_fifo_has_data())
        {
          xfer_cnt = iic.read(xfer_ptr, remaining_cnt);
          remaining_cnt -= xfer_cnt;
          xfer_ptr += xfer_cnt;
        }
       else
        {
          std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

  CHECK_FALSE(iic.is_busy());
  REQUIRE(!read_timeout);

  for (int v=0; v!=wrap_length; ++v)
    {
      CHECK(read_buffer[v]==write_buffer[v]);
    }
}

TEST_CASE( "Interactive_tests/i2c_pins/0020/write_random_read_test, repeated start"
         , "Write data to a memory device then read it back using random read "
           "and repeated start"
         )
{
  welcome();
  std::cout << "\nI2C write-random-read test: repeated start:\n";

  std::string dummy;
  std::cout << "Press <Enter> to continue..." << std::endl;
  std::getline(std::cin, dummy);
 
  random_byte value_generator;
  std::uint8_t addrs{value_generator()};
  std::uint8_t written_value{value_generator()};

  std::cout << "   Writing " << int(written_value) 
            << " to address " << int(addrs) 
            << std::endl;
  
  i2c_pins iic(pin_id(0),pin_id(1),sclk_frequency);
  iic.clear();
  CHECK(iic.good());

  std::uint8_t write_buffer[2] = {addrs, written_value};

  std::size_t xfer_cnt
              = iic.start_write(slave_address,2, write_buffer,2);
  CHECK(xfer_cnt==2);

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  CHECK_FALSE(iic.is_busy()); // should have sent the address & value bytes
  
  std::uint8_t read_value;
  CHECK(iic.start_read(slave_address, addrs, 1));

  one_shot_timer read_timeout{std::chrono::seconds(1)};
  while (!iic.read_fifo_has_data() && !read_timeout)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
  REQUIRE(!read_timeout);
  std::uint8_t read_count{iic.read(&read_value, 1)};
  CHECK(read_count==1);
  CHECK(read_value==written_value);
  CHECK_FALSE(iic.read_fifo_has_data());
  CHECK_FALSE(iic.is_busy());
}
