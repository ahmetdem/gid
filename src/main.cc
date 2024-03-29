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
  CommandLineParser::Option logOption ("log", "Show the Log of the Commits", logCommand);

  CommandLineParser::Option retrieveOption ("retrieve", "Retrieve a specific commit.", [argv, argc]() {
    if (argc != 3) {
        std::cout << "Usage: <program_name> retrieve <commit_hash>" << std::endl;
        return;
    }

    retrieveCommand(argv[2]); 
  });

  CommandLineParser::Option helpOption ("--help", "Get help.", []() {
      std::cout << "Usage of the program is as follows:\n"
                << "1. with `./gid init` command Initialize a Repository.\n"
                << "2. with `./gid add` command add changes if you got any.\n"
                << "3. with `./gid commit` command push the changes to the repo.\n"
                << "4. with `./gid log` command see the Commits you made.\n"
                << "5. retrieve the commit by Using `./gid retrieve <commit_hash>`." 
                << std::endl;
  });
 
  parser.add_custom_option(initOption);
  parser.add_custom_option(addOption);
  parser.add_custom_option(commitOption);
  parser.add_custom_option(logOption);
  parser.add_custom_option(retrieveOption);
  parser.add_custom_option(helpOption);

  if (argc == 1) {
    parser.help(); 
  }

  parser.parse(argc, argv);
  return 0;
}

