// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-string-display/config-file-tests.cpp
/// @brief Quick & dirty tests for led-string-display.cpp config. file support.
//
/// @copyright Copyright (c) Dibase Limited 2015
/// @author Ralph E. McArdell

#include "config-file-tests.h"
#include "config-file.h"

#include <iostream>
#include <sstream>

// #### HACK! Types & declarations copied from led-string-display.cpp 
// -- these tests are only temporary!

#include <chrono>
#include <vector>
#include <utility>

/// @brief Description of LED state: LED index [0,7], on|off 
typedef std::pair<int,bool>           led_desc;

/// @brief  Set of LED state changes: collection of LED state descriptions to apply
typedef std::vector<led_desc>         led_string_delta;

/// @brief  LED lighting effect sequence: 
/// Time between state changes, set of state changes
struct led_sequence
{
  std::chrono::milliseconds       delay;
  std::vector<led_string_delta>   deltas;  
};

/// @brief  Collection of LED lighting effect sequences
typedef std::vector<led_sequence> led_sequences;

led_sequences get_sequences_from_config_stream(std::istream & in);

// #### HACK ends

led_sequences get_sequences_from_config_string( std::string const & config_string )
{
  std::istringstream istrm(config_string);
  return get_sequences_from_config_stream( istrm );
}

std::string seqs_diff(led_sequences const & wanted, led_sequences const & got)
{
  std::ostringstream diff_report;
  int n_seqs = std::min(wanted.size(), got.size());
  if (wanted.size() != got.size())
    {
      diff_report << "Sequences size differs. Wanted: " << wanted.size()
                  << ", got: " << got.size() << ".\n";
    }
  for ( int i=0; i!=n_seqs; ++i )
    {
      led_sequence wanted_seq(wanted[i]);
      led_sequence got_seq(got[i]);
      if (got_seq.delay!= wanted_seq.delay)
        {
          diff_report << "Sequence " << i << ": delays differ. Wanted: "
                      << wanted_seq.delay.count() 
                      << ", got: " << got_seq.delay.count() << ".\n";
        }
      int n_led_deltas = std::min(wanted_seq.deltas.size(), got_seq.deltas.size());
      if ( got_seq.deltas.size()!=wanted_seq.deltas.size() )
        {
          diff_report << "Sequence " << i << ": Sequence delta sizes differ. Wanted: "
                      << wanted_seq.deltas.size() << ", got: " 
                      << got_seq.deltas.size() << ".\n";
        }
      for ( int di = 0; di != n_led_deltas; ++di )
        {
          led_string_delta wanted_states(wanted_seq.deltas[di]);
          led_string_delta got_states(got_seq.deltas[di]);
          if (got_states.size()!=wanted_states.size())
            {
              diff_report << "Sequence " << i << " delta " << di 
                          << ": delta state sizes differ. Wanted: "
                          << wanted_states.size() << ", got: " 
                          << got_states.size() << ".\n";
            
            }
          int n_states = std::min(got_states.size(),wanted_states.size());
          for (int si=0; si!=n_states;++si)
            {
              led_desc wanted_state(wanted_states[si]);
              led_desc got_state(got_states[si]);
              
              if (got_state!=wanted_state)
                {
                  diff_report << "Sequence " << i << " delta " << di 
                              << " state " << si
                              << ": LED state delta differs. Wanted: ("
                              << wanted_state.first << ", " << wanted_state.second
                              << "), got: (" << got_state.first 
                              << ", " << got_state.second << ").\n";
                }
            }
          if (got_states.size()<wanted_states.size())
            {
              diff_report << "Sequence " << i << " delta " << di 
                          << " is missing wanted LED state changes:\n";
              for (unsigned int si=n_states; si!=wanted_states.size();++si)
                {
                  led_desc wanted_state(wanted_states[si]);                 
                  diff_report << "   ("  << wanted_state.first 
                              << ", " << wanted_state.second << ").\n";
                }
            }
          else if (got_states.size()>wanted_states.size())
            {
              diff_report << "Sequence " << i << " delta " << di 
                          << " got extra LED state changes:\n";
              for (unsigned int si=n_states; si!=got_states.size();++si)
                {
                  led_desc got_state(got_states[si]);                 
                  diff_report << "   ("  << got_state.first 
                              << ", " << got_state.second << ").\n";
                }
            }
        }
      if ( got_seq.deltas.size()<wanted_seq.deltas.size() )
        {
          diff_report << "Sequence " << i << " missing wanted states:\n";
          for (unsigned int di = n_led_deltas; di != wanted_seq.deltas.size(); ++di)
            {
              led_string_delta wanted_states(wanted_seq.deltas[di]);
              for (unsigned int si=0; si!=wanted_states.size();++si)
                {
                  led_desc wanted_state(wanted_states[si]);                 
                  diff_report << "   ("  << wanted_state.first 
                              << ", " << wanted_state.second << "),  ";
                }
              diff_report << "\n";
            }
        }
      else if ( got_seq.deltas.size()>wanted_seq.deltas.size() )
        {
          diff_report << "Sequence " << i << " got extra states:\n";
          for (unsigned int di = n_led_deltas; di != got_seq.deltas.size(); ++di)
            {
              led_string_delta got_states(got_seq.deltas[di]);
              for (unsigned int si=0; si!=got_states.size();++si)
                {
                  led_desc got_state(got_states[si]);                 
                  diff_report << "   ("  << got_state.first 
                              << ", " << got_state.second << "),  ";
                }
              diff_report << "\n";
             }
        }
    }
  if (wanted.size() > got.size())
    {
      diff_report << "Missing wanted sequences:\n";
      for ( unsigned int i=n_seqs; i!=wanted.size(); ++i )
        {
          led_sequence wanted_seq(wanted[i]);
          for (unsigned int di = 0; di != wanted_seq.deltas.size(); ++di)
            {
              led_string_delta wanted_states(wanted_seq.deltas[di]);
              for (unsigned int si=0; si!=wanted_states.size();++si)
                {
                  led_desc wanted_state(wanted_states[si]);                 
                  diff_report << "   ("  << wanted_state.first 
                              << ", " << wanted_state.second << "),  ";
                }
              diff_report << "\n";
            }
          diff_report << "\n";
        }
    }
  else if (got.size() > wanted.size())
    {
      diff_report << "Got extra sequences:\n";
      for ( unsigned int i=n_seqs; i!=got.size(); ++i )
        {
          led_sequence got_seq(got[i]);
          for (unsigned int di = 0; di != got_seq.deltas.size(); ++di)
            {
              led_string_delta got_states(got_seq.deltas[di]);
              for (unsigned int si=0; si!=got_states.size();++si)
                {
                  led_desc got_state(got_states[si]);                 
                  diff_report << "   ("  << got_state.first 
                              << ", " << got_state.second << "),  ";
                }
              diff_report << "\n";
             }
          diff_report << "\n";
        }
    }
  return diff_report.str();
}

