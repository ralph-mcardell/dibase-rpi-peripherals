// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/open-collector.cpp 
/// @brief Gertboard_sw ocol program equivalent
//
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell
//
// Basic GNU compiler options building executable called main:
// g++ -std=c++0x -Wall -Wextra -pedantic -I.. -L../../lib -o ocol open-collector.cpp -lrpi-periphals 

#include "phymem_ptr.h"
#include <unistd.h>

#define BCM2708_PERI_BASE        0x20000000
#define CLOCK_BASE               (BCM2708_PERI_BASE + 0x101000)
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000)
#define PWM_BASE                 (BCM2708_PERI_BASE + 0x20C000)
#define SPI0_BASE                (BCM2708_PERI_BASE + 0x204000)
#define UART0_BASE               (BCM2708_PERI_BASE + 0x201000)
#define UART1_BASE               (BCM2708_PERI_BASE + 0x215000)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define INP_GPIO(g) *(pgpio.get()+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(pgpio.get()+((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET0   *(pgpio.get()+7)  // Set GPIO high bits 0-31
#define GPIO_CLR0   *(pgpio.get()+10) // Set GPIO low bits 0-31

using namespace dibase::rpi::peripherals;

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
