// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file phymem_ptr.h 
/// @brief Implementation of physical memory smart pointer via /dev/mem & mmap.
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//
// Basic GNU compiler options building executable called main:
// g++ -std=c++0x -Wall -Wextra -pedantic -o main phymem_ptr.cpp

// ##### This is an initial scratch experimental version only.  #####
// ##### It requires immediate cleaning up and splitting apart. #####

#include "phymem_ptr.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <system_error>

namespace dibase { namespace rpi {
  namespace peripherals
  {
    static int mem_fd(-1);
 
    raw_phymem_ptr::raw_phymem_ptr( off_t phy_addr, size_t mapped_length )
    : mem( MAP_FAILED )
    , length(0)
    {
      char const * DevMemPath = "/dev/mem";
      if ( mem_fd<0)
      { // Attempt to open mem device once and save fd in static mem_fd member
        if ( (mem_fd = open(DevMemPath, O_RDWR|O_SYNC)) < 0 ) 
        {
          throw std::system_error
                ( errno
                , std::system_category()
                , "open /dev/mem failed. Did you forgot to use 'sudo ..'? "
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
        throw std::system_error( errno
                               , std::system_category()
                               , "mmap failed mapping physical memory area. "
                               );
      }
      length = mapped_length;
    }

    raw_phymem_ptr::~raw_phymem_ptr()
    {
      if ( MAP_FAILED != mem )
      {
        munmap( mem, length );
      }
    }
  }
}}

using namespace dibase::rpi::peripherals;
#include <unistd.h>

#define BCM2708_PERI_BASE        0x20000000
#define CLOCK_BASE               (BCM2708_PERI_BASE + 0x101000) /* Clocks */
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO   */
#define PWM_BASE                 (BCM2708_PERI_BASE + 0x20C000) /* PWM    */
#define SPI0_BASE                (BCM2708_PERI_BASE + 0x204000) /* SPI0 controller */
#define UART0_BASE               (BCM2708_PERI_BASE + 0x201000) /* Uart 0 */
#define UART1_BASE               (BCM2708_PERI_BASE + 0x215000) /* Uart 1 (not used) */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define INP_GPIO(g) *(pgpio.get()+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(pgpio.get()+((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET0   *(pgpio.get()+7)  // Set GPIO high bits 0-31
#define GPIO_CLR0   *(pgpio.get()+10) // Set GPIO low bits 0-31

int main()
{
    phymem_ptr<volatile unsigned> pclk(CLOCK_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> pgpio(GPIO_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> ppwm(PWM_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> pspi0(SPI0_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> puart0(UART0_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> puart1(UART1_BASE, BLOCK_SIZE);
    
    INP_GPIO(4);
    OUT_GPIO(4);

    for (unsigned p = 0; p < 10; p++) 
    {
        GPIO_SET0 = 1 << 4;
        sleep(1);
        GPIO_CLR0 = 1 << 4;
        sleep(1);
    }
}
 