// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pwm_registers.h 
/// @brief \b Internal : low-level (GPIO) PWM control registers type definition.
///
/// Refer to the
/// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
/// Broadcom BCM2835 ARM Peripherals Datasheet</a> Chapter 9 Pulse Width
/// Modulator for details along with additional information on PWM found in 
/// wiringpi and Gertboard source code..
///
/// @copyright Copyright (c) Dibase Limited 2013
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_REGISTERS_H
# define DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_REGISTERS_H

# include "peridef.h"

namespace dibase { namespace rpi {
  namespace peripherals
  { namespace internal
    {
    /// @brief Strong enumeration type for PWM channels
    ///
    /// There are two channels known, confusingly, either as GPIO special
    /// functions PWM0, PWM1 or PWM channel 1, PWM channel 2 in the PWM
    /// controller registers descriptions.
      enum class pwm_channel : register_t
      { gpio_pwm0 = 0   ///< GPIO function nomenclature for first PWM channel
      , gpio_pwm1 = 1   ///< GPIO function nomenclature for second PWM channel
      , pwm_ch1 = 0     ///< PWM channel nomenclature for first PWM channel
      , pwm_ch2 = 1     ///< PWM channel nomenclature for second PWM channel
      };

    /// @brief Strong enumeration type for PWM modes
    ///
    /// The PWM controller supports two main modes: PWM and serialiser.
      enum class pwm_mode : register_t
      { pwm = 0         ///< PWM controller PWM mode
      , serialiser = 1  ///< PWM controller serialiser mode
      };
      
    /// @brief Represents layout of PWM control registers with operations.
    ///
    /// Permits access to BCM2835 PWM controller registers when an 
    /// instance is mapped to the correct physical memory location.
    ///
    /// See the
    /// <a href="http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf">
    /// Broadcom BCM2835 ARM Peripherals Datasheet</a> chapter 9 for published
    /// details. PWM clock control and divisor register offsets gleaned from
    /// Gertboard gb_pwm.h source code.
    ///
    /// Note that there is some confusion with regard to PWM channel numbering.
    /// While the GPIO pins special functions refer to PWM channels 0 and 1
    /// (PWM0, PWM1), the PWM control registers use PWM channels 1 and 2 - go
    /// figure. PWM0 maps onto PWM channel 1 and PWM1 to channel 2.
    ///
    /// Member function operations are provided to query and set the various
    /// fields and flags for PWM channel control.
      struct pwm_registers
      {
      private:
        enum
        { ctl_enable    = 0x1   ///< Control register: PWM channel enable
        , ctl_mode_ser  = 0x2   ///< Control register: serialiser mode
        , ctl_rptl      = 0x4   ///< Control register: repeat last data
        , ctl_sbit      = 0x8   ///< Control register: silence state 1s
        , ctl_pola      = 0x10  ///< Control register: reverse polarity
        , ctl_usef      = 0x20  ///< Control register: Use FIFO
        , ctl_clrf      = 0x40  ///< Control register: Clear FIFO
        , ctl_msen      = 0x80  ///< Control register: Use M/S algorithm
        , ctl_ch2_shift = 8U    ///< Shift for control bits for PWM1 / channel 2
        , sta_fifo_full = 0x1   ///< Status register: FIFO full flag
        , sta_fifo_empt = 0x2   ///< Status register: FIFO empty flag
        , sta_fifo_werr = 0x4   ///< Status register: FIFO write full error flag
        , sta_fifo_rerr = 0x8   ///< Status register: FIFO read empty error flag
        , sta_berr      = 0x100 ///< Status register: Bus error flag
        , sta_gapo      = 0x10  ///< Status register: Channel gap occurred flag
        , sta_state     = 0x200 ///< Status register: Channel state flag
        , sta_ch2_shift = 1U    ///< Shift for status bits for PWM1 / channel 2
        , dma_enable = (1U<<31U)///< PWM DMA (DMA channel 5) enable flag
        , dma_dreq_mask = 0xff  ///< PWM DMA data request threshold field mask
        , dma_dreq_max  = 0xff  ///< PWM DMA data request threshold field maximum
        , dma_panic_mask= 0xff00///< PWM DMA panic threshold field mask
        , dma_panic_max = 0xff  ///< PWM DMA panic threshold field maximum
        , dma_panic_shift = 8U  ///< PWM DMA panic threshold field bits-shift
        };

        constexpr static register_t ctl_ch_shift(pwm_channel ch, register_t v)
        { 
          return ch==pwm_channel::pwm_ch2 ? (v<<ctl_ch2_shift) : v;
        }

        constexpr static register_t sta_ch_shift(pwm_channel ch, register_t v)
        { 
          return ch==pwm_channel::pwm_ch2 ? (v<<sta_ch2_shift) : v;
        }

      public:
      /// @brief Physical address of start of BCM2835 PWM control registers
        constexpr static physical_address_t 
                              physical_address = peripheral_base_address+0x20C000;
        register_t control;     ///< PWM control register, CTL
        register_t status;      ///< PWM status register, STA
        register_t dma_config;  ///< PWM DMA configuration register, DMAC
        register_t reserved_do_not_use_0; ///< Reserved, currently unused
        register_t range1;      ///< PWM0 (channel 1) range register, RNG1
        register_t data1;       ///< PWM0 (channel 1) data register, DAT1
        register_t fifo_in;     ///< PWM FIFO input register FIF1
        register_t reserved_do_not_use_1; ///< Reserved, currently unused
        register_t range2;      ///< PWM1 (channel 2) range register, RNG2
        register_t data2;       ///< PWM1 (channel 2) data register, DAT2

      /// @brief Return value of \ref control register PWENi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return enable state for
      /// @returns true if channel control register PWEN bit set, false if not.
        bool get_enable(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_enable);
        }

      /// @brief Return value of \ref control register PWENi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return mode for
      /// @returns pwm_mode::pwm if channel is in PWM mode or
      ///          pwm_mode::serialiser if the channel is in serialiser mode.
        pwm_mode get_mode(pwm_channel ch) volatile const
        {
          return (control & ctl_ch_shift(ch,ctl_mode_ser))  ? pwm_mode::serialiser 
                                                        : pwm_mode::pwm;
        }

      /// @brief Return value of \ref control register RPTLi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return repeat last data state for
      /// @returns true if channel control register RPTL bit set, false if not.
        bool get_repeat_last_data(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_rptl);
        }

