// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file pin.h 
/// @brief Single GPIO pin I/O class specifications.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#ifndef DIBASE_RPI_PERIPHERALS_PIN_H
 #define DIBASE_RPI_PERIPHERALS_PIN_H

 #include "pin_id.h"

namespace dibase { namespace rpi {
  namespace peripherals
  {
  /// @brief Base class for I/O direction specific GPIO classes opin and ipin.
  ///
  /// Specifies public and protected member functions providing common
  /// functionality to the sub-types. Also stores, and provides read only
  /// access to sub classes, to the pin_id associated with an open single
  /// GPIO pin I/O object.
    class pin_base
    {
    public:
    /// @brief Query open state of pin objects.
    /// @return true if pin object is open, false otherwise. 
      bool is_open() const  { return open_flag; }

    /// @brief Close an open pin object. It is OK to close a closed pin.
      void close();
    protected:
    /// @brief Protected pin_base constructor initialises pin to being closed.
      pin_base()
      : pin(0)
      , open_flag(false)
      {}

    /// @brief open data direction mode flag enumerations
      enum direction_mode
      { in=1
      , out=2
      };

    /// @brief Common pin object open logic. 
    /// Allocates pin and sets pin's GPIO function to input or output as
    /// specified by the mode parameter.
    /// @param[in]  pin   Id of GPIO pin to open.
    /// @param[in]  dir   Data direction mode flag:
    ///                   in for input pins, out for output pins.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      void open( pin_id pin, direction_mode dir );

    /// @brief Returns the pin id of the GPIO pin open on this object.
      pin_id get_pin() const
      {
        return pin;
      }

    private:
      pin_id pin;     ///< Open GPIO pin's id.
      bool open_flag; ///< Flag tracking open/close state of pin objects
    };

  /// @brief Single GPIO pin class for output.
  ///
  /// Provides explicit open member function to open a single GPIO pin for
  /// output of Boolean values via a put member function. In addition provides
  /// a default constructor and a constructor that opens a pin for output and
  /// will also close an open pin on destruction.
    class opin : public pin_base
    {
    public:
    /// @brief Create output pin closed to be opened later
      opin() = default;

    /// @brief Create and open a GPIO pin for output
    /// @param[in]  pin   Id of GPIO pin to open for output.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      explicit opin( pin_id pin )
      {
        open( pin );
      }

    /// @brief Destroy pin object, closing it.
      ~opin()
      {
        pin_base::close();
      }

    /// @brief Open pin for output.
    /// @param[in]  pin   Id of GPIO pin to open for output.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
     void open(pin_id pin)
      {
        pin_base::open(pin, out);
      }

    /// @brief Set an open output pin to the specified state
    /// @param[in]  v Value to output:  true to set pin state high,
    ///                                 false set pin state low
    ///               Note: outputs nothing if pin is not open.
      void put( bool v );
    };

  /// @brief Single GPIO pin class for input.
  ///
  /// Provides explicit open member function to open a single GPIO pin for
  /// input as a Boolean value via a get member function. In addition provides
  /// a default constructor and a constructor that opens a pin for input and
  /// will also close an open pin on destruction.
    class ipin : public pin_base
    {
    friend class pin_edge_event;
    public:
    /// @brief Input open mode flag enumerations
      enum open_mode
      { pull_disable = 0
      , pull_up = 1
      , pull_down=2
      };

    /// @brief Create input pin closed to be opened later
      ipin() = default;

    /// @brief Create and open a GPIO pin for input
    /// @param[in]  pin   Id of GPIO pin to open for input.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      explicit ipin( pin_id pin, unsigned mode=0 )
      {
        open( pin, mode );
      }

    /// @brief Destroy pin object, closing it.
      ~ipin();

    /// @brief Open pin for input.
    /// @param[in]  pin   Id of GPIO pin to open for input.
    /// @exception  bad_pin_alloc if the GPIO pin is in use by this process
    ///             or elsewhere.
      void open(pin_id pin, unsigned mode=0);

    /// @brief Return the current state of open input pin
    /// @return true if pin is in a high state
    ///         false if pin is in a low state
    ///         Note: Always returns false if pin is not open.
      bool get();
    };
  } // namespace peripherals closed
}} // namespaces rpi and dibase closed
#endif // DIBASE_RPI_PERIPHERALS_PIN_H
