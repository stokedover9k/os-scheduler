#include "main.h"
#include "random_generator.h"
#include "timer.h"
#include "process.h"
#include "events_queue.h"
#include "event_simulation.h"
#include "loglib.h"
#include "scheduler.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <exception>
#include <stdexcept>
#include <map>

extern random_generator *rgen;
random_generator *rgen = NULL;

extern controlled_clock & timer_control;
controlled_clock & timer_control( * new controlled_clock(0) );

extern general_clock & timer;
general_clock & timer( timer_control );

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
    argmap["-s"] = "F";
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

  // create scheduler
  sch::scheduler * sched;
  {
    switch( argmap["-s"][0] )
    {
      case 'F':  sched = sch::create_policy_scheduler(sch::fcfs_policy());  break;
      case 'L':
      case 'S':
      case 'R':
      default: throw runtime_error("Error: argument indicates unknown scheduler type");
    }
  }

  // read processes
  des::events_queue events;
  std::map<prc::process_core *, int> cpu_bursts, io_bursts;
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
      cpu_bursts[p] = cpu_burst;
      io_bursts[p] = io_burst;
      events.push(arrival_time, p, prc::ARRIVE);
    }
  }

  // create discrete simulator
  des::event_simulation simulator(events, sched);
  while( simulator.run() )
  {
    TLOG;  // inserts a line into log
  }

  // statistics and summary
  {
    switch( argmap["-s"][0] )
    {
      case 'F':  cout << "FCFS" << endl;  break;
      case 'L':  cout << "LCFS" << endl;  break;
      case 'S':  cout << "SJF"  << endl;  break;
      case 'R':  cout << "RR "  << endl;  break;
    }

    int average_wait = 0;
    int last_finish = 0;
    int total_cpu = 0;
    int average_turnaround = 0;

    for (auto i = simulator.get_processes().begin(); 
      i != simulator.get_processes().end(); ++i)
    {
      auto proc = *i;

      int finish = simulator.get_finish_times().find(proc)->second;
      last_finish = std::max(last_finish, finish);
      average_wait += proc->get_ready_time();
      total_cpu += proc->get_cpu_used();
      int turnaround = finish - simulator.get_arrival_times().find(proc)->second;
      average_turnaround += turnaround;

      cout << std::setw(4) << std::setfill('0') << proc->get_pid() << ": "
           << simulator.get_arrival_times().find(proc)->second << ' '
           << proc->get_cpu_used() << ' '
           << cpu_bursts[proc] << ' '
           << io_bursts[proc]
           << " | "
           << finish << ' '
           << turnaround << ' '
           << proc->get_blocked_time() << ' '
           << proc->get_ready_time()
           << endl;
    }

    int num = simulator.get_processes().size();
    cout << "SUM:   "
         << std::fixed
         << last_finish << ' '
         << std::setprecision(2) << 100.0 * simulator.get_total_cpu_time() / last_finish << ' '
         << std::setprecision(2) << 100.0 * simulator.get_total_io_time() / last_finish << ' '
         << std::setprecision(2) << (float) average_turnaround / num << ' '
         << std::setprecision(2) << (float) average_wait / num << ' '
         << std::setprecision(3) << 100.0 * num / last_finish
         << endl;
  }
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
