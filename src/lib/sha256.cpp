#include "../include/lib/sha256.hpp"
#include "../include/utils/fileUtils.hpp"

using namespace std;

string getHashString(string filePath) {
    if (!isFileExists(filePath)) {
        return "File not exists";
    }
    return "0";
}
