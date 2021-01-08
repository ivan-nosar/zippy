#include "../../include/utils/hexadecimal.hpp"

#include <sstream>
#include <iomanip>

string toHex(vector<uint32> data, bool useLittleEndian, bool useColonDelimeter) {
    vector<uchar> buffer;
    buffer.reserve(data.size() * sizeof(uint32));
    for (int i = 0; i < data.size(); i++) {
        if (useLittleEndian) {
            buffer.push_back((data[i] << 24) >> 24);
            buffer.push_back((data[i] << 16) >> 24);
            buffer.push_back((data[i] << 8) >> 24);
            buffer.push_back(data[i] >> 24);
        } else {
            buffer.push_back(data[i] >> 24);
            buffer.push_back((data[i] << 8) >> 24);
            buffer.push_back((data[i] << 16) >> 24);
            buffer.push_back((data[i] << 24) >> 24);
        }
    }
    return toHex(buffer, useColonDelimeter);
}

string toHex(vector<uchar> data, bool useColonDelimeter) {
    if (data.size() == 0) {
        return "";
    }

    stringstream stringBuilder;
    stringBuilder << std::hex;

    stringBuilder << std::setw(2) << std::setfill('0') << (uint)data[0];

    if (data.size() > 1) {
        stringBuilder << std::setw(2) << std::setfill('0') << (uint)data[1];
        for(int i = 2; i < data.size(); i += 2) {
            if (useColonDelimeter) {
                stringBuilder << ":";
            }
            stringBuilder << std::setw(2) << std::setfill('0') << (uint)data[i];
            stringBuilder << std::setw(2) << std::setfill('0') << (uint)data[i + 1];
        }
    }

    return stringBuilder.str();
}
