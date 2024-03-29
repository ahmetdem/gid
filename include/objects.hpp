#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

// TODO: Implement the Blob object
// Implement the Commit object
// Implement the Tree object

// OPTIONAL Implement the Tag Object
// FIXME: Fix the problem when including global.hpp to this file.

/**
 * Represents a commit in a version control system.
 *
 * This struct encapsulates information about a commit, including the author's
 * name, timestamp, commit message, and the SHA-2 hash of the top-level tree
 * object.
 */
struct Commit {
  std::string authorName; // Committer's name
  std::string timestamp;  // Date and time of the commit
  std::string message;    // Commit message
  std::string treeHash;   // SHA-2 hash of the top-level tree object

  // Constructor
  Commit(const std::string &authorName, const std::string &message,
         const std::string &treeHash)
      : authorName(authorName), message(message), treeHash(treeHash) {
    // Set the timestamp to the current time
    timestamp = getCurrentTime();
  }

  /**
   * Retrieve the content of a Commit as a concatenated string.
   *
   * This function returns a string that combines the Commit's attributes,
   * including the author's name, timestamp, commit message, and the SHA-2 hash
   * of the top-level tree object, separated by a new line.
   *
   * @return A string containing the combined attributes of the Commit.
   */
  std::string getContent() const {
    return "commit:\nname:" + authorName + "\ntimestamp:" + timestamp +
           "\nmessage:" + message + "\ntreehash:" + treeHash + "\n";
  }

private:
  // Function to get the current time as a string
  std::string getCurrentTime() {
    time_t t = time(nullptr);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return buffer;
  }
};

/**
 * Represents an entry in a tree object.
 *
 * A `TreeEntry` encapsulates information about a file or directory in a tree
 * object, including its name, SHA-2 hash, and type (blob or tree).
 */
struct TreeEntry {
  std::filesystem::path relativePath;
  std::string sha, type;

  TreeEntry(const std::filesystem::path &relativePath, const std::string &sha,
            const std::string &type)
      : relativePath(relativePath), sha(sha), type(type) {}

  bool operator==(const TreeEntry& other) const {
    return relativePath == other.relativePath;
  }

  // For unordered_set 
  struct Hash
  {
    size_t operator()(const TreeEntry& treeEntry) const
    {
      // Combine hash values of relevant fields using bitwise operations:
      std::size_t path_hash = std::hash<std::string>()(treeEntry.relativePath.extension().string());
      std::size_t sha_hash = std::hash<std::string>()(treeEntry.sha);
      std::size_t type_hash = std::hash<std::string>()(treeEntry.type);

      // Combine hashes using a mix function to improve uniformity:
      return ((path_hash << 5) + sha_hash) ^ type_hash;
    }
  }; 
};

/**
 * Represents a tree object in a version control system.
 *
 * A `Tree` is a collection of `TreeEntry` objects, representing the structure
 * of a directory in a version control system. It can be used to represent a
 * snapshot of a directory at a specific point in time.
 */
struct Tree {
  std::vector<TreeEntry> entries;

  /**
   * Add an entry to the tree.
   *
   * @param name The name of the file or directory.
   * @param sha The SHA-1 hash of the object.
   * @param type The type of the object (blob or tree).
   */
  void addEntry(const std::filesystem::path &relativePath, const std::string &sha,
                const std::string &type) {
    entries.push_back(TreeEntry(relativePath, sha, type));
  }

  std::string getContent() const {
    std::string content;
    for (const TreeEntry &entry : entries) {
      content += entry.relativePath.string() + " " + entry.sha + " " + entry.type + "\n";
    }
    return "tree:\n" + content;
  }
};

/**
 * Represents a blob object in a version control system.
 *
 * A `Blob` contains the content of a file, represented as a string. It is a
 * fundamental unit of data in version control systems and can be used to store
 * file contents.
 */
struct Blob {
  std::string content;
  std::filesystem::path relativePath;

  // Constructor
  Blob(const std::string &content, const std::filesystem::path &relativePath)
      : content(content), relativePath(relativePath) {}

  std::string getContent() const { return "blob: " + relativePath.string() + "\n" + content; }
};

#endif
