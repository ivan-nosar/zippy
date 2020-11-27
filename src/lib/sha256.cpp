#include "../include/lib/sha256.hpp"

#include <stdexcept>
#include <filesystem>

using namespace std;

string getHashString(string filePath) {
    if (!filesystem::exists(filePath)) {
        throw invalid_argument("File not found" + filePath);
    }
    return "0";
}
