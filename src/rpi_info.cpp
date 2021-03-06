// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file rpi_info.cpp 
/// @brief Implementation and initialisation for rpi_info class functions.
///
/// @copyright Copyright (c) Dibase Limited 2012
/// @author Ralph E. McArdell

#include "rpi_info.h"
#include "rpi_init.h"
#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <cctype>

 namespace dibase {
  namespace rpi
  {
    namespace internal
    {
    /// @cond
    // A base rpi_init object is used to initialise the data for rpi_info
    // objects for production use. Tests may subclass rpi_init, override
    // virtual initialisation methods as appropriate, and replace the
    // rpi_initialiser pointer with a pointer to an instance of their test
    // initialiser.
      rpi_init production_code_initialiser;
      rpi_init * rpi_initialiser{&production_code_initialiser};
    /// @endcond

      static char const * get_label
      ( FILE * file
      , char * buffer
      , std::size_t buffer_size
      , char const * eof_label
      )
      {
        char const space{' '};
        if (file)
          {
            int ch;
            std::size_t count{0};
            while ( (ch=fgetc(file))!=EOF&&ch!=space&&count<buffer_size )
              {
                buffer[count++] = ch;
              }
            if (ch!=EOF)
              {
                buffer[count] = '\0';
                return buffer;
              }
          }
        return eof_label;
      }

      static void skip_to_next_line( FILE * file )
      {
        if ( file )
        {
          int ch;
          while ( (ch=fgetc(file))!=EOF&&ch!='\n' )
            ; // intentional do-nothing loop body
        }
      }

      static std::size_t extract_version( FILE * file )
      {
        std::size_t version{0}; // start with bad version no
        if ( file )
          {
            int ch;
            while ( (ch=fgetc(file))!=EOF&&ch!='\n'&&!isdigit(ch) )
              ; // intentional do-nothing loop body
            char const * hex_digits("abcdef");
            do
              {
                if ( isdigit(ch) )
                  {
                    version *= 16;
                    version += ch - '0';
                  }
                else if ( char const * hexdigit_ptr = std::strchr(hex_digits, std::tolower(ch)) )
                  {
                    version *= 16;
                    version += 10 + (hexdigit_ptr-hex_digits);
                  }
                else // oops something unexpected: return an invalid version
                  {
                    version = 0;
                    break;
                  } 
              }
            while ( (ch=fgetc(file))&&isalnum(ch) );
          }
        return version;
      }

      static std::size_t one_time_init_major_version()
      {
        std::size_t version{0};
        char const * cpu_info_path{"/proc/cpuinfo"};
        char const board_version_label[]{"Revision"};
        char const * eof_label{"###"};
        std::size_t const board_version_size{sizeof(board_version_label)};
        bool not_done{true};
        char buffer[board_version_size];
        std::size_t const board_version_length{board_version_size-1};
        char const * label{nullptr};
        FILE * info_file{fopen(cpu_info_path, "r")};
        do
          {
            label = get_label(info_file, buffer, board_version_length, eof_label);
            not_done = std::strcmp(label, eof_label)!=0;
            if (not_done)
              {
                not_done = std::strcmp(label, board_version_label)!=0;
                if (not_done)
                  {
                    skip_to_next_line( info_file );
                  }
                else
                  {
                    version = extract_version( info_file );
                  }
              }
          }
        while (not_done);
        if ( info_file )
          {
            fclose( info_file );
          }
        if (version>0&&version<=3) return 1;
        else if (version>3&&version<=0xf) return 2;
        else if (version==0x10 || version==0x12) return 3; // 0x10:B+, 0x12:A+
        else if (version==0x11) return 4; // Compute module
        else
          {
            throw std::runtime_error( "rpi_init::init_major_version: Unable to "
                                      "deduce board version from /proc/cpuinfo."
                                    );
          }
      }

      std::size_t rpi_init::init_major_version()
      {
        static std::size_t major_version{one_time_init_major_version()};
        return major_version;
      }
    } // Closing namespace internal

    rpi_info::rpi_info()
    : major_version_value(internal::rpi_initialiser->init_major_version())
    {
    }
  }
}
