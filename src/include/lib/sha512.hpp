#pragma once

#include <string>

#include "../types.hpp"

using namespace std;

namespace sha2 {
    inline string sha512FromStream(istream &stream, uint64 messageSize);

    string sha512(string filePath);
}
