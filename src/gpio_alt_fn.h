// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file gpio_alt_fn.h 
/// @brief \b Internal : GPIO pin alternative special function support :
/// type definitions and function declarations.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 6 General Purpose
/// I/O (GPIO) for details.
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_GPIO_ALT_FN_H
# define DIBASE_RPI_PERIPHERALS_GPIO_ALT_FN_H

# include "gpio_registers.h"
# include "pin_id.h"

# include <vector>
# include <initializer_list>

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Namespace grouping entities related to GPIO pin alternative
    /// functions' special function assignments
      namespace pin_alt_fn
      {
      /// @brief Enumerates special functions GPIO pins may support
      ///
      /// Unnumbered table following table 6-31 ("Special function legend") in
      /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
      /// Broadcom BCM2835 ARM Peripherals Datasheet</a> details the various
      /// possible special functions that GPIO pins may support. The enumeration
      /// names are based on the names in this table.
      /// Note: BSC is the Broadcom Serial Control bus that is I2C compliant.
        enum class gpio_special_fn
        {
        /// @brief No special function
        /// In BCM2835 peripherals datasheet table 6-31. Slot for pin
        /// alternative function is blank or specified as \<reserved\> or
        /// \<Internal\>.
          no_fn           
        , gpclk0          ///< General purpose Clock 0
        , gpclk1          ///< General purpose Clock 1
        , gpclk2          ///< General purpose Clock 2
        , pwm0            ///< Pulse Width Modulator 0
        , pwm1            ///< Pulse Width Modulator 1
        , txd0            ///< UART 0 Transmit Data
        , rxd0            ///< UART 0 Receive Data 
        , cts0            ///< UART 0 Clear To Send
        , rts0            ///< UART 0 Request To Send
        , txd1            ///< UART 1 Transmit Data
        , rxd1            ///< UART 1 Receive Data
        , cts1            ///< UART 1 Clear To Send
        , rts1            ///< UART 1 Request To Send 
        , sda0            ///< BSC master 0 data line
        , scl0            ///< BSC master 0 clock line 
        , sda1            ///< BSC master 1 data line 
        , scl1            ///< BSC master 1 clock line 
        , bscsl_sda_mosi  ///< BSC slave Data, SPI slave MOSI
        , bscsl_scl_sclk  ///< BSC slave Clock, SPI slave clock
        , bscsl_miso      ///< BSC - no function, SPI MISO
        , bscsl_ce_n      ///< BSC - no function, SPI CSn
        , spi0_ce1_n      ///< SPI0 Chip select 1
        , spi0_ce0_n      ///< SPI0 Chip select 0
        , spi0_miso       ///< SPI0 MISO
        , spi0_mosi       ///< SPI0 MOSI
        , spi0_sclk       ///< SPI0 Serial clock
        , spi1_ce0_n      ///< SPI1 Chip select 0
        , spi1_ce1_n      ///< SPI1 Chip select 1
        , spi1_ce2_n      ///< SPI1 Chip select 2
        , spi1_miso       ///< SPI1 MISO
        , spi1_mosi       ///< SPI1 MOSI
        , spi1_sclk       ///< SPI1 Serial clock
        , spi2_ce0_n      ///< SPI2 Chip select 0
        , spi2_ce1_n      ///< SPI2 Chip select 1
        , spi2_ce2_n      ///< SPI2 Chip select 2
        , spi2_miso       ///< SPI2 MISO
        , spi2_mosi       ///< SPI2 MOSI
        , spi2_sclk       ///< SPI2 Serial clock
        , pcm_clk         ///< PCM Audio clock
        , pcm_fs          ///< PCM Audio Frame Sync
        , pcm_din         ///< PCM Audio Data in
        , pcm_dout        ///< PCM Audio data out
        , sa0             ///< Secondary memory Address bus bit 0
        , sa1             ///< Secondary memory Address bus bit 1
        , sa2             ///< Secondary memory Address bus bit 2
        , sa3             ///< Secondary memory Address bus bit 3
        , sa4             ///< Secondary memory Address bus bit 4
        , sa5             ///< Secondary memory Address bus bit 5
        , soe_n_se        ///< Secondary memory Controls
        , swe_n_srw_n     ///< Secondary memory Controls
        , sd0             ///< Secondary memory data bus bit 0
        , sd1             ///< Secondary memory data bus bit 1
        , sd2             ///< Secondary memory data bus bit 2
        , sd3             ///< Secondary memory data bus bit 3
        , sd4             ///< Secondary memory data bus bit 4
        , sd5             ///< Secondary memory data bus bit 5
        , sd6             ///< Secondary memory data bus bit 6
        , sd7             ///< Secondary memory data bus bit 7
        , sd8             ///< Secondary memory data bus bit 8
        , sd9             ///< Secondary memory data bus bit 9
        , sd10            ///< Secondary memory data bus bit 10
        , sd11            ///< Secondary memory data bus bit 11
        , sd12            ///< Secondary memory data bus bit 12
        , sd13            ///< Secondary memory data bus bit 13
        , sd14            ///< Secondary memory data bus bit 14
        , sd15            ///< Secondary memory data bus bit 15
        , sd16            ///< Secondary memory data bus bit 16
        , sd17            ///< Secondary memory data bus bit 17
        , sd1_clk         ///< ?? Appears in table 6-31 but is not described ??
        , sd1_cmd         ///< ?? Appears in table 6-31 but is not described ??
        , sd1_dat0        ///< ?? Appears in table 6-31 but is not described ??
        , sd1_dat1        ///< ?? Appears in table 6-31 but is not described ??
        , sd1_dat2        ///< ?? Appears in table 6-31 but is not described ??
        , sd1_dat3        ///< ?? Appears in table 6-31 but is not described ??
        , arm_trst        ///< ARM JTAG reset
        , arm_rtck        ///< ARM JTAG return clock
        , arm_tdo         ///< ARM JTAG Data out
        , arm_tck         ///< ARM JTAG Clock
        , arm_tdi         ///< ARM JTAG Data in
        , arm_tms         ///< ARM JTAG Mode select     
        };

      /// @brief Immutable type for values describing a pin's alternative
      /// function.
      ///
      /// Immutable here allows object to be changed only by assignment to
      /// another descriptor object as required by std containers.
      ///
      /// A GPIO pin's alternative functions are defined by:
      ///   - the pin's id
      ///   - the pin alternative function mode (alt0...alt5)
      ///   - the special function the pin has in the alternative function mode
        class descriptor
        {
          pin_id          pin_;
          gpio_pin_fn     alt_fn_;
          gpio_special_fn special_fn_;

        public:
        /// @brief Construct from individual values
        /// @param p  Pin id of GPIO pin alt function that is described
        /// @param a  Alt function of GPIO pin that is described
        /// @param s  Special function performed by pin in alt mode described.
        /// @throws std::invalid_argument if gpio_pin_fn argument is 
        ///         gpio_pin_fn::input or gpio_pin_fn::output.
          descriptor(pin_id p, gpio_pin_fn a, gpio_special_fn s);

        /// @brief Return the value of the descriptor's pin_id
        /// @returns pin_id value descriptor created with.
          pin_id pin() const { return pin_; }

        /// @brief Return the value of the descriptor's alternative pin function
        /// @returns gpio_pin_fn value descriptor created with.
          gpio_pin_fn alt_fn() const { return alt_fn_; };

        /// @brief Return the value of the descriptor's special function.
        /// @returns gpio_special_fn value descriptor created with.
          gpio_special_fn special_fn() const { return special_fn_; }
        };

      /// @brief Mutable type used to build up results of pin alternative
      /// function queries
        class result_set_builder
        {
          typedef std::vector<descriptor>   descriptor_container_type;
          descriptor_container_type     items;

        public:
        /// @brief Unsigned integral type for result_set_builder size values
          typedef descriptor_container_type::size_type    size_type;

        /// @brief Number of result items held
        /// @returns Number of result items added to builder object
          size_type size() const noexcept { return items.size(); }

        /// @brief True if no result items currently held
        /// @returns true if builder contains no results, false if it does.
          bool empty() const noexcept { return items.empty(); }

        /// @brief Add copy of descriptor object to builder 
        /// @param d  Result object to add  copy of to builder object
          void add(descriptor const & d) { items.push_back(d); }

        /// @brief Add descriptor object constructed in place to builder
        /// @param p  Pin id of GPIO pin alt function of the descriptor
        /// @param a  Alt function of GPIO pin of the descriptor
        /// @param s  Special function value of the descriptor.
        /// @throws std::invalid_argument if gpio_pin_fn argument is 
        ///         gpio_pin_fn::input or gpio_pin_fn::output.
          void emplace_add(pin_id p, gpio_pin_fn a, gpio_special_fn s)
          {
            items.emplace_back(p,a,s);
          }

        /// @brief drain builder contents into a std::vector
        /// After calling drain empty()==true.
        /// @returns  std::vector<descriptor> containing builder object's
        ///           contained items.
          descriptor_container_type drain()
          {
            descriptor_container_type vessel;
            items.swap(vessel);
            return vessel;
          }
        };

      /// @brief Immutable type used to present results of pin alternative
      /// function queries
        class result_set
        {
          typedef std::vector<descriptor>   descriptor_container_type;
          descriptor_container_type     items; ///< result items collection

        public:
        /// @brief Unsigned integral type for result_set size values
          typedef descriptor_container_type::size_type        size_type;

        /// @brief Type of references to constant result_set elements
          typedef descriptor_container_type::const_reference  const_reference;

        /// @brief Type of constant iterators into the result_set
          typedef descriptor_container_type::const_iterator   const_iterator;

        /// @brief Create from result_set_builder.
        /// Theresult_set_builder object is drained thus post condition is that
        /// b.empty()==true
        /// @param  b   Builder object containing the descriptor results
          explicit result_set( result_set_builder & b )
          : items{b.drain()}
          {}

        /// @brief Number of result items held
        /// @returns Number of descriptor items in result set
          size_type size() const noexcept { return items.size(); }

        /// @brief True if no result items currently held
        /// @returns true if result set contains no items, false if it does.
          bool empty() const noexcept { return items.empty(); }

        /// @brief Element access, not bounds checked
        /// Effects undefined if n >= size().
        /// @param n  Zero-based index of item in result set.
        /// @returns Reference to item at index n. 
          const_reference operator[](size_type n) const { return items[n]; };

        /// @brief Element access, bounds checked
        /// @param n  Zero-based index of item in result set.
        /// @returns Reference to item at index n. 
        /// @throws std::out_of_range if n >= size().
          const_reference at(size_type n) const { return items.at(n); }

        /// @brief Iterator for constant access to first result
        /// @returns Iterator to first result item in result set or end()
        ///          if empty()==true.
          const_iterator begin() const noexcept { return items.begin(); }

        /// @brief Constant iterator to one past the last result
        /// @returns Iterator to one past the last item in result set
          const_iterator end() const noexcept { return items.end(); }

        /// @brief Iterator for constant access to first result
        /// @returns Iterator to first result item in result set or end()
        ///          if empty()==true.
          const_iterator cbegin() const noexcept { return items.cbegin(); }

        /// @brief Constant iterator to one past the last result
        /// @returns Iterator to one past the last item in result set
          const_iterator cend() const noexcept { return items.cend(); }
        };

      /// @brief Options used with select function overloads
        enum class select_options
        { exclude_no_fn  ///< Exclude pin/alt fn that have no special function
        , include_no_fn  ///< Include pin/alt fn that have no special function
        };

      /// @brief Select all GPIO pin, alternative function,special function data
      /// @param opt    Options. Choose whether to include data on pin/alt-fn
      ///               slots with no special function. Defaults to exclude
      ///               such slots.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin,  alternative function, special function} triples.
        result_set select(select_options opt=select_options::exclude_no_fn);

      /// @brief Select alternative function,special function data for a pin
      /// @param p      GPIO pin to return alternative function special function
      ///               data for.
      /// @param opt    Options. Choose whether to include data on pin/alt-fn
      ///               slots with no special function. Defaults to exclude
      ///               such slots.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin,  alternative function, special function} triples.
        result_set select( pin_id p
                         , select_options opt=select_options::exclude_no_fn
                         );

      /// @brief Select pin,alternative function data for a special function
      /// @param s      Special function to return pin, alternative function
      ///               data for.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select(gpio_special_fn s);

      /// @brief Select alternative function data for a pin, special function
      /// combination
      /// @param p      GPIO pin to return alternative function data for.
      /// @param s      Special function to return alternative function data for
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select(pin_id p, gpio_special_fn s);

      /// @brief Select alternative function,special function data for a
      /// set of pins
      /// @param ps     GPIO pin list to return alternative function special
      ///               function data for.
      /// @param opt    Options. Choose whether to include data on pin/alt-fn
      ///               slots with no special function. Defaults to exclude
      ///               such slots.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select( std::initializer_list<pin_id> ps
                         , select_options opt=select_options::exclude_no_fn
                         );

      /// @brief Select pin, alternative function data for a list of special
      /// functions
      /// @param ss     List of special functions to return pin, alternative
      ///               function data for.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select(std::initializer_list<gpio_special_fn> ss);


      /// @brief Select alternative function data for a pin, special function
      /// list combination
      /// @param p      GPIO pin to return alternative function  data for.
      /// @param ss     List of special functions to return alternative
      ///               function data for.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select( pin_id p
                         , std::initializer_list<gpio_special_fn> ss
                         );

      /// @brief Select alternative function data for a pin-list, special
      /// function combination
      /// @param ps     GPIO pin list to return alternative function data for.
      /// @param s      Special function to return alternative function data for
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select( std::initializer_list<pin_id> ps
                         , gpio_special_fn s
                         );

      /// @brief Select alternative function data for a pin-list, special
      /// function list combination
      /// @param ps     GPIO pin list to return alternative function data for.
      /// @param ss     List of special functions to return alternative
      ///               function data for.
      /// @returns  result_set of descriptors detailing the selected
      ///           {GPIO pin, alternative function, special function} triples.
        result_set select( std::initializer_list<pin_id> ps
                         , std::initializer_list<gpio_special_fn> ss
                         );
      } // namespace pin_alt_fn closed
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_GPIO_ALT_FN_H
