// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/spi0-adc-dac.cpp 
/// @brief SPI0 excample: read from SPI ADC and write to SPI DAC
/// 
/// Devices used are Microchip MCP3002 2 channel 10-bit Analogue to Digital
/// Converter (ADC), and the MCP4802 Digital to Analogue  Converter (DAC)
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "spi0_pins.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <iomanip>      // for std::setw
#include <chrono>       // for various time related types
#include <thread>       // for std::thread, std::this_thread

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
enum class adc_mode : std::uint8_t
{ single_ended_ch0          = 0xd0
, single_ended_ch1          = 0xf0
, differential_ch0_positive = 0x90
, differential_ch1_positive = 0xb0
};

/// @brief Type representing a MCP3002 ADC channel
///
/// Construct from the required adc_mode and SPI0 slave chip enable value
/// (spi0_slave::chip0 or spi0_slave::chip1) and data transfer frequency.
/// Sample values can then be read from device using the object's get member
/// function.
class mcp3002
{
  spi0_conversation conversation;
  std::uint8_t      mode;

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
    while (!conversation.read(byte))
      {
        std::this_thread::sleep_for(short_delay);
      }
    return byte;
  }

public:
/// @brief Construct from ADC mode and required SPI0 parameters
/// @param[in]  am    ADC mode required for object
/// @param{in}  cs    Slave chip select value.
/// @param[in]  f     SPI SCLK frequency  
  mcp3002(adc_mode am, spi0_slave cs, hertz f)
  : conversation(cs, f)
  , mode{static_cast<std::uint8_t>(am)}
  {
    (void)am; //keep compiler quiet -- thinks am not used (eh?)
  }

  mcp3002(mcp3002 const &) = delete;
  mcp3002& operator=(mcp3002 const &) = delete;
  mcp3002(mcp3002 &&) = delete;
  mcp3002& operator=(mcp3002 &&) = delete;

/// @brief Receive a sample value from the device.
/// @param[in]  sp    Valid spi0_pins object used to open an spi0_conversation.
/// @returns Positive value in the range [0,1023] or -1 if unable to
///           send request to the device due to the SPI0 transmit FIFO being
///           full (unlikely).
  int get(spi0_pins & sp)
  {
    conversation.open(sp);
    int result{-1};
  // Need to write 2 bytes to receive 2 bytes
    if (conversation.write(mode)&&conversation.write(mode))
      {
        result = read();
        result <<= upper_left_shift;
        std::uint8_t lower{read()};
        result |= (lower>>lower_right_shift);
        result &= ten_bit_mask;
      }
    conversation.close();
    return result;
  }
};

/// @brief MSCP48X2 DAC model values
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
enum class dac_mode : std::uint8_t
{ vout_a_x1           = 0x30U
, vout_b_x1           = 0xb0U
, vout_a_x2           = 0x10U
, vout_b_x2           = 0x90U
, vout_a_shutdown     = 0x20U
, vout_b_shutdown     = 0xa0U
};

/// @brief Type template representing a DAC channel from the set of MCP48X2
/// chips (MCP4802, MCP4812 and MCP4822)
///
/// Construct from the required dac_mode and SPI0 slave chip enable value
/// (spi0_slave::chip0 or spi0_slave::chip1) and data transfer frequency.
/// Sample values can then be written to device using the object's put member
/// function.
///
/// @tparam M     MCP48X2 model value
template <dac_model M>
class mcp48x2
{
  spi0_conversation conversation;
  std::uint8_t      mode;

  constexpr static int  twelve_bit_mask = 0xfff;
  constexpr static int  model_bit_shift = 12-static_cast<int>(M);

public:
/// @brief Construct from DAC mode and required SPI0 parameters
/// @param[in]  dm    DAC mode required for object
/// @param{in}  cs    Slave chip select value.
/// @param[in]  f     SPI SCLK frequency  
  mcp48x2(dac_mode dm, spi0_slave cs, hertz f)
  : conversation{cs, f} // naked pointer => non-owning
  , mode{static_cast<std::uint8_t>(dm)}
  {
    (void)dm; //keep compiler quiet -- thinks dm not used (eh?)
  }

  mcp48x2(mcp48x2 const &) = delete;
  mcp48x2& operator=(mcp48x2 const &) = delete;
  mcp48x2(mcp48x2 &&) = delete;
  mcp48x2& operator=(mcp48x2 &&) = delete;

/// @brief Send a sample value to the device
/// @param[in]  sp    Valid spi0_pins object used to open an spi0_conversation.
/// @param[in]  v     Value to send to the device in the range [0,255], [0,1023]
///                   or [0,4095] depending on M, the type's model parameter.
///                   Out of range values are masked to the expected range.
/// @returns true if request could be written to the SPI0 transmit FIFO,
///          false if it could not (unlikely).
  bool put(spi0_pins & sp, int v)
  {
    v = (v<<model_bit_shift) & twelve_bit_mask;
    conversation.open(sp);
    bool rc{ conversation.write(mode|(v>>8)) // mode + value most sig. 4 bits
          && conversation.write(v&255)       // value least significant 8 bits
           };
    while (!sp.write_fifo_is_empty())
      {
        std::this_thread::sleep_for(short_delay);
      }
    conversation.close();
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
      mcp3002 adc0( adc_mode::single_ended_ch0
                  , spi0_slave::chip0, adc_spi_frequency
                  );
      mcp48x2<dac_model::mcp4802> dac0( dac_mode::vout_a_x2
                                      , spi0_slave::chip1, dac_spi_frequency
                                      );

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
