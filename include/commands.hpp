#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <filesystem>
#include <fstream>

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

    std::string AUTHOR_NAME ;
    std::string COMMIT_MESSAGE ;

    const fs::path CURRENT_PATH = fs::current_path();
    const fs::path GID_DIRECTORY = CURRENT_PATH / ".gid";

    // TODO: Check if a repository already exists at 'GID_DIRECTORY'.
    if (fs::is_directory(GID_DIRECTORY)) { 
        std::cerr << "Already Existing Repository." << std::endl; 
        return;
    }
    
    // TODO: Create a directory for the repository at 'GID_DIRECTORY'.
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

    std::cout << "Repository is Created Successfully." << std::endl;

    // TODO: Create the initial commit representing the state of the repository at initialization.
    // Include metadata in the initial commit, such as the author's name, commit message, and a timestamp.

    // TODO: Store the initial commit object in the object storage.
    // Generate a unique hash for the initial commit using the implemented hashing algorithm.

    // TODO: Set the default branch (e.g., `main` or `master`) and point it to the initial commit.
    // Ensure that the branch is appropriately referenced in the repository's metadata.

    // TODO: Store any additional metadata related to branches, commits, and other repository information within the `.gid` directory.

    // TODO: Implement error-handling logic to address scenarios like invalid input, filesystem errors, or directory conflicts.

    // TODO: Display a completion message on the console, informing the user that the repository has been initialized successfully.
}

inline void commitCommand () {}
inline void addCommand() {}
inline void statusCommand() {}
inline void logCommand() {}

#endif
