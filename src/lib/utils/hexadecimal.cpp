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

string toHex(vector<uint64> data, bool useLittleEndian, bool useColonDelimeter) {
    vector<uchar> buffer;
    buffer.reserve(data.size() * sizeof(uint64));
    for (int i = 0; i < data.size(); i++) {
        if (useLittleEndian) {
            buffer.push_back((data[i] << 56) >> 56);
            buffer.push_back((data[i] << 48) >> 56);
            buffer.push_back((data[i] << 40) >> 56);
            buffer.push_back((data[i] << 32) >> 56);
            buffer.push_back((data[i] << 24) >> 56);
            buffer.push_back((data[i] << 16) >> 56);
            buffer.push_back((data[i] << 8) >> 56);
            buffer.push_back(data[i] >> 56);
        } else {
            buffer.push_back(data[i] >> 56);
            buffer.push_back((data[i] << 8) >> 56);
            buffer.push_back((data[i] << 16) >> 56);
            buffer.push_back((data[i] << 24) >> 56);
            buffer.push_back((data[i] << 32) >> 56);
            buffer.push_back((data[i] << 40) >> 56);
            buffer.push_back((data[i] << 48) >> 56);
            buffer.push_back((data[i] << 56) >> 56);
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

    stringBuilder << std::setw(2) << std::setfill('0') << (uint32)data[0];
    for(int i = 1; i < data.size(); i++) {
        if (useColonDelimeter) {
            stringBuilder << ":";
        }
        stringBuilder << std::setw(2) << std::setfill('0') << (uint32)data[i];
    }

    return stringBuilder.str();
}
