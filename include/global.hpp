#include <iostream>
#include "SHA256.hpp"

// TODO Write the Functions To use Right Here. 
// TODO Write the Globals Here.

std::string calculateSHA256(const std::string& message) {
    static SHA256 sha;
    
    sha.update(message);
    uint8_t* digest = sha.digest();
    std::string result = SHA256::toString(digest);
    delete[] digest;

    return result;
}

// TODO Implement An Object Hashing Algorithm 