#include <iostream>
#include <ostream>
#include <typeinfo> 
#include "../include/commands.hpp"
#include "../include/parser.hpp"
#include "../include/global.hpp"

// TODO: Write Logs in a Log file Continously.
// TODO: Make a prototype To keep track of a repo and report if a change has occured.


int main(int argc, char const *argv[])
{    
  CommandLineParser parser;

  CommandLineParser::Option initOption ("init", "Initialize the Repository.", initCommand);
  CommandLineParser::Option addOption ("add", "Adds changes to the stage aka. index file.", addCommand);
  CommandLineParser::Option commitOption ("commit", "Commit the changes inside the index file.", commitCommand);

  parser.add_custom_option(initOption);
  parser.add_custom_option(addOption);
  parser.add_custom_option(commitOption);

  if (argc == 1) {
    parser.help(); 
  }

  parser.parse(argc, argv);
  return 0;
}

