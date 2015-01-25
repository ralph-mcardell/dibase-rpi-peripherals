// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library examples
/// @file examples/led-string-display/testsconfig-file-unittests.cpp
/// @brief led-string-display config-file unit tests.
///
/// @copyright Copyright (c) Dibase Limited 2012-2015
/// @author Ralph E. McArdell

#include "catch.hpp"
#include "config-file.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace config_file;

std::string make_single_field_config( std::string const & name, std::string const & value)
{
  return std::string("{\n") + name + " " + value + "\n}";
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-text-field"
         , "Parsing single text field configuration should produce expected field value"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->text()==test_value);
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-integer-field"
         , "Parsing single integer field configuration should produce expected field value"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  std::string test_value("1234");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->integer()==std::stol(test_value));
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-boolean-field"
         , "Parsing single boolean field configuration should produce expected field value"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("boolean");
  parser.add_field( test_field, field_type::boolean, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,"true"));
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"TRUE"));
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"on"));
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"ON"));
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"yes"));
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"YES"));
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"false"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"FALSE"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"off"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"OFF"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"no"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  istrm.str(make_single_field_config(test_field,"NO"));
  istrm.clear();
  CHECK_FALSE(parser.parse_field(istrm).composite()[test_field][0]->boolean());
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-nested-composite-field"
         , "Parsing configuration with nested composite field should produce expected field value"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("composite");
  parser.add_field( test_field, field_type::composite, field_multiplicity::single, field_presence::required);
  composite_field_parser & inner_parser(parser.get_composite_field(test_field));
  std::string test_inner_field("integer");
  std::string test_inner_value("968472");
  inner_parser.add_field( test_inner_field, field_type::integer, field_multiplicity::single, field_presence::required);
  std::string test_config_text("{\n   ");
  test_config_text += test_field + "\t";
  test_config_text += make_single_field_config(test_inner_field,test_inner_value);
  test_config_text += "\n}";
  std::istringstream istrm(test_config_text);
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->composite()[test_inner_field][0]->integer()==std::stol(test_inner_value));
}

TEST_CASE( "Unit-tests/examples/led-string-display/composite_field::has_field"
         , "composite_field::has_field reports accurately on field existence"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  std::string test_value("1234");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  auto the_composite_field(parser.parse_field(istrm).composite());
  CHECK(the_composite_field.has_field(test_field));
  CHECK_FALSE(the_composite_field.has_field(test_field+"_NOT"));
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-repeated-field"
         , "Parsing repeated text fields configuration should produce expected field values"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_values[] = {"hello", "world", "!"};
  std::size_t const num_test_values(sizeof(test_values)/sizeof(std::string));
  parser.add_field( test_field, field_type::text, field_multiplicity::repeated, field_presence::required);
  std::string config_test_text("{\n");
  for (size_t i=0; i<num_test_values; ++i)
    {
      config_test_text += test_field + " " + test_values[i] + "\n";
    }
  config_test_text += "}";
  std::istringstream istrm(config_test_text);
  auto fields(parser.parse_field(istrm).composite());
  for (size_t i=0; i<num_test_values; ++i)
    {
      CHECK(fields[test_field][i]->text()==test_values[i]);
    }
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-optional-field-missing"
         , "Parsing single optional field configuration should not throw if field value not present"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::optional);
  std::istringstream istrm("{\n}");
  CHECK_FALSE(parser.parse_field(istrm).composite().has_field(test_field));
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-optional-field-present"
         , "Parsing single optional field configuration should produce expected field value if field present"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::optional);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->text()==test_value);
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-missing-required-field"
         , "Parsing single required field configuration should throw if field value not present"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::required);
  std::istringstream istrm("{\n}");
  REQUIRE_THROWS_AS ( parser.parse_field(istrm)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/parse-with-comments"
         , "Parsing single boolean field configuration should produce expected field value in presence of comments"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("boolean");
  parser.add_field( test_field, field_type::boolean, field_multiplicity::single, field_presence::required);
  std::string test_text("# a comment line\n");
  test_text += make_single_field_config(test_field,"true");
  std::istringstream istrm(test_text);
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  test_text = "{# a comment to end of line";
  test_text += make_single_field_config(test_field,"on");
  istrm.str(test_text);
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  test_text = "{\n";
  test_text += test_field + "#off\n on\n}";
  istrm.str(test_text);
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
  test_text = make_single_field_config(test_field,"yes");
  test_text += "\n\n\n# end of test text...";
  istrm.str(test_text);
  istrm.clear();
  CHECK(parser.parse_field(istrm).composite()[test_field][0]->boolean());
}

