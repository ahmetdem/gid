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
#include <unordered_set>

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
    std::cerr << "Error opening commits folder. \nYou probably need to initilize repository." << std::endl;

  while (std::getline(file, line)) {
    masterCommitHash = line;
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

inline std::unordered_set<TreeEntry, TreeEntry::Hash> getStoredEntries(const fs::path& treePath) {
  static std::unordered_set<TreeEntry, TreeEntry::Hash> storedEntries;

  // Get all the paths from the repo. 
  std::ifstream masterTreeFile(treePath);
  bool isFirstLine = true; std::string line, path, hash, type;
  const fs::path objectsPath = fs::current_path() / ".gid/objects";

  while (std::getline(masterTreeFile, line)) {
    if (isFirstLine) {
      isFirstLine = false;
      continue;
    }

    std::istringstream iss(line); 
    iss >> path >> hash >> type;

    if (fs::is_directory(path)) {
      fs::path treePath =
        objectsPath / hash.substr(0, 2) / hash.substr(2);

      getStoredEntries(treePath);
    } 

    TreeEntry treeEntry (path, hash, type);
    storedEntries.insert(treeEntry);  
  }

  return storedEntries;
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
inline Tree createTree(const fs::path &directoryPath,
    const std::unordered_set<TreeEntry, TreeEntry::Hash>& storedEntries = {}) {

  // TODO: Add an Option to exclude some type of files.
  // Iterate over the files and subdirectories in the specified director

  Tree tree;
  const fs::path objectsPath = fs::current_path() / ".gid/objects";

  for (auto const &dir_entry : fs::directory_iterator(directoryPath)) {
    // Exclude the .git files (duh).
    if (dir_entry.path().string().find(".git") != std::string::npos ||
        dir_entry.path().string().find(".gid") != std::string::npos)
      continue;

    TreeEntry currentEntry {dir_entry.path(), "", ""}; // Create a temporary TreeEntry for comparison
    auto it = storedEntries.find(currentEntry);
    if (it != storedEntries.end() && fs::is_directory(dir_entry)) {
      const TreeEntry& storedEntry = *it;
      tree.addEntry(storedEntry.relativePath, storedEntry.sha, storedEntry.type);
      continue;
    }

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
      Tree subTree = createTree(dir_entry.path().string(), storedEntries);
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
        std::cerr << "Error opening Commits Folder! \nYou probably need to initilize repository." << std::endl;

      commit_file << hashedNameCommit << "\n";
      commit_file.close();
    }
  }
}

namespace Add {

// Tell the compiler the function exists.
inline void
identify_changes_and_update_index_recursive(const std::string&, std::unordered_set<std::string>&);

// TODO: Add operation parameter to keep track of what type of change it is.
inline bool isPathStored(const std::string &path) {
  std::ifstream indexFile("./.gid/index");
  std::string line, storedPath, trash;

  while (std::getline(indexFile, line)) {
    std::istringstream iss(line);

    // Extract path and hash from the line
    iss >> trash >> storedPath;

    if (storedPath == path) {
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

  if (!isPathStored(file_path.string())) {
    std::ofstream index_file("./.gid/index", std::ios::app);
        
    switch (op) {
      case Operation::CREATED:
        index_file << "CREATED ";
        break;

      case Operation::CHANGED:
        index_file << "CHANGED ";
        break;

      case Operation::DELETED:
        index_file << "DELETED ";
        break;

      default:
        std::cerr << "Unknown Operation!" << std::endl; 
        break;
    }
    
    index_file << file_path.string() << " " << changed_hash << " " << newHash << "\n";
    index_file.close();
    std::cout << "A Change is Made in: " << file_path.string() << " \n";
  } 
}

inline void store_added_content(const std::unordered_set<std::string>& seenPaths) { 
  for (auto const& dir_entry : fs::recursive_directory_iterator(fs::current_path())){
    if (dir_entry.path().string().find(".git") != std::string::npos ||
      dir_entry.path().string().find(".gid") != std::string::npos)
      continue; 

    if (fs::is_regular_file(dir_entry)) {
      if (seenPaths.count(dir_entry.path().string()) <= 0) {
        Add::storeIndex("|", dir_entry, Operation::CREATED); 
      }
    }
  }
}

inline std::unordered_set<std::string> identify_changes_and_update_index() {
  fs::path masterTreePath = General::getMasterTreePath(); 
  std::unordered_set<std::string> seenPaths = {};
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
        seenPaths.insert(file_path.string());

      } else {
        std::cout << "file does not exists" << std::endl;
        Add::storeIndex(hash, file_path, Operation::DELETED);
        continue;
      }

      // if not equal, store it inside the index file.
      if (hashToCompare != hash) {
        Add::storeIndex(hash, file_path, Operation::CHANGED, hashToCompare);
      }
    } else {
      // it's a tree object, go to the hash of the tree object and call the
      // Function
      Add::identify_changes_and_update_index_recursive(hash, seenPaths);
    }
  }

  return seenPaths;
}

inline void
identify_changes_and_update_index_recursive(const std::string &hash, std::unordered_set<std::string>& seenPaths) {
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
        seenPaths.insert(file_path.string());

      } else {
        std::cout << "file does not exists" << std::endl;
        Add::storeIndex(hash, file_path, Operation::DELETED);
        continue;
      }

      // if not equal, store it inside the index file.
      if (hashToCompare != hash) {
        Add::storeIndex(hash, file_path, Operation::CHANGED , hashToCompare);
      }
    } else {

      Add::identify_changes_and_update_index_recursive(hash, seenPaths);
    }
  }
}
} // namespace Add