      /// @brief Return value of \ref control register SBITi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return silence (no transmission) state for
      /// @returns true if channel control register SBIT bit set, false if not.
        bool get_silence(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_sbit);
        }

      /// @brief Return value of \ref control register POLAi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return polarity inverted state
      /// @returns true if channel control register POLA bit set, false if not.
        bool get_polarity_inverted(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_pola);
        }

      /// @brief Return value of \ref control register USEFi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return using FIFO state
      /// @returns true if channel control register USEF bit set, false if not.
        bool get_use_fifo(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_usef);
        }

      /// @brief Return value of \ref control register MSENi bit for specified
      /// channel.
      /// @param ch  PWM channel id to return M/S PWM algorithm enabled state
      /// @returns true if channel control register MSEN bit set, false if not.
        bool get_ms_enabled(pwm_channel ch) volatile const
        {
          return control & ctl_ch_shift(ch,ctl_msen);
        }

      /// @brief Clear the FIFO. There is only 1 FIFO, no channel parameter
      /// needed.
        void clear_fifo() volatile
        {
          control |= ctl_clrf;
        }

      /// @brief Set value of \ref control register PWENi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set enable state for
      /// @param state  Enable state to set: true : enabled; false : disabled
        void set_enable(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_enable)
                          : control & ~ctl_ch_shift(ch,ctl_enable);
        }

      /// @brief Set value of \ref control register PWENi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set mode for
      /// @param mode   Mode to set channel to
        void set_mode(pwm_channel ch, pwm_mode mode) volatile
        {
          control = mode==pwm_mode::serialiser
                                        ? control|ctl_ch_shift(ch,ctl_mode_ser)
                                        : control&~ctl_ch_shift(ch,ctl_mode_ser);
        }

      /// @brief Set value of \ref control register RPTLi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set repeat last data for
      /// @param state  Repeat last data state to set. true: on; false: off
        void set_repeat_last_data(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_rptl)
                          : control & ~ctl_ch_shift(ch,ctl_rptl);
        }

      /// @brief Set value of \ref control register SBITi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set silence bits state for
      /// @param state  Silence bit state: true: 1; false: 0
        void set_silence(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_sbit)
                          : control & ~ctl_ch_shift(ch,ctl_sbit);
        }

      /// @brief Set value of \ref control register POLAi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set polarity invert state for
      /// @param state  Polarity invert state: true: invert bits; false: do not
        void set_polarity_inverted(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_pola)
                          : control & ~ctl_ch_shift(ch,ctl_pola);
        }

      /// @brief Set value of \ref control register USEFi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set use-FIFO for transmission state
      /// @param state  Use FIFO state: true: use FIFO; false: use data register 
        void set_use_fifo(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_usef)
                          : control & ~ctl_ch_shift(ch,ctl_usef);
        }

      /// @brief Set value of \ref control register MSENi bit for specified
      /// channel.
      /// @param ch     PWM channel id to set M/S algorithm enabled state for
      /// @param state  M/S enabled state: true: use M/S algorithm; 
      ///                                  false: use PWM algorithm
        void set_ms_enabled(pwm_channel ch, bool state) volatile
        {
          control = state ? control | ctl_ch_shift(ch,ctl_msen)
                          : control & ~ctl_ch_shift(ch,ctl_msen);
        }


      /// @brief Returns state of \ref status register FULL1 flag
      /// @returns true if FIFO full; false if not
        bool get_fifo_full() volatile const
        {
          return status & sta_fifo_full;
        }

      /// @brief Returns state of \ref status register EMPT1 flag
      /// @returns true if FIFO empty; false if not
        bool get_fifo_empty() volatile const
        {
          return status & sta_fifo_empt;
        }

      /// @brief Returns state of \ref status register WERR1 flag
      /// @returns true if error writing to FIFO; false if no such error
        bool get_fifo_write_error() volatile const
        {
          return status & sta_fifo_werr;
        }

      /// @brief Returns state of \ref status register RERR1 flag
      /// @returns true if error reading from FIFO; false if no such error
        bool get_fifo_read_error() volatile const
        {
          return status & sta_fifo_rerr;
        }

      /// @brief Returns state of \ref status register BERR1 flag
      /// @returns true if error writing to registers via APB; false if not
        bool get_bus_error() volatile const
        {
          return status & sta_berr;
        }

      /// @brief Return value of \ref status register GAPOi flag for specified
      /// channel.
      /// @param ch  PWM channel id to return gap occurred flag for
      /// @returns true if gap in transmission occurred; false if not 
        bool get_gap_occurred(pwm_channel ch) volatile const
        {
          return status & sta_ch_shift(ch,sta_gapo);
        }

      /// @brief Return value of \ref status register STAi flag for specified
      /// channel.
      /// @param ch  PWM channel id to return transmitting data state flag for
      /// @returns true if channel transmitting data; false if it is not 
        bool get_txd_state(pwm_channel ch) volatile const
        {
          return status & sta_ch_shift(ch,sta_state);
        }

      /// @brief Clear a FIFO write error state (get_fifo_write_error()==true)
        void clear_fifo_write_error() volatile
        {
          status |= sta_fifo_werr;
        }

      /// @brief Clear a FIFO read error state (get_fifo_read_error()==true)
        void clear_fifo_read_error() volatile
        {
          status |= sta_fifo_rerr;
        }

      /// @brief Clear a bus error state (get_bus_error()==true)
        void clear_bus_error() volatile
        {
          status |= sta_berr;
        }

      /// @brief Clear a channel gap occurred state (get_gap_occurred(ch)==true)
      /// @param ch  PWM channel id to clear gap occurred flag for
        void clear_gap_occurred(pwm_channel ch) volatile
        {
          status |= sta_ch_shift(ch,sta_gapo);
        }


      /// @brief Returns state of \ref dma_config register ENAB flag
      /// @returns true if PWM DMA enabled; false if not
        bool get_dma_enable() volatile const
        {
          return dma_config & dma_enable;
        }

      /// @brief Returns value of \ref dma_config DREQ field
      /// @returns DMA threshold for data request signal (0-255)
        register_t get_dma_data_req_threshold() volatile const
        {
          return dma_config & dma_dreq_mask;
        }

      /// @brief Returns value of \ref dma_config PANIC field
      /// @returns DMA threshold for panic signal (0-255)
        register_t get_dma_panic_threshold() volatile const
        {
          return (dma_config & dma_panic_mask)>>dma_panic_shift;
        }

      /// @brief Set value of \ref dma_config ENAB flag.
      /// @param state  DMA enable state to set: true: enabled; false: disabled
        void set_dma_enable(bool state) volatile
        {
          dma_config = state  ? dma_config | dma_enable
                              : dma_config & ~dma_enable;
        }

      /// @brief Set value of \ref dma_config DREQ field.
      /// @param value  PWM DMA data request signal threshold value to set
      ///               (0-255)
      /// @returns  true if value in range and operation performed,
      ///           false otherwise
        bool set_dma_data_req_threshold(register_t value) volatile
        {
          if (value>dma_dreq_max)
            {
              return false;
            }
          dma_config = (dma_config & ~dma_dreq_mask)|value;
          return true;
        }

      /// @brief Set value of \ref dma_config PANIC field.
      /// @param value  PWM DMA panic signal threshold value to set (0-255)
      /// @returns  true if value in range and operation performed,
      ///           false otherwise
        bool set_dma_panic_threshold(register_t value) volatile
        {
          if (value>dma_panic_max)
            {
              return false;
            }
          dma_config = (dma_config & ~dma_panic_mask)|(value<<dma_panic_shift);
          return true;
        }


      /// @brief Return PWM channel PWM_RNGi field value of \ref range1 or 
      /// \ref range2 register.
      /// specified channel
      /// @param ch PWM channel id to return range register value for
      /// @returns  Value of \ref range1 or \ref range2 register PWM_RNGi
      ///           field (0-0xffffffff)
        register_t get_range(pwm_channel ch) volatile const
        {
          return ch==pwm_channel::pwm_ch1 ? range1 : range2;
        }

      /// @brief Return PWM channel PWM_DATi field value of \ref data1 or 
      /// \ref data2 register.
      /// @param ch PWM channel id to return data register value for
      /// @returns  Value of \ref data1 or \ref data2 register PWM_DATi
      ///           field (0-0xffffffff)
        register_t get_data(pwm_channel ch) volatile const
        {
          return ch==pwm_channel::pwm_ch1 ? data1 : data2;
        }

      /// @brief Set value of the FIFO input register FIF1 single PWM_FIFO field
      /// @param value  Value to set FIFO input register to (0...0xffffffff)
        void set_fifo_input(register_t value) volatile
        {
          fifo_in = value;
        }

      /// @brief Set value of the channel range registers RNGi
      /// @param ch     PWM channel id to set range register value for
      /// @param value  Value to set \ref range1 or \ref range2 register
      ///               to (0...0xffffffff)
        void set_range(pwm_channel ch, register_t value) volatile
        {
          if (ch==pwm_channel::pwm_ch1)
            {
              range1 = value;
            }
          else
            {
              range2 = value;
            }
        }

      /// @brief Set value of the channel data registers DATi
      /// @param ch     PWM channel id to set data register value for
      /// @param value  Value to set \ref data1 or \ref data2 register
      ///               to (0...0xffffffff)
        void set_data(pwm_channel ch, register_t value) volatile
        {
          if (ch==pwm_channel::pwm_ch1)
            {
              data1 = value;
            }
          else
            {
              data2 = value;
            }
        }
      };
    } // namespace internal closed
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_INTERNAL_PWM_REGISTERS_H
