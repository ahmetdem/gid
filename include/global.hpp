#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "SHA256.hpp"
#include "objects.hpp"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

namespace fs = std::filesystem;

template <typename T>
inline void storeObject(const T &object, const std::string &hashed);

enum Operation {
  DELETED,
  CREATED,
  CHANGED,
};

namespace General {
/**
 * This function calculates a hash with SHA256 Algorithm.
 *
 * @param message The message to be hashed.
 * @return The hash of the message.
 */
inline std::string calculateSHA256(const std::string &message) {
  SHA256 sha;

  sha.update(message);
  uint8_t *digest = sha.digest();
  std::string result = SHA256::toString(digest);
  delete[] digest;

  return result;
}

inline std::tuple<fs::path, std::string, std::string>
parseLine(const std::string &str, const char &indic) {
  int left = 0, length = static_cast<int>(str.length());
  std::tuple<fs::path, std::string, std::string> result{"", "", ""};

  int substring_index = 0;
  for (int right = 0; right <= length && substring_index < 3; right++) {
    if (right == length || str[right] == indic || str[right] == '\t') {
      if (!(right - left == 0)) {
        switch (substring_index) {
        case 0:
          std::get<0>(result) = str.substr(left, right - left);
          break;
        case 1:
          std::get<1>(result) = str.substr(left, right - left);
          break;
        case 2:
          std::get<2>(result) = str.substr(left, right - left);
          break;
          // Handle additional cases if needed
        }
        substring_index++;
      }
      left = right + 1; // Move left to the next character
    }
  }

  return result; // Return the filled tuple
}

inline fs::path getMasterTreePath() {
  // Get the hash of the general tree object, Go to commits file, from there to
  // the general tree object.
  std::ifstream file("./.gid/commits", std::ios::binary);
  std::string line, masterCommitHash, masterTreeHash;

  if (file.fail())
    std::cerr << "Error opening commits folder." << std::endl;

  // TODO: Implement the Algorithm to get commits here. Just use something like
  // getRecent() in here. 
  // FIX: You probably will need to get the last hash not
  // the first.
  while (std::getline(file, line)) {
    masterCommitHash = line;
    break;
  }
  file.close();

  // Go to the masterCommitHash to reach the masterTreeHash
  const fs::path objectsPath = "./.gid/objects";
  const fs::path masterCommitPath =
      objectsPath / masterCommitHash.substr(0, 2) / masterCommitHash.substr(2);

  std::ifstream masterCommitFile(masterCommitPath, std::ios::binary);
  if (masterCommitFile.fail()) {
    std::cerr << "Error opening MasterCommitFile!" << std::endl;
  }

  while (std::getline(masterCommitFile, line)) { 
    masterTreeHash = line;
  }
  masterCommitFile.close();

  // Go inside the masterTreeFile and loop over the hashes inside it;
  masterTreeHash = masterTreeHash.substr(9);
  const fs::path masterTreePath =
      objectsPath / masterTreeHash.substr(0, 2) / masterTreeHash.substr(2);

  return masterTreePath;
}

} // namespace General

/**
 * Serializes the provided object into a string representation.
 *
 * This function supports serialization of Tree, Commit, and Blob objects.
 * For each object type, the corresponding data members are serialized into a
 * string using a stringstream.
 *
 * @tparam T The type of object to serialize.
 * @param object The object to serialize.
 * @return A string representation of the serialized object.
 */
template <typename T> inline std::string serializeObject(const T &object) {
  std::ostringstream ss;

  // Serialize object data into the stringstream
  if constexpr (std::is_same<T, Tree>::value) {
    for (const TreeEntry &entry : object.entries) {
      ss << entry.relativePath.string() << entry.sha << entry.type;
    }

  } else if constexpr (std::is_same<T, Commit>::value) {
    // Serialize commit object data
    ss << object.authorName << object.message << object.timestamp
       << object.treeHash;

  } else if constexpr (std::is_same<T, Blob>::value) {
    // Serialize blob object data
    ss << object.content;
  }

  return General::calculateSHA256(ss.str());
}

/**
 *  Function to create a Blob from a file
 *
 * @param filePath The filePath to be Turned into a Blob Object
 * @return The blob object of the file.
 */
inline Blob createBlob(const fs::path &filePath) {
  std::ifstream file(filePath);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file.");
  }

  std::string content, line;

  while (std::getline(file, line)) {
    content += line + "\n"; // Read the file line by line
  }

  file.close();

  return Blob(content, filePath);
}

