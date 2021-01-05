#include "../include/lib/sha256.hpp"

#include <stdexcept>
#include <filesystem>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "../include/utils/hexadecimal.hpp"

namespace fs = std::filesystem;

const uchar CHUNK_SIZE_IN_BYTES = 64;
// The maximum allowed message size is 2 ^ 61 (limitations of SHA256 algorthm)
const uint64 MAX_MESSAGE_SIZE = 0x200000000000002F; // 2305843009213693999 == 2 ^ 61 - 1

// Use with care: don't pass the number higher than 31 as the `shift` value.
// This function don't perform any edge-case checks
inline uint32 rightRotate(uint32 original, uchar shift) {
    return (original << (32 - shift)) | (original >> shift);
}

inline uint32 packUint32(uchar a, uchar b, uchar c, uchar d) {
    return a << 24 | b << 16 | c << 8 | d;
}

inline void unpackUint64(uint64 arg, uchar *outputBuffer) {
    outputBuffer[0] = arg >> 56; // right shift for 56 bits
    outputBuffer[1] = (arg << 8) >> 56; // left shift for 8 bits | then right shift for 56 bits
    outputBuffer[2] = (arg << 16) >> 56; // left shift for 16 bits | then right shift for 56 bits
    outputBuffer[3] = (arg << 24) >> 56; // left shift for 24 bits | then right shift for 56 bits
    outputBuffer[4] = (arg << 32) >> 56; // left shift for 32 bits | then right shift for 56 bits
    outputBuffer[5] = (arg << 40) >> 56; // left shift for 40 bits | then right shift for 56 bits
    outputBuffer[6] = (arg << 48) >> 56; // left shift for 48 bits | then right shift for 56 bits
    outputBuffer[7] = (arg << 56) >> 56; // left shift for 56 bits | then right shift for 56 bits
}

inline void fillMessagesSchedulePart1(uchar *message, uint32 *schedule) {
    for (uchar i = 0; i < 16; i++) {
        schedule[i] = uint32(message[i * 4]) << 24 | uint32(message[i * 4 + 1]) << 16 | uint32(message[i * 4 + 2]) << 8 | uint32(message[i * 4 + 3]);
    }
}

inline void fillMessagesSchedulePart2(uint32 *messagesSchedule) {
    for (uchar i = 16; i < 64; i++) {
        uint32 s0 = (rightRotate(messagesSchedule[i - 15], 7)) ^ (rightRotate(messagesSchedule[i - 15], 18)) ^ (messagesSchedule[i - 15] >> 3);
        uint32 s1 = (rightRotate(messagesSchedule[i - 2], 17)) ^ (rightRotate(messagesSchedule[i - 2], 19)) ^ (messagesSchedule[i - 2] >> 10);
        messagesSchedule[i] = messagesSchedule[i - 16] + s0 + messagesSchedule[i - 7] + s1;
    }
}

inline void fillChunk(uchar *chunk, istream &stream) {
    stream.read((char *)chunk, CHUNK_SIZE_IN_BYTES);
}

inline void processChunk(uchar *chunk, uint32 *w, uint32 *k, uint32 *h) {
    // Copy the original message to the schedule
    fillMessagesSchedulePart1(chunk, w);

    // Add the 48 more words, according to the algorithm
    fillMessagesSchedulePart2(w);

    // Start compressing the data according to the algorithm
    uint32 a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], _h = h[7];
    for (uchar i = 0; i < 64; i++) {
        uint32 S1 = (rightRotate(e, 6)) ^ (rightRotate(e, 11)) ^ (rightRotate(e, 25));
        uint32 ch = (e & f) ^ (~e & g);
        uint32 temp1 = _h + S1 + ch + k[i] + w[i];
        uint32 S0 = (rightRotate(a, 2)) ^ (rightRotate(a, 13)) ^ (rightRotate(a, 22));
        uint32 maj = (a & b) ^ (a & c) ^ (b & c);
        uint32 temp2 = S0 + maj;
        _h = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
    }

    // Recalculate hash values for this particular chunk of data
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += _h;
}

inline string evaluateHash(istream &stream, uint64 &messageSize) {
    // Evaluate the count of iterations
    // Right shift for 6 bits is equivalent of division on 64 (2 ^ 6, 64 bytes, 512 bits).
    // It's safe to use bitwise shift (and not arithmetical one) since
    // the size of the message is unsigened (it can't be negative)
    uint64 iterations = messageSize >> 6;
    uint64 messageSizeInBits = messageSize << 3; // The equivalent of multiplication on 8 (2 ^ 3)

    // Initialize the hash values
    uint32 h[] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

    uint32 k[] = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
    };

    uchar chunk[CHUNK_SIZE_IN_BYTES];
    // Schedule of messages (w)
    uint32 w[CHUNK_SIZE_IN_BYTES];

    // Iterate through the main part of message body
    for (; iterations > 0; iterations--) {
        fillChunk(chunk, stream);
        processChunk(chunk, w, k, h);
    }

    // Process the final chunk(s) of data
    uchar bytesLeft = messageSize % CHUNK_SIZE_IN_BYTES;
    stream.read((char *)chunk, bytesLeft);

    // 1 set bit and 7 unset bits
    chunk[bytesLeft] = 0x80; // == 128, == 0b1000000

    // // We have to determine do we have enough space in the final chunk to fill it
    // // with at least one set (1) bit and 64 bits that contains the length of the original message
    // // The normal chunk size is 64 bytes. We need at least 9 of them (1 for a set bit and 8 for the length)
    // // So the `bytesLeft` value shouldn't exceed 64 - 9 = 55
    uchar i = bytesLeft + 1;
    if (bytesLeft <= 55) {
        // Fill the rest part of chunk with the zeroes
        for (; i < 56; i++) {
            chunk[i] = 0;
        }
    } else {
        // Fill the rest part of chunk with the zeroes
        for (; i < 64; i++) {
            chunk[i] = 0;
        }

        // Process pre-final chunk
        processChunk(chunk, w, k, h);

        // Fill the final chunk
        for (i = 0; i < 56; i++) {
            chunk[i] = 0;
        }
    }

    // Add the 64 bits at the end of the message that is represents the length of input message in bits
    unpackUint64(messageSizeInBits, chunk + 56);

    processChunk(chunk, w, k, h);

    return toHex(vector<uint32> { h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7] }, false);
}

namespace sha2 {

    string sha256(string filePath) {
        // Check if file exists
        if (!fs::exists(filePath)) {
            throw invalid_argument("File not found" + filePath);
        }

        // Get the size of file (message)
        uint64 messageSize = fs::file_size(filePath);

        if (messageSize > MAX_MESSAGE_SIZE) {
            stringstream exceptionMessageBuilder;
            exceptionMessageBuilder <<
                "File exceeds maximum allowed size:\n\t" <<
                "Expected: less or equals " << (MAX_MESSAGE_SIZE) << "bytes\n\t" <<
                "Actual: " << messageSize;
            throw invalid_argument(exceptionMessageBuilder.str());
        }

        // Opening the file
        ifstream fileStream;
        fileStream.open(filePath);
        string result = evaluateHash(fileStream, messageSize);
        fileStream.close();

        return result;
    }

}