inline void retrieveBlobObject(const fs::path& blobPath, const std::string& hash) { 
  std::ifstream blobFile(blobPath);
  if (!blobFile.is_open()) {
      std::cerr << "Failed to open blob file." << std::endl;
      return;
  }

  fs::path outputDir = fs::current_path() / hash.substr(0, 3), outputPath;
  std::cout << "Retrieved Repo has been written to the: " << outputDir << std::endl;

  if (!fs::exists(outputDir)) {
    fs::create_directories(outputDir);
  }

  std::string line, path, content;
  bool isFirstLine = true;

  while (std::getline(blobFile, line)) {
    if (isFirstLine) {
      std::istringstream iss(line);
      iss >> line >> path; 

      outputPath = outputPath = outputDir / fs::relative(path, fs::current_path());
      
      isFirstLine = false;
      continue;
    }
    content += line + "\n";
  }

  fs::create_directories(outputPath.parent_path());

  std::ofstream outputFile(outputPath);
  if (!outputFile.is_open()) {
    std::cout << outputPath << "\n";
    std::cerr << "Failed to create output file." << std::endl;
    return;
  }

  outputFile << content;
}

inline void createRetrievedFile(const fs::path& treePath) {
  std::ifstream treeFile(treePath);
  if (!treeFile.is_open()) {
    std::cerr << "Failed to open tree file." << std::endl;
    return;
  }

  std::string path, hash, type, line; 
  fs::path objectsPath = "./.gid/objects";
  bool isFirstLine = true;
  
  while (std::getline(treeFile, line)) {
    if (isFirstLine) {
      isFirstLine = false;
      continue;
    }

    std::istringstream iss(line);
    iss >> path >> hash >> type;

    if (type == "blob") {
      fs::path blobPath = objectsPath / hash.substr(0, 2) / hash.substr(2);
      retrieveBlobObject(blobPath, hash); 

    } else {
      fs::path subTreePath = objectsPath / hash.substr(0, 2) / hash.substr(2); 
      createRetrievedFile(subTreePath);
    }
 
    // std::cout << "Path is: " << path << "\n";
  } 
}

#endif