TEST_CASE( "Unit-tests/examples/led-string-display/bad-composite-field-syntax-blank"
         , "Parsing blank text throws runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("boolean");
  parser.add_field( test_field, field_type::boolean, field_multiplicity::single, field_presence::required);
  std::istringstream istrm("");
  REQUIRE_THROWS_AS ( parser.parse_field(istrm)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/bad-composite-field-syntax-no closing brace"
         , "Parsing composite field with missing closing brace throws runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("boolean");
  parser.add_field( test_field, field_type::boolean, field_multiplicity::single, field_presence::required);
  std::istringstream istrm("{ boolean on ");
  REQUIRE_THROWS_AS ( parser.parse_field(istrm)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/bad-composite-field-syntax-no opening brace"
         , "Parsing composite field with missing opening brace throws runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("boolean");
  parser.add_field( test_field, field_type::boolean, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(" boolean on }");
  REQUIRE_THROWS_AS ( parser.parse_field(istrm)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/bad-composite-field unknown field"
         , "Parsing an unknown field should throw a runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config("not_hello",test_value));
  REQUIRE_THROWS_AS ( parser.parse_field(istrm)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/access non integer as integer"
         , "Accessing a non-integer field field as an integer should throw a logic error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  REQUIRE_THROWS_AS ( parser.parse_field(istrm).composite()[test_field][0]->integer()
                    , std::logic_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/access non boolean as boolean"
         , "Accessing a non-boolean field field as a boolean should throw a logic error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  REQUIRE_THROWS_AS ( parser.parse_field(istrm).composite()[test_field][0]->boolean()
                    , std::logic_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/access non composite as composite"
         , "Accessing a non-composite field field as a composite should throw a logic error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("text");
  std::string test_value("hello");
  parser.add_field( test_field, field_type::text, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  REQUIRE_THROWS_AS ( parser.parse_field(istrm).composite()[test_field][0]->composite()
                    , std::logic_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/access non text as text"
         , "Accessing a non-text field field as text should throw a logic error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  std::string test_value("1234");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::required);
  std::istringstream istrm(make_single_field_config(test_field,test_value));
  REQUIRE_THROWS_AS ( parser.parse_field(istrm).composite()[test_field][0]->text()
                    , std::logic_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/parser-duplicate-field"
         , "Adding a duplicate field to a composite_field_parser should throw a runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  parser.add_field("F", field_type::text, field_multiplicity::single, field_presence::required);
  REQUIRE_THROWS_AS ( parser.add_field("F", field_type::text, field_multiplicity::single, field_presence::required)
                    , std::runtime_error
                    );
}

TEST_CASE( "Unit-tests/examples/led-string-display/parser-get-composite-non-existent-field"
         , "Getting composite field parser that is a non-existent composite field parser field should throw a runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  parser.add_field( "F", field_type::text, field_multiplicity::single, field_presence::required);
  REQUIRE_THROWS_AS (parser.get_composite_field("G"), std::runtime_error);
}

TEST_CASE( "Unit-tests/examples/led-string-display/parser-get-composite-non-composite-field"
         , "Getting composite field parser that is a non-existent composite field parser field should throw a runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  parser.add_field( "F", field_type::text, field_multiplicity::single, field_presence::required);
  REQUIRE_THROWS_AS (parser.get_composite_field("F"), std::runtime_error);
}

TEST_CASE( "Unit-tests/examples/led-string-display/composite_field.at no such field"
         , "composite_field::at throws runtime error if non-existent field requested"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::optional);
  std::istringstream istrm("{\n}");
  auto the_composite_field(parser.parse_field(istrm).composite());
  CHECK_FALSE(the_composite_field.has_field("whatever"));
  REQUIRE_THROWS_AS (the_composite_field.at("whatever"), std::runtime_error);
}

TEST_CASE( "Unit-tests/examples/led-string-display/composite_field [] no such field"
         , "composite_field::operator[] throws runtime error if non-existent field requested"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::optional);
  std::istringstream istrm("{\n}");
  auto the_composite_field(parser.parse_field(istrm).composite());
  CHECK_FALSE(the_composite_field.has_field("whatever"));
  REQUIRE_THROWS_AS (the_composite_field["whatever"], std::runtime_error);
}

TEST_CASE( "Unit-tests/examples/led-string-display/parser: parse from bad stream"
         , "passing a bad stream to a field parser throws a runtime error"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  std::string test_field("integer");
  parser.add_field( test_field, field_type::integer, field_multiplicity::single, field_presence::optional);
  std::istringstream istrm("{\n}");
  istrm.setstate(std::ios::failbit);
  REQUIRE_THROWS_AS(parser.parse_field(istrm).composite(), std::runtime_error);
}


TEST_CASE( "Unit-tests/examples/led-string-display/zzz_parse-complex-configuration"
         , "Parsing a complex configuration type should work"
         )
{
  composite_field_parser parser(field_multiplicity::single, field_presence::required);
  parser.add_field( "triangle", field_type::composite, field_multiplicity::repeated, field_presence::optional);
  parser.add_field( "rectangle", field_type::composite, field_multiplicity::repeated, field_presence::optional);
  parser.add_field( "circle", field_type::composite, field_multiplicity::repeated, field_presence::optional);

  composite_field_parser & triangle_parser(parser.get_composite_field("triangle"));
  triangle_parser.add_field("point0", field_type::composite, field_multiplicity::single, field_presence::required);
  triangle_parser.add_field("point1", field_type::composite, field_multiplicity::single, field_presence::required);
  triangle_parser.add_field("point2", field_type::composite, field_multiplicity::single, field_presence::required);
  triangle_parser.add_field("name", field_type::text, field_multiplicity::single, field_presence::optional);
  composite_field_parser & triangle_pt0_parser(triangle_parser.get_composite_field("point0"));
  triangle_pt0_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  triangle_pt0_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);
  composite_field_parser & triangle_pt1_parser(triangle_parser.get_composite_field("point1"));
  triangle_pt1_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  triangle_pt1_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);
  composite_field_parser & triangle_pt2_parser(triangle_parser.get_composite_field("point2"));
  triangle_pt2_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  triangle_pt2_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);

  composite_field_parser & rect_parser(parser.get_composite_field("rectangle"));
  rect_parser.add_field("top-left", field_type::composite, field_multiplicity::single, field_presence::required);
  rect_parser.add_field("bottom-right", field_type::composite, field_multiplicity::single, field_presence::required);
  rect_parser.add_field("name", field_type::text, field_multiplicity::single, field_presence::optional);
  composite_field_parser & rect_tl_parser(rect_parser.get_composite_field("top-left"));
  rect_tl_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  rect_tl_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);
  composite_field_parser & rect_br_parser(rect_parser.get_composite_field("bottom-right"));
  rect_br_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  rect_br_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);

  composite_field_parser & circle_parser(parser.get_composite_field("circle"));
  circle_parser.add_field("centre", field_type::composite, field_multiplicity::single, field_presence::required);
  circle_parser.add_field("radius", field_type::integer, field_multiplicity::single, field_presence::required);
  circle_parser.add_field("name", field_type::text, field_multiplicity::single, field_presence::optional);
  composite_field_parser & circle_centre_parser(circle_parser.get_composite_field("centre"));
  circle_centre_parser.add_field("x", field_type::integer, field_multiplicity::single, field_presence::required);
  circle_centre_parser.add_field("y", field_type::integer, field_multiplicity::single, field_presence::required);
  std::string 
  shapes
  ("{\n"
   "  triangle { point0 { x 10  y 20 } point1 { x 30  y 50 } point2 { x 40  y 0 } }\n"
   "  rectangle { top-left { x 10  y 10 } bottom-right { x 50  y 100 } }\n"
   "  circle { radius 35 centre { x 75  y 125 } }\n"
   "  triangle { name tri2 point2 { x 100  y 100 } point0 { x 30  y 50 } point1 { x 150  y 50 } }\n"
   "  rectangle { name rect2 top-left { x 100  y 10 } bottom-right { x 500   y 100 } }\n"
   "  circle { name circle2 centre { x 7   y 15 } radius 12 }\n"
   "}"
  );
  std::istringstream istrm(shapes);
  auto config_fields(parser.parse_field(istrm).composite());
  CHECK(config_fields.has_field("triangle"));
  CHECK(config_fields.has_field("rectangle"));
  CHECK(config_fields.has_field("circle"));
  if (config_fields.has_field("triangle"))
    {
      CHECK(config_fields["triangle"].size()==2);
      REQUIRE(config_fields["triangle"][0]->composite().has_field("point0"));
      REQUIRE(config_fields["triangle"][0]->composite()["point0"][0]->composite().has_field("x"));
      CHECK(config_fields["triangle"][0]->composite()["point0"][0]->composite()["x"][0]->integer()==10);
      REQUIRE(config_fields["triangle"][0]->composite()["point0"][0]->composite().has_field("y"));
      CHECK(config_fields["triangle"][0]->composite()["point0"][0]->composite()["y"][0]->integer()==20);
      REQUIRE(config_fields["triangle"][0]->composite().has_field("point1"));
      REQUIRE(config_fields["triangle"][0]->composite()["point1"][0]->composite().has_field("x"));
      CHECK(config_fields["triangle"][0]->composite()["point1"][0]->composite()["x"][0]->integer()==30);
      REQUIRE(config_fields["triangle"][0]->composite()["point1"][0]->composite().has_field("y"));
      CHECK(config_fields["triangle"][0]->composite()["point1"][0]->composite()["y"][0]->integer()==50);
      REQUIRE(config_fields["triangle"][0]->composite().has_field("point2"));
      REQUIRE(config_fields["triangle"][0]->composite()["point2"][0]->composite().has_field("x"));
      CHECK(config_fields["triangle"][0]->composite()["point2"][0]->composite()["x"][0]->integer()==40);
      REQUIRE(config_fields["triangle"][0]->composite()["point2"][0]->composite().has_field("y"));
      CHECK(config_fields["triangle"][0]->composite()["point2"][0]->composite()["y"][0]->integer()==0);
      REQUIRE_FALSE(config_fields["triangle"][0]->composite().has_field("name"));
      if (config_fields["triangle"].size()>=2)
        {
          REQUIRE(config_fields["triangle"][1]->composite().has_field("point0"));
          REQUIRE(config_fields["triangle"][1]->composite()["point0"][0]->composite().has_field("x"));
          CHECK(config_fields["triangle"][1]->composite()["point0"][0]->composite()["x"][0]->integer()==30);
          REQUIRE(config_fields["triangle"][1]->composite()["point0"][0]->composite().has_field("y"));
          CHECK(config_fields["triangle"][1]->composite()["point0"][0]->composite()["y"][0]->integer()==50);
          REQUIRE(config_fields["triangle"][1]->composite().has_field("point1"));
          REQUIRE(config_fields["triangle"][1]->composite()["point1"][0]->composite().has_field("x"));
          CHECK(config_fields["triangle"][1]->composite()["point1"][0]->composite()["x"][0]->integer()==150);
          REQUIRE(config_fields["triangle"][1]->composite()["point1"][0]->composite().has_field("y"));
          CHECK(config_fields["triangle"][1]->composite()["point1"][0]->composite()["y"][0]->integer()==50);
          REQUIRE(config_fields["triangle"][1]->composite().has_field("point2"));
          REQUIRE(config_fields["triangle"][1]->composite()["point2"][0]->composite().has_field("x"));
          CHECK(config_fields["triangle"][1]->composite()["point2"][0]->composite()["x"][0]->integer()==100);
          REQUIRE(config_fields["triangle"][1]->composite()["point2"][0]->composite().has_field("y"));
          CHECK(config_fields["triangle"][1]->composite()["point2"][0]->composite()["y"][0]->integer()==100);
          REQUIRE(config_fields["triangle"][1]->composite().has_field("name"));
          CHECK(config_fields["triangle"][1]->composite()["name"][0]->text()=="tri2");
        }
    }
  if (config_fields.has_field("rectangle"))
    {
      CHECK(config_fields["rectangle"].size()==2);
      REQUIRE(config_fields["rectangle"][0]->composite().has_field("top-left"));
      REQUIRE(config_fields["rectangle"][0]->composite()["top-left"][0]->composite().has_field("x"));
      CHECK(config_fields["rectangle"][0]->composite()["top-left"][0]->composite()["x"][0]->integer()==10);
      REQUIRE(config_fields["rectangle"][0]->composite()["top-left"][0]->composite().has_field("y"));
      CHECK(config_fields["rectangle"][0]->composite()["top-left"][0]->composite()["y"][0]->integer()==10);
      REQUIRE(config_fields["rectangle"][0]->composite().has_field("bottom-right"));
      REQUIRE(config_fields["rectangle"][0]->composite()["bottom-right"][0]->composite().has_field("x"));
      CHECK(config_fields["rectangle"][0]->composite()["bottom-right"][0]->composite()["x"][0]->integer()==50);
      REQUIRE(config_fields["rectangle"][0]->composite()["bottom-right"][0]->composite().has_field("y"));
      CHECK(config_fields["rectangle"][0]->composite()["bottom-right"][0]->composite()["y"][0]->integer()==100);
      REQUIRE_FALSE(config_fields["rectangle"][0]->composite().has_field("name"));
      if (config_fields["rectangle"].size()>=2)
        {
          REQUIRE(config_fields["rectangle"][1]->composite().has_field("top-left"));
          REQUIRE(config_fields["rectangle"][1]->composite()["top-left"][0]->composite().has_field("x"));
          CHECK(config_fields["rectangle"][1]->composite()["top-left"][0]->composite()["x"][0]->integer()==100);
          REQUIRE(config_fields["rectangle"][1]->composite()["top-left"][0]->composite().has_field("y"));
          CHECK(config_fields["rectangle"][1]->composite()["top-left"][0]->composite()["y"][0]->integer()==10);
          REQUIRE(config_fields["rectangle"][1]->composite().has_field("bottom-right"));
          REQUIRE(config_fields["rectangle"][1]->composite()["bottom-right"][0]->composite().has_field("x"));
          CHECK(config_fields["rectangle"][1]->composite()["bottom-right"][0]->composite()["x"][0]->integer()==500);
          REQUIRE(config_fields["rectangle"][1]->composite()["bottom-right"][0]->composite().has_field("y"));
          CHECK(config_fields["rectangle"][1]->composite()["bottom-right"][0]->composite()["y"][0]->integer()==100);
          REQUIRE(config_fields["rectangle"][1]->composite().has_field("name"));
          CHECK(config_fields["rectangle"][1]->composite()["name"][0]->text()=="rect2");
        }
    }
  if (config_fields.has_field("circle"))
    {
      CHECK(config_fields["circle"].size()==2);
      REQUIRE(config_fields["circle"][0]->composite().has_field("centre"));
      REQUIRE(config_fields["circle"][0]->composite()["centre"][0]->composite().has_field("x"));
      CHECK(config_fields["circle"][0]->composite()["centre"][0]->composite()["x"][0]->integer()==75);
      REQUIRE(config_fields["circle"][0]->composite()["centre"][0]->composite().has_field("y"));
      CHECK(config_fields["circle"][0]->composite()["centre"][0]->composite()["y"][0]->integer()==125);
      REQUIRE(config_fields["circle"][0]->composite().has_field("radius"));
      CHECK(config_fields["circle"][0]->composite()["radius"][0]->integer()==35);
      REQUIRE_FALSE(config_fields["circle"][0]->composite().has_field("name"));
      if (config_fields["circle"].size()>=2)
        {
          REQUIRE(config_fields["circle"][1]->composite().has_field("centre"));
          REQUIRE(config_fields["circle"][1]->composite()["centre"][0]->composite().has_field("x"));
          CHECK(config_fields["circle"][1]->composite()["centre"][0]->composite()["x"][0]->integer()==7);
          REQUIRE(config_fields["circle"][1]->composite()["centre"][0]->composite().has_field("y"));
          CHECK(config_fields["circle"][1]->composite()["centre"][0]->composite()["y"][0]->integer()==15);
          REQUIRE(config_fields["circle"][1]->composite().has_field("radius"));
          CHECK(config_fields["circle"][1]->composite()["radius"][0]->integer()==12);          
          REQUIRE(config_fields["circle"][1]->composite().has_field("name"));
          CHECK(config_fields["circle"][1]->composite()["name"][0]->text()=="circle2");
        }
    }
}
