// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/spi0-adc-dac.cpp 
/// @brief SPI0 excample: read from SPI ADC and write to SPI DAC
/// 
/// Devices used are Microchip MCP3002 2 channel 10-bit Analogue to Digital
/// Converter (ADC), and the MCP4802 Digital to Analogue  Converter (DAC)
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//

#include "spi0_pins.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <iomanip>      // for std::setw
#include <chrono>
#include <thread>
#include <cstdlib>
#include <memory>

using namespace dibase::rpi::peripherals;

constexpr static auto  short_delay = std::chrono::microseconds(100);

/// @brief MCP3002 ADC usage modes
///
/// The format of the mode values is:
///
/// MSB
///
/// <start bit><sgl/diff bit><odd/sign bit><MSB 1st bit> <4-bits zero padding>
///
/// See the MSCP3002 data sheet Serial Communication section for details:
///  <a href="http://ww1.microchip.com/downloads/en/DeviceDoc/21294C.pdf">
///
/// Note that only MSB first modes are defined
///
enum class adc_mode : std::uint8_t
{ single_ended_ch0          = 0xd0
, single_ended_ch1          = 0xf0
, differential_ch0_positive = 0x90
, differential_ch1_positive = 0xb0
};

class mcp3002
{
  spi0_conversation * conversation_ptr;
  std::uint8_t        mode;

  constexpr static int   ten_bit_mask = 0x3ff;
  constexpr static int   number_of_upper_bits = 3; // in least significant bits
  constexpr static int   number_of_lower_bits = 7; // in most significant bits
  
// Shift of upper bits up to make space for lower bits
  constexpr static int   upper_left_shift = number_of_lower_bits; 

// Shift of lower bits down to least significant bits (8==bits in a byte)
  constexpr static int   lower_right_shift = 8-number_of_lower_bits;
  
  std::uint8_t read()
  {
    std::uint8_t byte{0U};
    while (!conversation_ptr->read(byte))
      {
        std::this_thread::sleep_for(short_delay);
      }
    return byte;
  }

public:
  mcp3002(spi0_conversation & cr, adc_mode am)
  : conversation_ptr{&cr} // naked pointer => non-owning
  , mode{static_cast<std::uint8_t>(am)}
  {
    (void)am; //keep compiler quiet -- thinks am not used (eh?)
  }

  int get(spi0_pins & sp)
  {
    conversation_ptr->open(sp);
    int result{-1};
  // Need to write 2 bytes to receive 2 bytes
    if (conversation_ptr->write(mode)&&conversation_ptr->write(mode))
      {
        result = read();
        result <<= upper_left_shift;
        std::uint8_t lower{read()};
        result |= (lower>>lower_right_shift);
        result &= ten_bit_mask;
      }
    conversation_ptr->close();
    return result;
  }
};

/// @brief Specify which MSCP48X2 model DAC is being used
///
/// The differences are the output data lengths 8, 10 or 12 bits
/// The interface is the same for all 3 models but which bits are
/// relevant differ in the last 8-bits sent in each 16-bit transaction.
enum class dac_model
{ mcp4802 = 8
, mcp4812 = 10
, mcp4822 = 12
};

/// @brief MCP48X2 usage modes
///
/// The format of the mode values is:
///
/// MSB
///
/// <start bit><sgl/diff bit><odd/sign bit><MSB 1st bit> <4-bits zero padding>
///
/// See the MSC48X2 data sheet Serial Interface section for details:
///  <a href="http://ww1.microchip.com/downloads/en/devicedoc/22249a.pdf">
///
/// Note that only MSB first modes are defined
///
enum class dac_mode : std::uint8_t
{ vout_a_x1           = 0x30U
, vout_b_x1           = 0xb0U
, vout_a_x2           = 0x10U
, vout_b_x2           = 0x90U
, vout_a_shutdown     = 0x20U
, vout_b_shutdown     = 0xa0U
};

template <dac_model M>
class mcp48x2
{
  spi0_conversation * conversation_ptr;
  std::uint8_t        mode;

  constexpr static int  twelve_bit_mask = 0xfff;
  constexpr static int  model_bit_shift = 12-static_cast<int>(M);

public:
  mcp48x2(spi0_conversation & cr, dac_mode dm)
  : conversation_ptr{&cr} // naked pointer => non-owning
  , mode{static_cast<std::uint8_t>(dm)}
  {
    (void)dm; //keep compiler quiet -- thinks dm not used (eh?)
  }

  bool put(spi0_pins & sp, int v)
  {
    v = (v<<model_bit_shift) & twelve_bit_mask;
    conversation_ptr->open(sp);
    bool rc{ conversation_ptr->write(mode|(v>>8)) // mode + value most sig. 4 bits
          && conversation_ptr->write(v&255)       // value least significant 8 bits
           };
    while (!sp.write_fifo_is_empty())
      {
        std::this_thread::sleep_for(short_delay);
      }
    conversation_ptr->close();
    return rc;   
  }
};

void output_inverse_of_input(bool const & running)
{
  constexpr auto adc_spi_frequency(megahertz{1});
  constexpr auto dac_spi_frequency(megahertz{1});
  constexpr auto sample_duration(std::chrono::milliseconds{50});
  try
    {
      spi0_pins sp(rpi_p1_spi0_full_pin_set);
      spi0_conversation adc_sc(spi0_slave::chip0, adc_spi_frequency);
      spi0_conversation dac_sc(spi0_slave::chip1, dac_spi_frequency);
      mcp3002 adc0(adc_sc, adc_mode::single_ended_ch0);
      mcp48x2<dac_model::mcp4802> dac0(dac_sc, dac_mode::vout_a_x1);

      auto t_sample(std::chrono::system_clock::now());
      while (running)
        {
          int in{adc0.get(sp)};
          int out{(1023-in)>>2};
          dac0.put(sp,out);
          std::cout << "ADC0: " << std::setw(4) << in
                    << "   DAC0: "<< std::setw(4) << out
                    << '\r';
          std::cout.flush();
          do
            { // Set next sample time point
              t_sample += sample_duration;
            }
          while ( t_sample < std::chrono::system_clock::now() );
          std::this_thread::sleep_until( t_sample );
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
  catch (...)
    {
      std::cerr << "A problem occurred. Caught by catch all handler\n";
    }
}

int main()
{
  std::cout << "Press enter to quit...." << std::endl;
  bool running{true};
  std::thread worker{ output_inverse_of_input, std::cref(running) };
  std::string dummy;
  std::getline(std::cin, dummy);
  running = false;
  worker.join();
}
