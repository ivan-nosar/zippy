#pragma once

#include <string>

#include "../types.hpp"

using namespace std;

namespace sha2 {
    inline string sha256FromStream(istream &stream, uint64 messageSize);

    string sha256(string filePath);
}
