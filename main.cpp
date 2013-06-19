#include "main.h"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <map>

// Returns a map with following (optional) fields
// "-v"          -> "true"
// "-s"          -> [scheduler_code]
// "input_file"  -> [input file name]
// "random_file" -> [random file name]
std::map<std::string, std::string> parse_args( int argc, char *argv[] );

//--------------------------------------------------------

int main(int argc, char *argv[])
{
  using namespace std;

  map<string, string> argmap( parse_args(argc, argv) );
  if( !argmap.count("-s") )
    argmap["-s"] = "FCFS";
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