class test_stats
{
  int test_count;
  int pass_count;
  int fail_count;
  int excp_count;
  std::string test_name;
  std::string test_desc;
public:
  test_stats()
  : test_count(0), pass_count(0), fail_count(0), excp_count(0)
  {}
  void set_test(std::string name, std::string desc="")
  {
    test_name = name;
    test_desc = desc;
  }
  void test_good_config(std::string const & config,  led_sequences const & expected);
  void test_bad_config(std::string const & config);
  void print_results();
};

void test_stats::test_good_config(std::string const & config,  led_sequences const & expected)
{
  try
    {
      ++test_count;
      std::cerr << "Doing test " << test_name << "\n   Parsing test sequence...\n";

      auto got(get_sequences_from_config_string(config));
      std::cerr << "   done. Checking against expected sequences...\n";
      auto report(seqs_diff(expected, got));
      if ( !report.empty() )
        {
          std::cerr << "Test '" << test_name 
                    << "' failed with unexpected results:\n"
                    << report << "\n";
          ++fail_count;
        }
      else
        {
          ++pass_count;
        }
      std::cerr << "   done.\n";
    }
  catch (std::exception const & e)
    {
      std::cerr << "Test '" << test_name << "' failed with exception:\n"
                << "   " << e.what() << "\n";
      ++fail_count;
      ++excp_count;
    }
  catch (...)
    {
      std::cerr << "Test '" << test_name << "' failed with unanticipated exception:\n";
      ++fail_count;
      ++excp_count;
    }
}

void test_stats::test_bad_config(std::string const & config)
{
  try
    {
      ++test_count;
      auto got(get_sequences_from_config_string(config));
      std::cerr << "Test '" << test_name 
                << "' did not raise an exception as expected\n";
      ++fail_count;
    }
  catch (std::exception const & e)
    {
      ++pass_count;
    }
  catch (...)
    {
      std::cerr << "Test '" << test_name << "' failed with unanticipated exception:\n";
      ++fail_count;
      ++excp_count;
    }
}
void test_stats::print_results()
{
  std::cout << "Ran " << test_count << " tests. Passed " << pass_count
            << " failed " << fail_count 
            << " (" << excp_count << " unexpected exceptions).\n";
}

