#pragma once

#include <string>

#include "../types.hpp"

using namespace std;

namespace md {
    inline string md5FromStream(istream &stream, uint64 messageSize);

    string md5(string filePath);
}
