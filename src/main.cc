#include <iostream>
#include "commands.hpp"
#include "parser.hpp"
#include "global.hpp"

// TODO Write Logs in a Log file Continously.
// TODO Make a prototype To keep track of a repo and report if a change has occured.


int main(int argc, char const *argv[])
{   
    CommandLineParser parser;
    CommandLineParser::Option initOption ("init", "Initialize the Repository.", initCommand);

    parser.add_custom_option(initOption);
    parser.parse(argc, argv);

    return 0;
}
