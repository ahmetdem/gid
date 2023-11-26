#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <filesystem>
#include <fstream>
#include "objects.hpp"
#include "global.hpp"

// TODO: Implement The Commands Here 

/* NOTE: - init: Initialize a new repository.
    - add: Stage changes for commit.
    - commit: Create a new commit with staged changes.
    - push: Push local changes to a remote repository.
    - status: Show the status of files in the working directory.
    - log: Display commit history. */

namespace fs = std::filesystem;

/**Initializes a new Git repository in the current working directory.
 *
 * This function creates the necessary directory structure and files for a Git
 * repository, including the `.gid` directory, `config`, `HEAD`, `index`, and
 * `description` files.
 */
inline void initCommand() {

    // OPTIONAL Add various things in config and description.

    // TODO find a way to get author name and commit message.
    std::string AUTHOR_NAME = "Ahmet Yusuf Demir" ;
    std::string COMMIT_MESSAGE = "Initial Commit!!";

    const fs::path CURRENT_PATH = fs::current_path();
    const fs::path GID_DIRECTORY = CURRENT_PATH / ".gid";

    // TODO: Check if a repository already exists at 'GID_DIRECTORY'.
    if (fs::is_directory(GID_DIRECTORY)) { 
        std::cerr << "Already Existing Repository." << std::endl; 
        return;
    }
    
    // Create a directory for the repository at 'GID_DIRECTORY'.
    fs::create_directory(GID_DIRECTORY); fs::create_directory(GID_DIRECTORY / "objects");

    // Create the config file (adjust file content as needed)
    fs::path configPath = GID_DIRECTORY / "config";
    std::ofstream configFile(configPath);
    // configFile << "repository=" << repoName << std::endl;  // Set repository name
    configFile.close();

    // Create the HEAD file (adjust contents for your initial state)
    fs::path headPath = GID_DIRECTORY / "HEAD";
    std::ofstream headFile(headPath);
    headFile << "ref: refs/heads/main" << std::endl;  // Set the initial branch
    headFile.close();

    // Create the index file (for staging changes)
    fs::path indexPath = GID_DIRECTORY / "index";
    std::ofstream indexFile(indexPath);
    indexFile.close();

    fs::path descriptionPath = GID_DIRECTORY / "description";
    std::ofstream descriptionFile(descriptionPath);
    descriptionFile.close();

    Tree initialTree = createTree(CURRENT_PATH);
    Commit initialCommit(AUTHOR_NAME, COMMIT_MESSAGE, serializeObject<Tree>(initialTree));

    /* Store the Objects in ./gid/objects folder with first 
     two chars of hashes being subdirectory name and rest being 
    the name of the file that contains content of the objects. */

    storeObject<Commit>(initialCommit, "");
    storeObject<Tree>(initialTree, "");

    std::cout << "Repository is Created Successfully." << std::endl;
}

inline void commitCommand () {}
inline void addCommand() {}
inline void statusCommand() {}
inline void logCommand() {}

#endif
