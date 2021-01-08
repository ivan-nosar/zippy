#pragma once

#include <string>
#include <istream>

#include "../types.hpp"

using namespace std;

typedef string HashFunction (istream &stream, uint64 messageSize);

string validateFileForHashFunction(string filePath, uint64 maxMessageSize, HashFunction hashFunction);
