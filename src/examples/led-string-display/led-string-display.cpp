// Project: Raspberry Pi BCM2708 / BCM2835 peripherals C++ library
/// @file examples/led-string-display/led-string-display.cpp
/// @brief GPIO output to 8 LEDs - assumed to be in a 'string' like Xmas lights
//
/// @copyright Copyright (c) Dibase Limited 2013-2015
/// @author Ralph E. McArdell

#include "config-file.h"
#include "pin.h"

#include <exception>    // for std::exception
#include <iostream>     // for std IO stream objects
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>
#include <string>

using namespace dibase::rpi::peripherals;
bool g_running{ true };  ///< Global flag used to communicate quit request

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

led_sequences get_sequences_from_config_stream(std::istream & in)
{
  using namespace config_file;
  composite_field_parser config_parser(field_multiplicity::single, field_presence::required);
  config_parser.add_field( "sequence", field_type::composite, field_multiplicity::repeated, field_presence::required);
  
  composite_field_parser & seq_parser(config_parser.get_composite_field("sequence"));
  seq_parser.add_field("rate_ms", field_type::integer, field_multiplicity::single, field_presence::required);
  seq_parser.add_field("initial_state", field_type::composite, field_multiplicity::single, field_presence::required);
  seq_parser.add_field( "delta", field_type::composite, field_multiplicity::repeated, field_presence::optional);

  composite_field_parser & initial_leds_parser(seq_parser.get_composite_field("initial_state"));
  initial_leds_parser.add_field( "0", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "1", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "2", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "3", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "4", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "5", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "6", field_type::boolean, field_multiplicity::single, field_presence::required);
  initial_leds_parser.add_field( "7", field_type::boolean, field_multiplicity::single, field_presence::required);

  composite_field_parser & delta_leds_parser(seq_parser.get_composite_field("delta"));
  delta_leds_parser.add_field( "0", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "1", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "2", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "3", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "4", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "5", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "6", field_type::boolean, field_multiplicity::single, field_presence::optional);
  delta_leds_parser.add_field( "7", field_type::boolean, field_multiplicity::single, field_presence::optional);

  composite_field seqs_value(config_parser.parse_field(in).composite());

  led_sequences seqs;
  for ( auto const & upsv : seqs_value["sequence"] )
    {
      led_sequence seq;
      seq.delay = std::chrono::milliseconds(upsv->composite()["rate_ms"][0]->integer());
      led_string_delta d;
      d.emplace_back(0, upsv->composite()["initial_state"][0]->composite()["0"][0]->boolean());
      d.emplace_back(1, upsv->composite()["initial_state"][0]->composite()["1"][0]->boolean());
      d.emplace_back(2, upsv->composite()["initial_state"][0]->composite()["2"][0]->boolean());
      d.emplace_back(3, upsv->composite()["initial_state"][0]->composite()["3"][0]->boolean());
      d.emplace_back(4, upsv->composite()["initial_state"][0]->composite()["4"][0]->boolean());
      d.emplace_back(5, upsv->composite()["initial_state"][0]->composite()["5"][0]->boolean());
      d.emplace_back(6, upsv->composite()["initial_state"][0]->composite()["6"][0]->boolean());
      d.emplace_back(7, upsv->composite()["initial_state"][0]->composite()["7"][0]->boolean());
      seq.deltas.push_back(d);
      if ( upsv->composite().has_field("delta"))
        {
          for ( auto const & updv : upsv->composite()["delta"] )
            {
              d.clear();
              if (updv->composite().has_field("0"))
                { d.emplace_back(0, updv->composite()["0"][0]->boolean());}
              if (updv->composite().has_field("1"))
                { d.emplace_back(1, updv->composite()["1"][0]->boolean());}
              if (updv->composite().has_field("2"))
                { d.emplace_back(2, updv->composite()["2"][0]->boolean());}
              if (updv->composite().has_field("3"))
                { d.emplace_back(3, updv->composite()["3"][0]->boolean());}
              if (updv->composite().has_field("4"))
                { d.emplace_back(4, updv->composite()["4"][0]->boolean());}
              if (updv->composite().has_field("5"))
                { d.emplace_back(5, updv->composite()["5"][0]->boolean());}
              if (updv->composite().has_field("6"))
                { d.emplace_back(6, updv->composite()["6"][0]->boolean());}
              if (updv->composite().has_field("7"))
                { d.emplace_back(7, updv->composite()["7"][0]->boolean());}
              seq.deltas.push_back(d);
            }
        }
      seqs.push_back(seq);
    }

  return seqs;
}

