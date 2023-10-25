#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <type_traits>
#include "SHA256.hpp"
#include "objects.hpp"

// TODO: Write the Functions To use Right Here. 
// Write the Globals Here.


/**
 * This function calculates a hash with SHA256 Algorithm.
 *
 * @param message The message to be hashed.
 * @return The hash of the message.
 */
inline std::string calculateSHA256(const std::string& message) {
    static SHA256 sha;
    
    sha.update(message);
    uint8_t* digest = sha.digest();
    std::string result = SHA256::toString(digest);
    delete[] digest;

    return result;
}

/**
 *  Function to create a Blob from a file 
 * 
 * @param filename The Filename to be Turned into a Blob Object
 * @return The blob object of the file.
 */
inline Blob createBlobFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // Handle the case where the file cannot be opened
        throw std::runtime_error("Failed to open file.");
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n"; // Read the file line by line
    }

    file.close();

    return Blob(content);
}


/**
 * Recursively generates a tree object to represent the directory structure
 * and its contents starting from the specified directory.
 *
 * @param directoryPath The path to the root directory to create a tree from.
 *
 * @returns A 'Tree' object representing the directory structure and its contents.
 *          The 'Tree' contains entries for both files and subdirectories, with
 *          each entry including its name, SHA-2 hash, and type (blob or tree).
 */
Tree createTreeFromDirectory(const std::string& directoryPath) {
    Tree tree;
    
    // TODO: Add an Option to exclude some type of files.
    // Iterate over the files and subdirectories in the specified directory

    for (auto const& dir_entry : fs::recursive_directory_iterator(directoryPath))
    {

        if (dir_entry.path().string().find(".git") != std::string::npos) 
            continue;

        if (fs::is_regular_file(dir_entry)) {
            // It's a file, create a blob object
            // Read the content of the file (you'll need to implement this)

            std::ifstream file(dir_entry.path(), std::ios::binary);
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            // Compute the SHA-1 hash for the content
            std::string hashedNameBlob = calculateSHA256(content);

            // Add an entry for the file to the tree
            tree.addEntry(dir_entry.path().filename(), hashedNameBlob, "blob");
        } else if (fs::is_directory(dir_entry)) {
            // It's a directory, call the function recursively
            // Compute the SHA-1 hash for the directory's name (for simplicity)

            std::string hashedNameTree = calculateSHA256(dir_entry.path().filename().string());

            // Create a subtree by calling the function recursively
            Tree subTree = createTreeFromDirectory(dir_entry.path().string());

            // Add an entry for the subdirectory to the tree
            tree.addEntry(dir_entry.path().filename(), hashedNameTree, "tree");
        }
    }

    return tree;
}

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
template <typename T>
inline std::string serializeObject(const T& object) {
    std::stringstream ss;

    // Serialize object data into the stringstream
    if constexpr (std::is_same<T, Tree>::value) {
        // Serialize tree object data
        for (const TreeEntry& entry : object.entries) {
            ss << entry.name << entry.sha << entry.type; }
            
    } else if constexpr (std::is_same<T, Commit>::value) {
        // Serialize commit object data
        ss << object.authorName << object.message << object.timestamp << object.treeHash ;

    } else if constexpr (std::is_same<T, Blob>::value) {
        // Serialize blob object data
        ss << object.content;

    }

    return calculateSHA256(ss.str());
}



#endif