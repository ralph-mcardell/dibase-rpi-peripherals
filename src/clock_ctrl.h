// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file clock_ctrl.h 
/// @brief Internal GPIO clock control type.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H
 #define DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H

# include "phymem_ptr.h"
# include "clock_registers.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
      class clock_parameters;
      constexpr unsigned gpclk0{0U}; ///< GPCLK0 internal index value
      constexpr unsigned gpclk1{1U}; ///< GPCLK1 internal index value
      constexpr unsigned gpclk2{2U}; ///< GPCLK2 internal index value
      constexpr unsigned pwmclk{3U}; ///< PWMCLK internal index value
      constexpr std::size_t number_of_clocks{4U};///< Number of supported clocks

    /// @broef Convert an internal clock index value to a clock_id enum value
    /// @param i  Internal clock index value: gpclk0, gpclk1, gpclk2 or pwmclk.
    ///           NOT range checked.
    /// @returns clock_id enumeration value used with clock register operations
      clock_id index_to_clock_id(unsigned i);

    /// @brief GPIO clock control type. There is only 1 (yes it's a singleton!)
    /// Maps BCM2708/2835 GPIO clock registers into the requisite physical
    /// memory mapped area
      struct clock_ctrl
      {

      /// @brief Simple clock allocation class.
      ///
      /// Supports allocate, de-allocate and in_use operations for clocks
      /// specified using internal clock index values gpclk0, gpclk1, gpclk2
      /// and pwmclk.
        class clock_allocator
        {
          unsigned allocated;

        public:
        /// @brief Construct with all clocks available for allocation
          clock_allocator() : allocated{0U} {}

        /// @brief Return whether a clock is marked as in use or not
        /// @param clock_idx  Internal clock index value of clock to check
        ///                   allocation state of: gpclk0, gpclk1, gpclk2 or
        ///                   pwmclk. Partially range checked.
        /// @returns true if clock marked as allocated; false if it is not OR
        ///          clock_idx is out of range.
          bool is_in_use(unsigned clock_idx)
          {
            return (clock_idx<number_of_clocks) && (allocated&(1<<clock_idx));
          }

        /// @brief Allocate a clock marking it as in use
        /// @param clock_idx  Internal clock index value of clock to allocate:
        ///                   gpclk0, gpclk1, gpclk2 or pwmclk. Range checked.
        /// @returns true if clock was allocated by call; false if it was not
        ///          as already allocated or clock_idx out of range
          bool allocate(unsigned clock_idx)
          {
            if ( (clock_idx<number_of_clocks) && !is_in_use(clock_idx) )
              {
                allocated |= (1<<clock_idx);
                return true;
              }
            return false;
          }

        /// @brief De-allocate a clock marking it as free for use
        /// @param clock_idx  Internal clock index of clock to de-allocate:
        ///                   gpclk0, gpclk1, gpclk2 or pwmclk. Range checked.
        /// @returns true if clock was de-allocated by call; false if it was not
        ///          as already free or clock_idx out of range
          bool deallocate(unsigned clock_idx)
          {
            if ( is_in_use(clock_idx) )
              {
                allocated &= ~(1<<clock_idx);
                return true;
              }
            return false;
          }
        };

        /// @brief Pointer to BCM2708 / BCM2835 clock control registers instance
        phymem_ptr<volatile clock_registers>   regs;

      /// @brief Clock allocator instance
        clock_allocator                       alloc;

      /// @brief Singleton instance getter
      /// @returns THE instance of the GPIO clock control object.
        static clock_ctrl & instance();

      /// @brief Initialise a clock with passed parameters
      /// @param clk  id of clock to initialise
      /// @param cp   Parameters to use for clock.
        static void initialise_clock(clock_id clk, clock_parameters const & cp);

      private:
      /// @brief Construct: initialise regs with correct physical address & size 
        clock_ctrl();

        clock_ctrl(clock_ctrl const &) = delete;
        clock_ctrl(clock_ctrl &&) = delete;
        clock_ctrl & operator=(clock_ctrl const &) = delete;
        clock_ctrl & operator=(clock_ctrl &&) = delete;
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_CLOCK_CTRL_H
