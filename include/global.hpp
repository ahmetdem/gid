#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <type_traits>
#include "SHA256.hpp"
#include "objects.hpp"

namespace fs = std::filesystem;

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
inline Blob createBlob(const fs::path& filename) {
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
 * Store an object of a specific type in the .gid objects folder.
 *
 * This function stores an object of a specified type (Tree, Commit, or Blob) in the
 * .gid objects folder by serializing the object's content and saving it with its hash
 * as the filename. If the object with the same hash already exists, it will not be
 * overwritten.
 *
 * @tparam T The type of the object to store (Tree, Commit, or Blob).
 * @param object The object to be stored.
 */
template <typename T>
inline void storeObject(const T& object) {
    // OPTIONAL: Implement an Unlimited object parameter ? 

    const fs::path objectsPath = fs::current_path() / ".gid/objects"; 

    if (!fs::exists(objectsPath)) {
        std::cerr << "The .gid Files are Corrupted. Objects folder can not be found. Stop." << std::endl;
        return;
    }
    
    std::string content = object.getContent();

    // OPTIONAL Optimize the code a bit.
    if constexpr (std::is_same<T, Tree>::value) {
        // Store the Tree object 

        // FIXME The problem when storing Trees.
        std::string hashedNameTree = serializeObject<Tree>(object);
        fs::path treePath = objectsPath / hashedNameTree.substr(0, 2) / hashedNameTree.substr(2);

        // Create the directory if does not exist.
        fs::create_directories(objectsPath.parent_path());

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
        std::string hashedNameCommit = serializeObject<Commit>(object);
        fs::path commitPath = objectsPath / hashedNameCommit.substr(0, 2) / hashedNameCommit.substr(2);
        
        // Create the directory if does not exist.
        fs::create_directories(commitPath.parent_path());

        if (!fs::exists(commitPath)) {
            // BUG See what the binary mode is.
            std::ofstream file(commitPath, std::ios::binary);

            if (file.fail())
                std::cerr << "Error creating Commit file: " << commitPath << std::endl;

            file.write(content.c_str(), content.size());
            file.close();
        }

    } else if constexpr (std::is_same<T, Blob>::value) {
        // Store the Blob Object
        std::string hashedNameBlob = serializeObject<Blob>(object);
        fs::path blobPath = objectsPath / hashedNameBlob.substr(0, 2) / hashedNameBlob.substr(2);

        std::cout << content << std::endl;
        // Create the directory if does not exist.
        fs::create_directories(blobPath.parent_path());

        if (!fs::exists(blobPath)) {
            // BUG See what the binary mode is.
            std::ofstream file(blobPath, std::ios::binary);
            file.write(content.c_str(), content.size());
            file.close();
        }
    }
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
Tree createTree(const fs::path& directoryPath) {
    
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
            Tree subTree = createTree(dir_entry.path().string());

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
        ss << object.content ;
    }

    return calculateSHA256(ss.str());
}

#endif