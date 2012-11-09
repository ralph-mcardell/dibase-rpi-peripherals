// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
// File: phymem_ptr.cpp : Smart pointer to physical memory via /dev/mem & mmap.
//
// Copyright (c) Dibase Limited 2012
// Author: Ralph E. McArdell
//
// Basic GNU compiler options building executable called main:
// g++ -std=c++0x -Wall -Wextra -pedantic -o main phymem_ptr.cpp

// ##### This is an initial scratch experimental version only.  #####
// ##### It requires immediate cleaning up and splitting apart. #####

#include <sys/mman.h>
#include <fcntl.h>
#include <system_error>
#include <cstdlib>

std::size_t const PageSize(4*1024);

class raw_phymem_ptr
{
    void *  mem;
    std::size_t length;
    static int mem_fd;

 public:
    raw_phymem_ptr( off_t byte_offset, size_t length );
    ~raw_phymem_ptr();
    void * get() { return mem; }
 };
 
 template <typename T>
 class phymem_ptr : public raw_phymem_ptr
 {
 public:
    phymem_ptr( off_t byte_offset, size_t length ) 
    : raw_phymem_ptr(byte_offset, length)
    {}

    T* get() { return reinterpret_cast<T*>(raw_phymem_ptr::get()); }
    T* get(std::size_t idx) { return reinterpret_cast<T*>(raw_phymem_ptr::get())+idx; }
    T& operator*() { return *get(); }
    T* operator->() { return get(); }
    T& operator[](std::size_t idx) { return *get(idx); } 
 };
 
 int raw_phymem_ptr::mem_fd(-1);
 
 raw_phymem_ptr::raw_phymem_ptr( off_t byte_offset, size_t mapped_length )
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
                    , "open /dev/mem failed. Did you forgot to use 'sudo .. '? "
                    );
        }
    }

    mem = mmap( NULL
              , mapped_length
              , PROT_READ|PROT_WRITE
              , MAP_SHARED
              , mem_fd
              , byte_offset
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


#include <iostream>

 
 raw_phymem_ptr::~raw_phymem_ptr()
 {
    if ( MAP_FAILED != mem )
    {
std::cerr << "Calling munmap("<<mem<<", "<<length<<");\n";
        munmap( mem, length );
    }
 }
 

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

#include <unistd.h>
int main()
{
    phymem_ptr<volatile unsigned> pclk(CLOCK_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> pgpio(GPIO_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> ppwm(PWM_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> pspi0(SPI0_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> puart0(UART0_BASE, BLOCK_SIZE);
    phymem_ptr<volatile unsigned> puart1(UART1_BASE, BLOCK_SIZE);

    std::cerr << "pgpio.get()="
              << pgpio.get()
              << "  pgpio.get(4/10)="
              << pgpio.get(4/10)<<"\n";
    
std::cerr << "About to execute INP_GPIO(4)\n";
    INP_GPIO(4);
std::cerr << "About to execute OUT_GPIO(4)\n";
    OUT_GPIO(4);

    for (unsigned p = 0; p < 10; p++) 
    {
std::cerr << "About to execute GPIO_SET0 = 1 << 4\n";
        GPIO_SET0 = 1 << 4;
        sleep(1);
std::cerr << "About to execute GPIO_CLR0 = 1 << 4\n";
        GPIO_CLR0 = 1 << 4;
        sleep(1);
    }
}
 