#pragma once

#include <string>
#include <vector>

#include "../types.hpp"

using namespace std;

string toHex(vector<uint32> data, bool useColonDelimeter = false);

string toHex(vector<uchar> data, bool useColonDelimeter = false);