void do_config_tests()
{
  test_stats tester;
  tester.set_test("0001 : Simple valid single sequence");
  led_sequences t1 = { { std::chrono::milliseconds{200}
                       , { { {0,true},{1,false},{2,true},{3,false}
                           , {4,true},{5,false},{6,true},{7,false} }
                         }
                       }
                     };
  tester.test_good_config
  ( "{ sequence { rate_ms 200 initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 TRUE 7 FALSE } } }"
  , t1
  );

  tester.set_test("0002 : Valid single sequence, 1 delta");
  led_sequences t2 = { { std::chrono::milliseconds{200}
                       , { { {0,true},{1,false},{2,true},{3,false}
                           , {4,true},{5,false},{6,true},{7,false} }
                         , { {0, false}, {1,true} }
                         }
                       }
                     };
  tester.test_good_config
  ( "{ sequence\n"
    "  { rate_ms 200\n"
    "    initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }\n"
    "    delta { 0 OFF 1 ON }\n"
    "  }\n" 
    "}"
  , t2
  );

  tester.set_test("0003 : Valid single sequence, 1 delta, all change");
  led_sequences t3 = { { std::chrono::milliseconds{321}
                       , { { {0,true},{1,false},{2,true},{3,false}
                           , {4,true},{5,false},{6,true},{7,false} }
                         , { {0,false},{1,true},{2,false},{3,true}
                           , {4,false},{5,true},{6,false},{7,true} }
                         }
                       }
                     };
  tester.test_good_config
  ( "{ sequence\n"
    "  { rate_ms 321\n"
    "    initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }\n"
    "    delta { 0 OFF 1 ON 2 FALSE 3 TRUE 4 off 5 on 6 off 7 on }\n"
    "  }\n" 
    "}\n"
  , t3
  );

  tester.set_test("0004 : Valid single sequence, 2 deltas");
  led_sequences t4 = { { std::chrono::milliseconds{321}
                       , { { {0,true},{1,false},{2,true},{3,false}
                           , {4,true},{5,false},{6,true},{7,false} }
                         , { {0,false},{1,true} }
                         , { {0,true},{1,false},{2,false},{3,true} }
                         }
                       }
                     };
  tester.test_good_config
  ( "{ sequence\n"
    "  { rate_ms 321\n\n"
    "    initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }\n"
    "    delta { 0 OFF 1 ON }\n"
    "    delta { 0 on 1 off 2 off 3 on }\n"
    "  }\n" 
    "}"
  , t4
  );

  tester.set_test("0005 : Valid 2 sequences, 2 deltas");
  led_sequences t5 = { { std::chrono::milliseconds{321}
                       , { { {0,true},{1,false},{2,true},{3,false}
                           , {4,true},{5,false},{6,true},{7,false} }
                         , { {0,false},{1,true} }
                         , { {0,true},{1,false},{2,false},{3,true} }
                         }
                       }
                     , { std::chrono::milliseconds{642}
                       , { { {0,true},{1,true},{2,true},{3,true}
                           , {4,true},{5,true},{6,true},{7,true} }
                         , { {0,false},{1,false} }
                         , { {0,true},{1,true},{2,false},{3,false} }
                         }
                       }
                     };
  tester.test_good_config
  ( "{ sequence\n"
    "  { rate_ms 321\n"
    "    initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }\n"
    "    delta { 0 OFF 1 ON }\n"
    "    delta { 0 on 1 off 2 off 3 on }\n"
    "  }\n\n" 
    " sequence\n"
    "  { rate_ms 642\n"
    "    initial_state { 0 on 1 on 2 on 3 on 4 on 5 on 6 on 7 on }\n"
    "    delta { 0 no 1 off }\n"
    "    delta { 0 on 1 on 2 no 3 no }\n"
    "  }\n" 
    "}\n\n"
  , t5
  );

  tester.set_test("0006 : Valid 2 sequences, 2 deltas, with comments");
  tester.test_good_config
  ( "{ sequence # first sequence\n"
    "  { rate_ms 321\n"
    "    initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }\n"
    "    delta { 0 OFF 1 ON }\n"
    "    delta { 0 on 1 off 2 off 3 on }\n"
    "  }\n\n" 
    "#second sequence\n"
    " sequence\n"
    "  { rate_ms 642# Half rate of 1st sequence\n"
    "    initial_state { 0 on 1 on 2 on 3 on 4 on 5 on 6 on 7 on }\n"
    "    delta { 0 no 1 off }\n"
    "    delta { 0 on 1 on 2 no 3 no }\n"
    "  }\n" 
    "}#end"
  , t5
  );

  tester.set_test("9001 : Bad : Invalid keyword #1");
  tester.test_bad_config( "{ xx y }" );
  tester.set_test("9002 : Bad : Invalid keyword #2");
  tester.test_bad_config( "{ sequence { bad_kwd crud }" );
  tester.set_test("9003 : Bad : Empty configuration");
  tester.test_bad_config( "" );
  tester.set_test("9004 : Bad : only comment");
  tester.test_bad_config( "#comment\n" );
  tester.set_test("9005 : Bad : missing opening brace");
  tester.test_bad_config( "sequence {rate_ms 321 initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }  } }" );
  tester.set_test("9006 : Bad : missing closing brace");
  tester.test_bad_config( "sequence {rate_ms 321 initial_state { 0 true 1 false 2 on 3 off 4 yes 5 no 6 YES 7 NO }  } " );
  tester.set_test("9007 : Bad : missing value");
  tester.test_bad_config( "{ sequence   } " );
 
  tester.print_results();
}
