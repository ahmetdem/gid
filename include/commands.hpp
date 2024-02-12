#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "global.hpp"
#include "objects.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

// TODO: Implement The Commands Here For better Organization:

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

  std::string AUTHOR_NAME = "Ahmet Yusuf Demir";
  std::string COMMIT_MESSAGE = "Initial Commit!!";

  const fs::path CURRENT_PATH = fs::current_path();
  const fs::path GID_DIRECTORY = CURRENT_PATH / ".gid";

  // TODO: Check if a repository already exists at 'GID_DIRECTORY'.
  if (fs::is_directory(GID_DIRECTORY)) {
    std::cerr << "Already Existing Repository." << std::endl;
    return;
  }

  // Create a directory for the repository at 'GID_DIRECTORY'.
  fs::create_directory(GID_DIRECTORY);
  fs::create_directory(GID_DIRECTORY / "objects");

  // Create the config file (adjust file content as needed)
  fs::path configPath = GID_DIRECTORY / "config";
  std::ofstream configFile(configPath);
  // configFile << "repository=" << repoName << std::endl;  // Set repository
  // name
  configFile.close();

  // Create the HEAD file (adjust contents for your initial state)
  fs::path headPath = GID_DIRECTORY / "HEAD";
  std::ofstream headFile(headPath);
  headFile << "ref: refs/heads/main" << std::endl; // Set the initial branch
  headFile.close();

  // Create the index file (for staging changes)
  fs::path indexPath = GID_DIRECTORY / "index";
  std::ofstream indexFile(indexPath);
  indexFile.close();

  fs::path descriptionPath = GID_DIRECTORY / "description";
  std::ofstream descriptionFile(descriptionPath);
  descriptionFile.close();

  fs::path commitsPath = GID_DIRECTORY / "commits";
  std::ofstream commitsFile(commitsPath);
  commitsFile.close();

  Tree initialTree = createTree(CURRENT_PATH);
  std::string hashedTree { serializeObject<Tree>(initialTree) };
  
  Commit initialCommit(AUTHOR_NAME, COMMIT_MESSAGE,
                      hashedTree);

  /* Store the Objects in ./gid/objects folder with first
   two chars of hashes being subdirectory name and rest being
  the name of the file that contains content of the objects. */

  storeObject<Commit>(initialCommit, "");
  storeObject<Tree>(initialTree, initialCommit.treeHash);

  std::cout << "Repository is Created Successfully." << std::endl;
}

inline void addCommand() {
  const std::unordered_set<std::string> seenPaths = Add::identify_changes_and_update_index(); 
  Add::store_added_content(seenPaths);
}

inline void commitCommand() {
  // Get the content of the index file.
  std::ifstream indexFile("./.gid/index");
  std::string line;

  std::vector<std::tuple<std::string, std::string>> oldNewHash;

  while (std::getline(indexFile, line)) {
    std::istringstream iss(line);
    std::string storedPath, storedHash, newHash, op;

    // Extract path and hash from the line
    iss >> op >> storedPath >> storedHash >> newHash; 
   
    if (!fs::exists(storedPath)) continue; 

    if (op == "DELETED") {
      std::cout << "DELETED" << storedPath << "\n";

    } else if (op == "CREATED") {
      std::cout << "CREATED" << storedPath << "\n";

    } else { 
      std::cout << "CHANGED" << storedPath << "\n";
    }
  }

  indexFile.close();
}

inline void statusCommand() {}
inline void logCommand() {}

#endif
