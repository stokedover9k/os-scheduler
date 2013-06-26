#include "main.h"
#include "random_generator.h"
#include "timer.h"
#include "process.h"
#include "events_queue.h"
#include "loglib.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <exception>
#include <stdexcept>
#include <map>

extern random_generator *rgen;
random_generator *rgen = NULL;

extern general_clock & timer;
general_clock & timer( * new controlled_clock(0) );

/* Returns a map with following (optional) fields
 * "-v"          -> "true"
 * "-s"          -> [scheduler_code]
 * "input_file"  -> [input file name]
 * "random_file" -> [rando<std::istream_iterator<int, char, std::char_traits<char>, long> >(std::istream_iterator<int, char, std::char_traits<char>, long>, std::istream_iterator<int, m file name]
 */
std::map<std::string, std::string> parse_args( int argc, char *argv[] );

//--------------------------------------------------------

int main(int argc, char *argv[])
{
  using namespace std;

  Output2FILE::Stream() = fopen( "trace.log", "w" );

  // parse arguments and set defaults where missing
  map<string, string> argmap( parse_args(argc, argv) );
  if( !argmap.count("-s") )
    argmap["-s"] = "FCFS";
  if( !argmap.count("random_file") )
    argmap["random_file"] = "./rfile";

  // set up random generator
  {
    ifstream inrandom(argmap["random_file"]);
    if( !inrandom )
      throw std::ios_base::failure("could not open random file: " + argmap["random_file"]);
    int rnumbers;
    inrandom >> rnumbers;
    rgen = new looping_random_generator(istream_iterator<int>(inrandom), istream_iterator<int>());
  }

  // read processes
  des::events_queue events;
  {
    string line;
    ifstream infile(argmap["input_file"]);
    while( getline(infile, line) ) {
      istringstream iss( line );
      int arrival_time, total_cpu, cpu_burst, io_burst;
      iss >> arrival_time >> total_cpu >> cpu_burst >> io_burst;
      if( ! iss )
        throw runtime_error("Error: malformed input file");

      prc::stochastic_process *p = new prc::stochastic_process(events.size(), total_cpu, cpu_burst, io_burst);
      events.push(arrival_time, p, prc::ARRIVE);
    }
  }

  // test code

  /*
  typedef prc::process_core process;
  typedef prc::stochastic_process sproc;

  des::events_queue events;
  TLOG << "empty queue: " << events.size();

  process * p1 = new sproc( 1, 100, 10, 5 );
  process * p2 = new sproc( 2, 50, 3, 15 );

  des::event e1( 5, p1, prc::ARRIVE );
  des::event e2( 5, p2, prc::ARRIVE );

  events.push(e1);
  events.push(e2);
  TLOG << "queue size (ins): " << events.size();

  des::event e = events.peek();
  TLOG << "queue size (peek): " << events.size();
  TLOG << "pid: " << e.get_process()->get_pid();

  e = events.pop();
  TLOG << "queue size (pop): " << events.size();
  TLOG << "pid: " << e.get_process()->get_pid();

  e = events.pop();
  TLOG << "queue size (pop): " << events.size();
  TLOG << "pid: " << e.get_process()->get_pid();
  */
}

//--------------------------------------------------------

std::map<std::string, std::string> parse_args( int argc, char *argv[] )
{
  std::map<std::string, std::string> argmap;

  for(int i = 1; i < argc; ++i)
  {
    std::string arg(argv[i]);

    if( arg[0] == '-' ) {
      if( arg == "-v" )
        argmap["-v"] = "true";
      else if( arg.length() > 2 && arg[1] == 's' ) {
        argmap["-s"] = arg.substr(2);
      }
    }
    else {
      if( !argmap.count("input_file") )
        argmap["input_file"] = arg;
      else if( !argmap.count("random_file") )
        argmap["random_file"] = arg;
    }
  }

  if( !argmap.count("input_file") || !argmap.count("random_file") )
    throw std::invalid_argument("at least input and random files must be provided as arguments");

  return argmap;
}
