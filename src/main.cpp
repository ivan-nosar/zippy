#include <iostream>
#include <vector>
#include <string>

#include "lib/sha256.cpp"

using namespace std;

int main(int argc, char* argv[])
{
    vector<string> files = vector<string> {
        ".testFiles/file.json",
        // ".testFiles/file.jpeg",
        // ".testFiles/file.png",
        // ".testFiles/file.mp4",
    };

    for (string& filePath : files) {
        cout << "Evaluating the SHA256 hash for the '" << filePath << "' file" << endl;
        string fileHash = getHashString(filePath);
        cout << fileHash << endl << "=============================" << endl;
    }

    cout << endl;
}
