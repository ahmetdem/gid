#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <vector>
#include <string>
#include <iostream>

// TODO: Implement the Blob object
// Implement the Commit object  
// Implement the Tree object

// OPTIONAL Implement the Tag Object

struct Commit {
    std::string authorName;  // Committer's name
    std::string timestamp;      // Date and time of the commit
    std::string message;        // Commit message
    std::string treeHash;       // SHA-1 hash of the top-level tree object
    // Add more fields as needed

    // Constructor
    Commit(
        const std::string& authorName,
        const std::string& message,
        const std::string& treeHash
    ) 
        : authorName(authorName),
          message(message),
          treeHash(treeHash) {
            
        // Set the timestamp to the current time
        timestamp = getCurrentTime();
    }

private:
    // Function to get the current time as a string
    std::string getCurrentTime() {
        std::time_t t = std::time(nullptr);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return buffer;
    }
}; 

struct TreeEntry {
	std::string name;
	std::string sha;
	std::string type;

    TreeEntry(const std::string& name, const std::string& sha, const std::string& type)
        : name(name), sha(sha), type(type) {}
};

struct Tree {
  std::vector<TreeEntry> entries;

	void addEntry(const std::string& name, const std::string& sha1, const std::string& type) {
		entries.push_back(TreeEntry(name, sha1, type));
    }

};

struct Blob {
    std::string content;

    Blob(const std::string& content) : content(content) {}
};


#endif