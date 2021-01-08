#include "../../include/utils/fileValidator.hpp"

#include <stdexcept>
#include <filesystem>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

string validateFileForHashFunction(string filePath, uint64 maxMessageSize, HashFunction hashFunction) {
    // Check if file exists
    if (!fs::exists(filePath)) {
        throw invalid_argument("File not found" + filePath);
    }

    // Get the size of file (message)
    uint64 messageSize = fs::file_size(filePath);

    if (maxMessageSize != 0 && messageSize > maxMessageSize) {
        stringstream exceptionMessageBuilder;
        exceptionMessageBuilder <<
            "File exceeds maximum allowed size:\n\t" <<
            "Expected: less or equals " << (maxMessageSize) << "bytes\n\t" <<
            "Actual: " << messageSize;
        throw invalid_argument(exceptionMessageBuilder.str());
    }

    // Opening the file
    ifstream fileStream;
    fileStream.open(filePath);
    string result = hashFunction(fileStream, messageSize);
    fileStream.close();

    return result;
}
