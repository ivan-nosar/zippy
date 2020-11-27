#include <iostream>
#include <vector>
#include <string>

#include "include/lib/sha256.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    vector<string> files = vector<string> {
        "/Users/ivan-nosar/Documents/petProjects/zippy/testFiles/file.json",
        // "/Users/ivan-nosar/Documents/petProjects/zippy/testFiles/file.jpeg",
        // "/Users/ivan-nosar/Documents/petProjects/zippy/testFiles/file.png",
        // "/Users/ivan-nosar/Documents/petProjects/zippy/testFiles/file.mp4",
    };

    for (string& filePath : files) {
        cout << "Evaluating the SHA256 hash for the '" << filePath << "' file" << endl;
        string fileHash = getHashString(filePath);
        cout << fileHash << endl << "=============================" << endl;
    }

    cout << endl;
}
