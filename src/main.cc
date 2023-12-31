#include <iostream>
#include <ostream>
#include <typeinfo> 
#include "../include/commands.hpp"
#include "../include/parser.hpp"
#include "../include/global.hpp"
#include "../include/dmp_diff.hpp"

// TODO: Write Logs in a Log file Continously.
// TODO: Make a prototype To keep track of a repo and report if a change has occured.


int main(int argc, char const *argv[])
{   
  
  CommandLineParser parser;
  CommandLineParser::Option initOption ("init", "Initialize the Repository.", initCommand);
  CommandLineParser::Option addOption ("add", "Adds changes to the stage aka. index file.", addCommand);

  parser.add_custom_option(initOption);
  parser.add_custom_option(addOption);

  parser.parse(argc, argv);
  return 0;
}

