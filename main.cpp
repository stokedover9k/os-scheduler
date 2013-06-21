#include "main.h"
#include "random_generator.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <exception>
#include <stdexcept>
#include <map>

// Returns a map with following (optional) fields
// "-v"          -> "true"
// "-s"          -> [scheduler_code]
// "input_file"  -> [input file name]
// "random_file" -> [rando<std::istream_iterator<int, char, std::char_traits<char>, long> >(std::istream_iterator<int, char, std::char_traits<char>, long>, std::istream_iterator<int, m file name]
std::map<std::string, std::string> parse_args( int argc, char *argv[] );

//--------------------------------------------------------

int main(int argc, char *argv[])
{
  using namespace std;

  // parse arguments and set defaults where missing
  map<string, string> argmap( parse_args(argc, argv) );
  if( !argmap.count("-s") )
    argmap["-s"] = "FCFS";
  if( !argmap.count("random_file") )
    argmap["random_file"] = "data1/rfile";

  ifstream inrandom(argmap["random_file"]);
  int rnumbers;  inrandom >> rnumbers;
  looping_random_generator *myrand = new looping_random_generator(istream_iterator<int>(inrandom), istream_iterator<int>());
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
