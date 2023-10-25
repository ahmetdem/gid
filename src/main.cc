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

    // For test purposes.
    Tree projectTree = createTreeFromDirectory("./");

    for (const TreeEntry& entry : projectTree.entries) {
        std::cout << "Name: " << std::left << std::setw(20) << entry.name
                << "SHA-2: " << std::left << std::setw(40) << entry.sha
                << "Type: " << entry.type << std::endl;
    }


    parser.add_custom_option(initOption);
    parser.parse(argc, argv);

    return 0;
}

