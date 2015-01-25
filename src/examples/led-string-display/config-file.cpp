// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-string-display/config-file.cpp
/// @brief Configuration file support for led-string-display.cpp
//
/// @copyright Copyright (c) Dibase Limited 2015
/// @author Ralph E. McArdell

#include "config-file.h"
#include <istream>
#include <limits>

namespace config_file
{
  constexpr char const * CommentToken = "#";

  composite_field::composite_field(composite_field const & other)
  {
    for (auto const & fld : other.fields)
      {
        for ( auto const & upv : fld.second)
          {
            this->fields[fld.first].emplace_back(new field_value(*upv));
          }
      }
  }
  
  composite_field & composite_field::operator=(composite_field const & rhs)
  {
    this->fields.clear();
    for (auto const & fld : rhs.fields)
      {
        for ( auto const & upv : fld.second)
          {
            this->fields[fld.first].emplace_back(new field_value(*upv));
          }
      }
    return *this;
  }

  void composite_field::add_field
  ( std::string const & name
  , field_value const & value
  )
  {
    this->fields[name].emplace_back(new field_value(value));
  }

  composite_field::field_list
  composite_field::at(std::string const & field_name)
  {
    auto pos(this->fields.find(field_name));
    if (pos==this->fields.end())
      {
        std::string 
                what("Configuration file composite field has no field named '");
        what += field_name;
        what += "'.";
        throw std::runtime_error(what);
      }
    field_list list;
    for (auto const & upv : pos->second)
      {
        list.emplace_back(new field_value(*upv));
      }
    return list;
  }

  std::string field_parser::get_token( std::istream & in )
  {
    std::string token;
    do
      {
        std::string raw_token;
        if (!(in >> raw_token))
          {
            if (in.eof())
              {
                break;
              }
            throw std::runtime_error("Problem reading configuration file.");
          }
        // check for comment:
        auto comment_pos(raw_token.find(CommentToken));
        if (comment_pos!=std::string::npos)
          { // have comment, remove from token and eat line as rest is comment
            token = raw_token.substr(0, comment_pos);
            in.ignore(std::numeric_limits<int>::max(),'\n'); 
          }
        else
          {
            token = raw_token;
          }
      }
    while ( token.size()==0 );
    return token;
  }

  class simple_field_parser : public field_parser
  {
  public:
    simple_field_parser(field_multiplicity m, field_presence p)
    : field_parser(m,p)
    {}

    field_value parse_field( std::istream & in ); //override;
  };

  field_value simple_field_parser::parse_field( std::istream & in )
  {
    return get_token(in);
  }

  class boolean_field_parser : public simple_field_parser
  {
  public:
    boolean_field_parser(field_multiplicity m, field_presence p)
    : simple_field_parser(m,p)
    {}

    field_value parse_field( std::istream & in ); //override;
  };

  field_value boolean_field_parser::parse_field( std::istream & in )
  {
    auto raw_value = simple_field_parser::parse_field(in).text();
    bool value{false};
    if (  raw_value=="true" || raw_value=="TRUE"
       || raw_value=="yes" || raw_value=="YES"
       || raw_value=="on" || raw_value=="ON"
       )
    {
      value = true;
    }
    else if (  raw_value=="false" || raw_value=="FALSE"
            || raw_value=="no" || raw_value=="NO"
            || raw_value=="off" || raw_value=="OFF"
            )
    {
      value = false;
    }
    else
    {
      throw std::runtime_error("Bad boolean value in configuration file.");
    }
    return value;
  }

  class integer_field_parser : public simple_field_parser
  {
  public:
    integer_field_parser(field_multiplicity m, field_presence p)
    : simple_field_parser(m,p)
    {}

    field_value parse_field( std::istream & in ); //override;
  };

  field_value integer_field_parser::parse_field( std::istream & in )
  {
    auto raw_value = simple_field_parser::parse_field(in).text();
    return std::stol(raw_value);
  }

  field_parser * composite_field_parser::get_field(std::string const & name)
  {
    auto pos(this->fields.find(name));
    if (pos==this->fields.end())
      {
        std::string 
                what("Configuration file composite field has no field named '");
        what += name;
        what += "'.";
        throw std::runtime_error(what);
      }
    return pos->second.get();
  }

  composite_field_parser & composite_field_parser::get_composite_field
  ( std::string const & name
  )
  {
    composite_field_parser * cfpp( dynamic_cast<composite_field_parser*>
                                               (this->get_field(name))
                                 );
    if (!cfpp)
      {
        std::string
                what("Configuration file composite field has no composite field named '");
        what += name;
        what += "'.";
        throw std::runtime_error(what);
      }
    return *cfpp;
  }

  void composite_field_parser::add_field
  ( std::string const & name
  , field_type type
  , field_multiplicity m
  , field_presence p
  )
  {
    auto pos(this->fields.find(name));
    if (pos!=this->fields.end())
      {
        std::string 
                what("Configuration file composite field already has a field named '");
        what += name;
        what += "'.";
        throw std::runtime_error(what);
      }
    switch ( type )
    {
    case field_type::boolean:
      this->fields[name].reset(new boolean_field_parser(m,p));
      break;
    case field_type::integer:
      this->fields[name].reset(new integer_field_parser(m,p));
      break;
    case field_type::text:
      this->fields[name].reset(new simple_field_parser(m,p));
      break;
    case field_type::composite:
      this->fields[name].reset(new composite_field_parser(m,p));
      break;
    }
  }

  field_value composite_field_parser::parse_field( std::istream & in )
  {
    std::string token(get_token(in));
    if (token!="{")
      {
        std::string
              what("Configuration file composite field: expected '{', found '");
        what += token;
        what += "'.";
        throw std::runtime_error(what);
      }
    composite_field value;
    while ( (token=get_token(in))!="}" )
      {
        auto fld_parser_ptr(this->get_field(token));
        if ( value.has_field(token)
          && fld_parser_ptr->get_multiplicity()==field_multiplicity::single
           )
          {
            std::string what("Configuration file composite field:"
                                " more than one entry for field '");
            what += token;
            what += "'.";
            throw std::runtime_error(what);
          }
        auto fld_value(fld_parser_ptr->parse_field(in));
        value.add_field(token, fld_value);
      }
    for ( auto const & entry : this->fields )
      {
        if (entry.second.get()->get_presence()==field_presence::required)
          {
            if ( !value.has_field(entry.first) )
              {
                std::string
                  what("Configuration file composite field: required field '");
                what += entry.first;
                what += "' is missing.";
                throw std::runtime_error(what);
              }
          }
      }
    return value;
  }
}
