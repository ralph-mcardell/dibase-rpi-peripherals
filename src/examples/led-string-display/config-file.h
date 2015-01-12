// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-string-display/config-file.h
/// @brief Configuration file support for led-string-display.cpp
//
/// @copyright Copyright (c) Dibase Limited 2015
/// @author Ralph E. McArdell

#ifndef LED_STRING_DISPLAY_CONFIG_FILE_H
# define LED_STRING_DISPLAY_CONFIG_FILE_H

# include <vector>
# include <map>
# include <memory>
# include <string>
# include <stdexcept>

namespace config_file
{
  ///< Configuration file parser field multiplicity
  enum class field_multiplicity { single, repeated };

  ///< Configuration file parser field presence requirements
  enum class field_presence { required, optional };

  class field_value;

  class composite_field
  {
    friend class composite_field_parser;

  public:
    typedef std::vector<std::unique_ptr<field_value>> field_list;

  private:
    typedef std::map<std::string, field_list> field_dictionary;

    field_dictionary fields;

    void add_field(std::string const & name, field_value const & value);

  public:
    composite_field() {}
    composite_field(composite_field const & other);
    composite_field & operator=(composite_field const & rhs);
 
    bool has_field( std::string const & name)
    {
      return this->fields.find(name)!=this->fields.end();
    }

    field_list at(std::string const & field_name);
    field_list operator[](std::string const & field_name)
    {
      return this->at(field_name);
    }
  };
  
  enum class field_type { composite, boolean, integer, text };
  class field_value
  {  
    field_type type;
    union 
    {
      composite_field * c;
      std::string s;
      long        i;
      bool        b;
    };

  public:
    field_value( std::string const & str )
    : type(field_type::text)
    , s(str)
    {
    }

    field_value( bool b )
    : type(field_type::boolean)
    , b(b)
    {
    }

    field_value( long l )
    : type(field_type::integer)
    , i(l)
    {
    }
    field_value( composite_field const & c )
    : type(field_type::composite)
    , c(new composite_field(c))
    {
    }

    field_value( field_value const & other )
    : type(other.type)
    {
      switch ( type )
      {
      case field_type::boolean:   b = other.b; break;
      case field_type::integer:   i = other.i; break;
      case field_type::text:      s = other.s; break;
      case field_type::composite:
        {
          c = new composite_field(*other.c);
          break;
        }
      }
    }
    
    ~field_value() 
    {
      if (type==field_type::composite)
        {
          delete c;
        }
    }

    std::string string()
    {
      if (type==field_type::text)
      {
        return s;
      }
      throw std::logic_error
            ("String field value requested for non-string field_value.");
    }

    long integer()
    {
      if (type==field_type::integer)
      {
        return i;
      }
      throw std::logic_error
            ("Integer field value requested for non-integer field_value.");
    }

    bool boolean()
    {
      if (type==field_type::boolean)
      {
        return b;
      }
      throw std::logic_error
            ("Boolean field value requested for non-boolean field_value.");
    }

    composite_field composite()
    {
      if (type==field_type::composite)
      {
        return *c;
      }
      throw std::logic_error
            ("Composite field value requested for non-composite field_value.");
    }
  };

  class field_parser
  {
    field_multiplicity  multiplicity;
    field_presence      presence;

  protected:
    static std::string get_token( std::istream & in );

  public:
    field_parser(field_multiplicity m, field_presence p)
    : multiplicity(m)
    , presence(p)
    {}

    virtual field_value parse_field( std::istream & in ) = 0;
    
    field_multiplicity get_multiplicity() { return this->multiplicity; }
    field_presence     get_presence()     { return this->presence; }
  };

  class composite_field_parser : public field_parser
  {
    typedef std::unique_ptr<field_parser>           field_parser_ptr;
    typedef std::map<std::string,field_parser_ptr>  field_dictionary;
    
    field_dictionary  fields;
    
    field_parser * get_field(std::string const & name );

  public:
    composite_field_parser(field_multiplicity m, field_presence p)
    : field_parser(m,p)
    {}

    void add_field(std::string const & name, field_type type, field_multiplicity m, field_presence p);
    
    composite_field_parser & get_composite_field(std::string const & name);

    field_value parse_field( std::istream & in ); //override;
  };
}
#endif // LED_STRING_DISPLAY_CONFIG_FILE_H
