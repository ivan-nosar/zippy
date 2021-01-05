#include <iostream>
#include <vector>
#include <string>

#include "include/lib/sha256.hpp"
#include "include/utils/hexadecimal.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    const char *filePath = argv[1];
    string stringHash = sha2::sha256(filePath);
    cout << stringHash << "  " << filePath << endl;
}
