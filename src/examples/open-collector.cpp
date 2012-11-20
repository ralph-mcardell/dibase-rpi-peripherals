// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//
// Basic GNU compiler options building executable called ocol:
// g++ -std=c++0x -Wall -Wextra -pedantic -I.. -L../../lib -o ocol open-collector.cpp -lrpi-periphals 

// ##### Ongoing version. Will be updated when more facilities avaialble #####

#include "phymem_ptr.h"
#include "gpio_registers.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <unistd.h>     // for sleep

// The following macros were copied from the Gertboard software gb_common.h
#define BCM2708_PERI_BASE        0x20000000
#define CLOCK_BASE               (BCM2708_PERI_BASE + 0x101000)
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000)
#define PWM_BASE                 (BCM2708_PERI_BASE + 0x20C000)
#define SPI0_BASE                (BCM2708_PERI_BASE + 0x204000)
#define UART0_BASE               (BCM2708_PERI_BASE + 0x201000)
#define UART1_BASE               (BCM2708_PERI_BASE + 0x215000)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

using namespace dibase::rpi::peripherals;

int main()
{
  try
    {
      phymem_ptr<volatile unsigned> pclk(CLOCK_BASE, BLOCK_SIZE);
      phymem_ptr<volatile unsigned> ppwm(PWM_BASE, BLOCK_SIZE);
      phymem_ptr<volatile unsigned> pspi0(SPI0_BASE, BLOCK_SIZE);
      phymem_ptr<volatile unsigned> puart0(UART0_BASE, BLOCK_SIZE);
      phymem_ptr<volatile unsigned> puart1(UART1_BASE, BLOCK_SIZE);

      phymem_ptr<volatile gpio_registers> pgpio(GPIO_BASE, BLOCK_SIZE);
      
      pgpio->set_pin_function(4, gpio_pin_fn::output);

      for (unsigned p = 0; p < 10; p++) 
        {
          pgpio->gpset[0] = 1 << 4;
          sleep(1);
          pgpio->gpclr[0] = 1 << 4;
          sleep(1);
        }
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occured. Description: " << e.what() << "\n";
    }
}
