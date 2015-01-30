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
  /// @brief Configuration file parser field multiplicity
  enum class field_multiplicity { single, repeated };

  /// @brief Configuration file parser field presence requirements
  enum class field_presence { required, optional };

  /// @brief Bare bones variant type for configuration field values
  ///
  /// Stores bool, long, pointer to std::string or pointer to composite_field
  /// as a union, together with a field_type value to form a discriminated
  /// union type. 
  ///
  /// Only permits values to be set on value construction or copy construction
  /// (no assignment operator) and accessed via accessors.
  class field_value;

  /// @brief A configuration field composed of multiple named fields
  ///
  /// Field values are held as field_value instances, potentially including
  /// other composite fields. Each named field is mapped to a collection of
  /// field values to support fields which may occur more than once (repeated
  /// fields).
  class composite_field
  {
    friend class composite_field_parser;

  public:
    /// @brief Type-alias for collection of field values type
    typedef std::vector<std::unique_ptr<field_value>> field_list;

  private:
    typedef std::map<std::string, field_list> field_dictionary;

    field_dictionary fields;

    void add_field(std::string const & name, field_value const & value);

  public:
    /// @brief Default construct: creates empty composite_field
    composite_field() {}

    /// @brief Copy construct: makes deep copy of other composite_field instance
    /// @param other  Reference to other composite_field to copy
    composite_field(composite_field const & other);

    /// @brief Assignment: Copy-assign from another composite_field
    /// @param rhs  reference to composite_field on right hand side of operator
    composite_field & operator=(composite_field const & rhs);
 
    /// @brief Check if composite_field contains a specific field
    /// @param name   Name of field to check existence of
    /// @returns true if instance has field with the passed name, false if not.
    bool has_field( std::string const & name )
    {
      return this->fields.find(name)!=this->fields.end();
    }

    /// @brief Return field value collection for given field name
    /// @param field_name   Name of field to return values for
    /// @returns field_list of field_values for field. The returned list
    ///          is completely separate from that contained in the
    ///          composite_field instance.
    /// @throws std::runtime_error if object does not contain a field
    ///         named field_name.
    field_list at(std::string const & field_name);

    /// @brief Synonym for composite_field::at.
    field_list operator[](std::string const & field_name)
    {
      return this->at(field_name);
    }
  };

  /// @brief field type ids used by field_value
  enum class field_type { composite, boolean, integer, text };

  class field_value
  {  
    field_type type;
    union 
    {
      composite_field * c;
      std::string * s;
      long        i;
      bool        b;
    };

  public:
    /// @brief Construct text field_value from string
    /// Note: takes copy of passed string
    /// @param str  String value to initialise field_value with
    field_value( std::string const & str )
    : type(field_type::text)
    , s(new std::string(str))
    {
    }

    /// @brief Construct boolean field_value from bool
    /// @param b  Boolean value to initialise field_value with
    field_value( bool b )
    : type(field_type::boolean)
    , b(b)
    {
    }

    /// @brief Construct integer field_value from long
    /// @param l  Long integer value to initialise field_value with
    field_value( long l )
    : type(field_type::integer)
    , i(l)
    {
    }

    /// @brief Construct composite field_value from composite_field
    /// Note: takes copy of passed composite_field
    /// @param c  composite_field value to initialise field_value with
    field_value( composite_field const & c )
    : type(field_type::composite)
    , c(new composite_field(c))
    {
    }

    /// @brief Copy construction
    /// Note: makes copies of text and composite field values
    /// @param other  Other field_value to copy from
    field_value( field_value const & other )
    : type(other.type)
    {
      switch ( type )
      {
      case field_type::boolean:   b = other.b; break;
      case field_type::integer:   i = other.i; break;
      case field_type::text:      
      {
        s = new std::string(*other.s);
        break;
      }
      case field_type::composite:
        {
          c = new composite_field(*other.c);
          break;
        }
      }
    }

    /// @brief Destroy field_value
    /// Deletes value if value is text or composite
    ~field_value() 
    {
      if (type==field_type::composite)
        {
          delete c;
        }
      else if (type==field_type::text)
        {
          delete s;
        }
    }

    /// @brief Return copy of text field_value's string
    /// @returns Copy of text field's string value
    /// @throws std::logic_error if field_value does not contain a string
    std::string text() const
    {
      if (type==field_type::text)
      {
        return *s;
      }
      throw std::logic_error
            ("Text field value requested for non-text field_value.");
    }

    /// @brief Return value of integer field_value's long integer value
    /// @returns integer field_value's long integer value
    /// @throws std::logic_error if field_value does not contain an integer
    long integer() const
    {
      if (type==field_type::integer)
      {
        return i;
      }
      throw std::logic_error
            ("Integer field value requested for non-integer field_value.");
    }

    /// @brief Return value of boolean field_value'sbool value
    /// @returns boolean field_value's bool value
    /// @throws std::logic_error if field_value does not contain a bool
    bool boolean() const
    {
      if (type==field_type::boolean)
      {
        return b;
      }
      throw std::logic_error
            ("Boolean field value requested for non-boolean field_value.");
    }

    /// @brief Return copy of composite field_value's composite_field
    /// @returns Copy of composite field's composite_field value
    /// @throws std::logic_error if field_value does not contain a
    ///         composite_field
    composite_field composite() const
    {
      if (type==field_type::composite)
      {
        return *c;
      }
      throw std::logic_error
            ("Composite field value requested for non-composite field_value.");
    }
  };

  /// @brief Base type for parsing configuration fields
  /// Declares/defines operations and state common for all field parsers.
  /// All field have multiplicity (single, repeated) and presence
  /// (required, optional) specifications and have a parse_field requirement.
  /// Parsing fields requires getting tokens from the stream being parsed.
  class field_parser
  {
    field_multiplicity  multiplicity;
    field_presence      presence;

  protected:
    /// @brief Get the next token from a stream
    /// Tokens are strings of non-white-space characters separated by
    ///  white-space. Additionally comments to end of line are indicated by a
    /// '#' character which may appear in what would otherwise be the middle of
    /// a token. All characters from a '#' to the end of line are consumed.
    /// @param in   input stream to read token from
    /// @returns single token string or empty string on EOF
    /// @throws std::runtime_error on any other non-good stream state when 
    ///         reading from in.
    static std::string get_token( std::istream & in );

  public:
    /// @brief Construct from field multiplicity and presence values
    /// @param m  Multiplicity of field: field_multiplicity::single 
    ///           or field_multiplicity::repeated
    /// @param p  Presence of field: field_presence::required 
    ///           or field_presence::optional
    field_parser(field_multiplicity m, field_presence p)
    : multiplicity(m)
    , presence(p)
    {}

    /// @brief Field parsing operation
    /// @param in Stream to read tokens from to parse field.
    virtual field_value parse_field(std::istream & in) = 0;

    /// @brief Get multiplicity value field_parser constructed with
    /// @returns Instance's field multiplicity value
    field_multiplicity get_multiplicity() { return this->multiplicity; }

    /// @brief Get presence value field_parser constructed with
    /// @returns Instance's field presence value
    field_presence     get_presence()     { return this->presence; }
  };

  /// @brief field_parser sub-class for composite fields
  /// As well as providing a parse_field operation override for
  /// composite_fields also holds concrete field_parsers for each field
  /// in a composite_field, associated with each field's field name (string).
  class composite_field_parser : public field_parser
  {
    typedef std::unique_ptr<field_parser>           field_parser_ptr;
    typedef std::map<std::string,field_parser_ptr>  field_dictionary;
    
    field_dictionary  fields;
    
    field_parser * get_field(std::string const & name);

  public:
    /// @brief Construct from field multiplicity and presence values
    /// Passes parameter values to base class.
    /// @param m  Multiplicity of field: field_multiplicity::single 
    ///           or field_multiplicity::repeated
    /// @param p  Presence of field: field_presence::required 
    ///           or field_presence::optional
    composite_field_parser(field_multiplicity m, field_presence p)
    : field_parser(m,p)
    {}

    /// @brief add a new field (parser) to the composite_field_parser
    /// @param name Name of the new field
    /// @param type Type of the new field: text, integer, boolean, composite
    /// @param m  Multiplicity of the new field: field_multiplicity::single 
    ///           or field_multiplicity::repeated
    /// @param p  Presence of the new field: field_presence::required 
    ///           or field_presence::optional
    /// @throws std::runtime_error if field with value of name already added
    void add_field
    ( std::string const & name
    , field_type type
    , field_multiplicity m
    , field_presence p
    );
    
    /// @brief Returns parser for nested composite field
    /// As composite_field_parsers are created empty users need access to
    /// the composite_field_parsers of composite fields in order to add fields
    /// to them
    /// @param name Name of field that is a composite field to return
    ///             reference to the composite_field_parser for.
    /// @returns reference to field's composite_field_parser.
    /// @throws std::runtime_error if there is no field with the specified
    ///         name or it is not a composite field.
    composite_field_parser & get_composite_field(std::string const & name);

    /// @brief Parses a composite field.
    /// A composite field has the grammar:
    /// {
    ///   field-name field-value
    /// ...
    /// }
    /// After reading each field name looks up the associated field parser and
    /// hands off value parsing to it. Stores the value in a composite_field
    /// under the field's name.
    /// @param in   stream to parse composite field from
    /// @returns composite type of field_value.
    /// @throws std::runtime_error if any field name is not part of the
    ///         composite field, a field having single multiplicity occurs more
    ///         than once, a field whose presence is required has no occurrence
    ///         or opening '{' or closing '}' are missing (note: missing too
    ///         many tokens might combine fields).
    field_value parse_field(std::istream & in); //override;
  };
}
#endif // LED_STRING_DISPLAY_CONFIG_FILE_H
