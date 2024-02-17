#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "global.hpp"
#include "objects.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>

// TODO: Implement The Commands Here For better Organization:

/* NOTE: 
 *  - init: Initialize a new repository.
    - add: Stage changes for commit.
    - commit: Create a new commit with staged changes.
    - log: Display commit history. */

namespace fs = std::filesystem;

const fs::path CURRENT_PATH = fs::current_path();
const fs::path GID_DIRECTORY = CURRENT_PATH / ".gid";

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
  std::string line, storedPath, storedHash, newHash, op;
  // Get the content of the index file.
  std::ifstream indexFile("./.gid/index");

  // Check if the index file is empty
  if (indexFile.peek() == std::ifstream::traits_type::eof()) {
    std::cout << "Index file is empty. No changes to commit." << std::endl;
    indexFile.close(); // Close the file
    return; // Exit the function without committing
  }

  // Reset the file pointer to the beginning of the file
  indexFile.clear();
  indexFile.seekg(0);

  const fs::path masterTreePath { General::getMasterTreePath() };
  const std::unordered_set<TreeEntry, TreeEntry::Hash> storedEntries { General::getStoredEntries(masterTreePath) };

  const fs::path objectsPath = fs::current_path() / ".gid/objects";
  std::unordered_set<std::string> indexPaths;

  while (std::getline(indexFile, line)) {
    std::istringstream iss(line);

    iss >> op >> storedPath >> storedHash >> newHash; 
    indexPaths.insert(storedPath);
    
    if (op == "DELETED ") {
      std::cout << "DELETED" << storedPath << "\n";
      continue;
    }

    if (!fs::exists(storedPath)) continue; 
  }

  indexFile.close(); // Close the file before reopening in write mode

  // Reopen the index file in write mode and truncate its content
  std::ofstream indexFileClear("./.gid/index", std::ios::out | std::ios::trunc);
  indexFileClear.close(); // Close the file after truncating

  Tree tree { createTree(CURRENT_PATH, storedEntries) };

  std::string treeHash { serializeObject<Tree>(tree) };
  Commit commit("Ahmet Yusuf Demir", "Commit Test", 
          treeHash);

  storeObject<Commit>(commit, "");
  storeObject<Tree>(tree, commit.treeHash);

  std::cout << "Commit is Successfully Made!!" << std::endl;
}


inline void retrieveCommand(const std::string& commitHash) {

  fs::path objectsPath = "./.gid/objects";
  fs::path commitPath = objectsPath / commitHash.substr(0, 2) / commitHash.substr(2);

  if (!fs::exists(commitPath)) {
    std::cerr << "Commit Path does not exist.\nUse `./gid log` to see valid commits." << std::endl;
    return;
  }

  std::ifstream commitFile(commitPath);
  if (!commitFile.is_open()) {
    std::cerr << "Failed to open commit file." << std::endl;
    return;
  }

  std::string treeHash; 
  std::string line;
  while (std::getline(commitFile, line)) {
    // Store the last non-empty line
    if (!line.empty()) {
        treeHash = line;
    }
  }

  size_t colonPos = treeHash.find(':');
  if (colonPos != std::string::npos) {
      // Extract the substring after ':'
      treeHash = treeHash.substr(colonPos + 1);

      // Trim any leading or trailing whitespace
      treeHash.erase(0, treeHash.find_first_not_of(" \t\r\n"));
      treeHash.erase(treeHash.find_last_not_of(" \t\r\n") + 1);
      
      std::cout << treeHash << std::endl;
  }
  commitFile.close();
   
  fs::path treePath = objectsPath / treeHash.substr(0, 2) / treeHash.substr(2);

  createRetrievedFile(treePath);
}

inline void logCommand() {
  std::ifstream commitsFile("./.gid/commits");
  std::string commitHash;

  while (std::getline(commitsFile, commitHash)) {
    std::cout << "Commit Hash is: " << commitHash << "\n";

    fs::path objectsPath = "./.gid/objects";
    fs::path commitPath = objectsPath / commitHash.substr(0, 2) / commitHash.substr(2);

    std::ifstream commitFile(commitPath);
    std::string line;

    while (std::getline(commitFile, line)) {
      std::cout << line << "\n";  
    }

    std::cout << "\n";
  }
}

#endif
