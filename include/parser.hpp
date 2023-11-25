#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <functional>
#include <filesystem>

class CommandLineParser {
public:

    // Define a structure to represent a option to parse.
    struct Option {
        const std::string name;
        const std::string description;
        std::function<void()> function;

        // Constructor overload for functions without arguments
        Option(const std::string& n, const std::string& desc, std::function<void()> func)
            : name(n), description(desc), function(func) {}
    };

    /* Option to add custom functions. */
    void add_custom_option(const Option& option) { options.push_back(option); }
    

    // Method to parse command-line arguments and execute associated functions
    void parse(const int argc, const char* argv[]) {
        for (int i = 1; i < argc; ++i) {            

            for (const Option op : options)
            {
                if (argv[i] == op.name) {
                    op.function();
                }
            }
        }
    }

private:

    Option OptionFromName(const char* name) {
        for (Option op : options)
        {
            if ( op.name == name )
            {
                return op;
            }
        }
        throw std::runtime_error("Option not found");
    }

    std::string stripLastWord(const std::string& path) {
        
        int index { 0 }; 
        for (size_t i = path.size() - 1; i > 0; i--)
        {
            if (path[i] == '/')
            {
                index = i;
                break;   
            }
        }
        return path.substr(index, path.size());
    }
    
    std::vector<Option> options {};

    Option helpOption { "--help", "Display help Message", []() {
        std::cout << "Help: Help has been arrived." << std::endl;
    }};

    Option versionOption {"--version", "Version Check", []() {
        std::cout << "Version: Version is 9.10" << std::endl;
    }};

};

#endif // PARSER_H