// Apply a single set of LED state changes
void apply(led_string_delta const & delta, opin ** leds)
{
  for (auto & desc : delta)
    {
      leds[desc.first]->put(desc.second);
    }
}

/// @brief  Core program logic
///
/// - create the 8 GPIO opin, out pins that drive the LEDs
/// - set initial state
/// - while running:
///    - apply sequences a number of times each
///      - switch sequence after each one completes a set number of times
///        - wrap from last to first sequence
void do_light_show( led_sequences const & seqs )
{
  try
    {
      opin led0{gpio_gen6}; // Gertboard J2 GP25
      opin led1{gpio_gen5}; // Gertboard J2 GP24
      opin led2{gpio_gen4}; // Gertboard J2 GP23
      opin led3{gpio_gen3}; // Gertboard J2 GP22
      opin led4{rxd};       // Gertboard J2 GP15
      opin led5{gpio_gen2}; // Gertboard J2 GP21 (GPIO27 on rev.2 boards)
      opin led6{gpio_gen1}; // Gertboard J2 GP18 
      opin led7{gpio_gen0}; // Gertboard J2 GP17 
      opin * leds[] = {&led0, &led1, &led2, &led3, &led4, &led5, &led6, &led7};

      auto t_wake(std::chrono::system_clock::now());
      constexpr auto t_snooze(std::chrono::milliseconds{50});
      auto t_do_change(std::chrono::system_clock::now());
      constexpr int iterations_per_sequence{10};
      std::size_t seq_idx{0};
      auto seq(seqs[seq_idx]);
      auto pos = seq.deltas.begin();
      int count{0};
      while (g_running) 
        {
          std::this_thread::sleep_until(t_wake);
          if ( t_do_change < std::chrono::system_clock::now() )
            {
              apply(*pos,leds);
              t_do_change += seq.delay;
              ++pos;
              if ( pos==seq.deltas.end())
                {
                  if (++count>=iterations_per_sequence)
                   {
                      count = 0;
                      ++seq_idx;
                      if (seq_idx >= seqs.size())
                        {
                          seq_idx = 0;
                        }
                     seq = seqs[seq_idx];
                   }
                  pos = seq.deltas.begin();
                }
            }
          t_wake += t_snooze;
        }

      for (int i = 0; i != 8; ++i)
        leds[i]->put(false);
    }
  catch ( std::exception & e )
    {
      std::cerr << "A problem occurred. Description: " << e.what() << "\n";
    }
}

#include "config-file-tests.h"

char const * default_config_file = "led-string-display.cfg";

/// @brief Tell user how to quit, spawn worker thread, wait for user quit request
///
/// Runs main work on separate thread while main thread is waiting for console
/// input from user, whereupon quit running request signalled by setting
/// global g_running flag to false, and the worker thread is joined to wait
/// for it to exit before returning.
int main(int argc, char* argv[])
{
  std::string config_pathname(default_config_file);
  if (argc>1)
    {
      std::string arg1(argv[1]);
      if (arg1=="test")
        {
          do_config_tests();
          return 0;
        }
       else if ( arg1=="/?" || arg1=="/h" || arg1=="/H"
              || arg1=="-h" || arg1=="-H"
              || arg1=="--help" || arg1=="--HELP"
               )
        {
          std::cout << argv[0] 
                    << " {<pathname>} | {test} | {/? | /h | -h | --help}\n"
                       "   <pathname> Pathname of sequences configuration file. If no path is\n"
                       "              given then the current directory is searched. If no\n"
                       "              pathname is specified then a file named '"
                    << default_config_file << "'\n"
                       "              is looked for in the current directory.\n"
                       "   test       Run built-in tests.\n"
                       "   /? /h -h   Print this help.\n"
                       "   --help"
                    << std::endl;
          return 0;
        }
      else
        {
          config_pathname = arg1;
        }
    }
  std::ifstream cfg_in(config_pathname.c_str());
  if (!cfg_in.is_open())
    {
      std::cerr << "ERROR: Unable to open configuration file '"
                << config_pathname << "'.\n";
      return 1;
    }
  led_sequences seqs(get_sequences_from_config_stream(cfg_in));
  std::cout << "Press enter to quit....\n";
  std::thread counter{ do_light_show, std::ref(seqs) };
  std::string dummy;
  std::getline(std::cin, dummy);
  g_running = false;
  counter.join();
}
