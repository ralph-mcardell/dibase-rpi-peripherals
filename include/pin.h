// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin.h 
/// @brief Use a GPIO pin for input or output : class definitions.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_H
# define DIBASE_RPI_PERIPHERALS_PIN_H

# include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Base class for I/O direction specific GPIO classes
  /// \ref opin and \ref ipin.
  ///
  /// Specifies public and protected member functions providing common
  /// functionality to the sub-types. Also stores, and provides read only
  /// access to sub classes, to the pin_id associated with an open single
  /// GPIO pin I/O object.
    class pin_base
    {
    protected:
    /// @brief open data direction mode flag enumerations
      enum direction_mode
      { in=1      ///< Direction mode value for data input
      , out=2     ///< Direction mode value for data output
      };

    /// @brief Initialise pin to being closed.
      pin_base(pin_id pin, direction_mode dir);

    /// @brief Destroy pin object - deallocates pin
      ~pin_base();

      pin_base(pin_base const &) = delete;
      pin_base& operator=(pin_base const &) = delete;
      pin_base(pin_base &&) = delete;
      pin_base& operator=(pin_base &&) = delete;

    /// @brief Returns the pin id of the GPIO pin open on this object.
      pin_id get_pin() const
      {
        return pin;
      }

    private:
      pin_id pin;     ///< Open GPIO pin's id.
    };

  /// @brief Use a single GPIO pin for output.
  ///
  /// Provides explicit open member function to open a single GPIO pin for
  /// output of Boolean values via a put member function. In addition provides
  /// a default constructor and a constructor that opens a pin for output and
  /// will also close an open pin on destruction.
    class opin : public pin_base
    {
    public:
    /// @brief Create and open a GPIO pin for output
    /// @param[in]  pin   Id of GPIO pin to open for output.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      explicit opin( pin_id pin )
      : pin_base(pin, direction_mode::out)
      {}
 
    /// @brief Set an open output pin to the specified state
    /// @param[in]  v Value to output:  true to set pin state high,
    ///                                 false set pin state low
    ///               Note: outputs nothing if pin is not open.
      void put( bool v );
    };

  /// @brief Use a single GPIO pin for input.
  ///
  /// Provides explicit open member function to open a single GPIO pin for
  /// input as a Boolean value via a get member function. In addition provides
  /// a default constructor and a constructor that opens a pin for input and
  /// will also close an open pin on destruction.
    class ipin : public pin_base
    {
    friend class pin_edge_event;///< ipin objects can be associated with events

    public:
    /// @brief Input open mode flag enumerations
      enum open_mode
      { pull_disable = 0    ///< No pull up or pull down mode value
      , pull_up = 1         ///< Apply pull up resistor mode value
      , pull_down=2         ///< Apply pull down resistor mode value
      };

    /// @brief Create and open a GPIO pin for input
    /// @param[in]  pin   Id of GPIO pin to open for input.
    /// @param[in]  mode  Open mode for input pin. Specifies pull up/down
    ///                   mode, default is pull_disable (no pull).
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      explicit ipin( pin_id pin, unsigned mode=0 );

    /// @brief Destroy pin object, closing it.
      ~ipin();

    /// @brief Return the current state of open input pin
    /// @return true if pin is in a high state
    ///         false if pin is in a low state
    ///         Always returns false if pin is not open.
      bool get();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_H