/**
 * Recursively generates a tree object to represent the directory structure
 * and its contents starting from the specified directory.
 *
 * @param directoryPath The path to the root directory to create a tree from.
 *
 * @returns A 'Tree' object representing the directory structure and its
 * contents. The 'Tree' contains entries for both files and subdirectories, with
 *          each entry including its name, SHA-2 hash, and type (blob or tree).
 */
inline Tree createTree(const fs::path &directoryPath) {

  Tree tree;
  // TODO: Add an Option to exclude some type of files.
  // Iterate over the files and subdirectories in the specified director

  // Parse the lines.
  // Compare it with the previous.y

  const fs::path objectsPath = fs::current_path() / ".gid/objects";

  for (auto const &dir_entry : fs::directory_iterator(directoryPath)) {
    // Exclude the .git files (duh).
    if (dir_entry.path().string().find(".git") != std::string::npos ||
        dir_entry.path().string().find(".gid") != std::string::npos)
      continue;

    if (fs::is_regular_file(dir_entry)) {
      // It's a file, create a blob object
      // Read the content of the file (you'll need to implement this)

      Blob blob{createBlob(dir_entry)};
      std::string blobContent = blob.getContent();

      // Compute the SHA-1 hash for the content
      std::string hashedNameBlob = serializeObject<Blob>(blob);
      fs::path blobPath =
          objectsPath / hashedNameBlob.substr(0, 2) / hashedNameBlob.substr(2);

      // Store Blob objects right here.
      fs::create_directories(blobPath.parent_path());

      if (!fs::exists(blobPath)) {
        // BUG See what the binary mode is.
        std::ofstream file(blobPath, std::ios::binary);
        file.write(blobContent.c_str(), blobContent.size());
        file.close();
      }
      // Add an entry for the file to the tree
      tree.addEntry(dir_entry.path(), hashedNameBlob, "blob");

    } else {
      // It's a directory, call the function recursively
      // Compute the SHA-1 hash for the directory's name (for simplicity)
      std::string hashedNameTree =
          General::calculateSHA256(dir_entry.path().filename().string());

      // Create a subtree by calling the function recursively
      Tree subTree = createTree(dir_entry.path().string());
      storeObject<Tree>(subTree, hashedNameTree);

      // Add an entry for the subdirectory to the tree
      tree.addEntry(dir_entry.path(), hashedNameTree, "tree");
    }
  }
  return tree;
}

/**
 * Store an object of a specific type in the .gid objects folder.
 *
 * This function stores an object of a specified type (Tree, Commit, or Blob) in
 * the .gid objects folder by serializing the object's content and saving it
 * with its hash as the filename. If the object with the same hash already
 * exists, it will not be overwritten.
 *
 * @tparam T The type of the object to store (Tree, Commit, or Blob).
 * @param object The object to be stored.
 */
template <typename T>
inline void storeObject(const T &object, const std::string &hashed) {
  // OPTIONAL: Implement an Unlimited object parameter ?

  const fs::path objectsPath = ".gid/objects";

  if (!fs::exists(objectsPath)) {
    std::cerr << "The .gid Files are Corrupted. Objects folder can not be "
                 "found. Stop."
              << std::endl;
    return;
  }

  std::string content = object.getContent();

  // OPTIONAL Optimize the code a bit.
  // TODO Change ifs to switch case.
  if constexpr (std::is_same<T, Tree>::value) {
    // Store the Tree object

    std::string hashedNameTree =
        (hashed.empty()) ? serializeObject<Tree>(object) : hashed;
    fs::path treePath =
        objectsPath / hashedNameTree.substr(0, 2) / hashedNameTree.substr(2);

    // Create the directory if does not exist.
    fs::create_directories(treePath.parent_path());

    if (!fs::exists(treePath)) {
      // BUG See what the binary mode is.
      std::ofstream file(treePath, std::ios::binary);

      if (file.fail())
        std::cerr << "Error creating Tree file: " << treePath << std::endl;

      file.write(content.c_str(), content.size());
      file.close();
    }

  } else if constexpr (std::is_same<T, Commit>::value) {
    // Store the Commit Object
    std::string hashedNameCommit =
        (hashed.empty()) ? serializeObject<Commit>(object) : hashed;
    fs::path commitPath = objectsPath / hashedNameCommit.substr(0, 2) /
                          hashedNameCommit.substr(2);

    // Create the directory if does not exist.
    fs::create_directories(commitPath.parent_path());

    if (!fs::exists(commitPath)) {
      // BUG See what the binary mode is.
      std::ofstream file(commitPath, std::ios::binary);

      if (file.fail())
        std::cerr << "Error creating Commit file: " << commitPath << std::endl;

      file.write(content.c_str(), content.size());
      file.close();

      std::ofstream commit_file(".gid/commits", std::ios::app);

      if (file.fail())
        std::cerr << "Error opening Commits Folder!" << std::endl;

      commit_file << hashedNameCommit << "\n";
      commit_file.close();
    }
  }
}

namespace Add {

// Tell the compiler the function exists.
inline void
identify_changes_and_update_index_recursive(const std::string &hash);

// TODO: Add operation parameter to keep track of what type of change it is.
inline bool isHashStored(const std::string &hash) {
  std::ifstream indexFile("./.gid/index");
  std::string line;

  while (std::getline(indexFile, line)) {
    std::istringstream iss(line);
    std::string storedPath, storedHash;

    // Extract path and hash from the line
    iss >> storedPath >> storedHash;

    if (storedHash == hash) {
      indexFile.close();
      return true;
    }
  }

  indexFile.close();
  return false;
}

// Function to store a hash in the index file if it hasn't been stored yet
inline void storeIndex(const std::string &changed_hash,
                      const fs::path &file_path, 
                      const Operation& op = Operation::CHANGED,
                      const std::string &newHash = " | ") {

  if (!isHashStored(changed_hash)) {
    std::ofstream index_file("./.gid/index", std::ios::app);
    index_file << file_path.string() << " " << changed_hash << " " << newHash << " ";
    
    switch (op) {
      case Operation::CREATED:
        index_file << "CREATED\n";
        break;

      case Operation::CHANGED:
        index_file << "CHANGED\n";
        break;

      case Operation::DELETED:
        index_file << "DELETED\n";
        break;

      default:
        std::cerr << "Unknown Operation!" << std::endl; 
        break;
    }

    index_file.close();
    std::cout << "A Change is Made in: " << file_path.string() << " \n";
  } 
}

inline void identify_added_file() {
  
}

inline void identify_changes_and_update_index() {
  fs::path masterTreePath = General::getMasterTreePath();
  std::string line;

    std::ifstream masterTreeFile(masterTreePath, std::ios::binary);
  bool isFirstLine = true;
  while (std::getline(masterTreeFile, line)) {
    if (isFirstLine) {
      isFirstLine = false;
      continue; 
    }

    auto [file_path, hash, type] = General::parseLine(line, ' ');

    // If it's a file, simply calc another hash and compare with the previous
    if (type == "blob") {
      std::string hashToCompare;

      if (fs::exists(file_path)) {
        hashToCompare = serializeObject<Blob>(createBlob(file_path));
      } else {
        std::cout << "file does not exists" << std::endl;
        Add::storeIndex(hash, file_path, Operation::DELETED);

        continue;
      }

      // if not equal, store it inside the index file.
      if (hashToCompare != hash) {
        Add::storeIndex(hash, file_path, Operation::CHANGED, hashToCompare );
      }
    } else {
      // it's a tree object, go to the hash of the tree object and call the
      // Function
      Add::identify_changes_and_update_index_recursive(hash);
    }
  }
}

inline void
identify_changes_and_update_index_recursive(const std::string &hash) {
  // Get inside the file_path and get the hashes.
  fs::path objectsPath = "./.gid/objects";
  fs::path tree_path = objectsPath / hash.substr(0, 2) / hash.substr(2);

  std::ifstream treeFile(tree_path, std::ios::binary);
  bool isFirstLine = true;
  std::string line;

  while (std::getline(treeFile, line)) {
    if (isFirstLine) {
      isFirstLine = false;
      continue;
    }

    auto [file_path, hash, type] = General::parseLine(line, ' ');

    // If it's a file, simply calc another hash and compare with the previous
    if (type == "blob") {
      std::string hashToCompare;

      if (fs::exists(file_path)) {
        hashToCompare = serializeObject<Blob>(createBlob(file_path));
      } else {
        // TODO: Handle the case where a file is deleted or renamed.
        // Ultimately make a OPT variable to store the type of the change.
        std::cout << "file does not exists" << std::endl;
        Add::storeIndex(hash, file_path, Operation::DELETED);
        continue;
      }

      // if not equal, store it inside the index file.
      if (hashToCompare != hash) {
        Add::storeIndex(hash, file_path, Operation::CHANGED , hashToCompare);
      }
    } else {

      Add::identify_changes_and_update_index_recursive(hash);
    }
  }
}
} // namespace Add

#endif
