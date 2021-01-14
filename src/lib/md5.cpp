#include "../include/lib/md5.hpp"

#include <vector>
#include <istream>

#include "../include/utils/hexadecimal.hpp"
#include "../include/utils/fileValidator.hpp"

namespace md {

    namespace _md5 {
        const uchar CHUNK_SIZE_IN_BYTES = 64;
        // 0 means that the MD5 algorithm does not have any limitations around the size of the input message.
        // However this program can't handle the message longer than 2 ^ 64 - 1 bytes
        // due to the istream interface and the file systems limitations.
        const uint64 MAX_MESSAGE_SIZE = 0;

        // Use with care: don't pass the number higher than 31 as the `shift` value.
        // This function don't perform any edge-case checks
        inline uint32 leftRotate(uint32 original, uchar shift) {
            return (original << shift) | (original >> (32 - shift));
        }

        inline void packBlocksFromChunks(uchar *chunk, uint32* blocks) {
            // Little endian
            for (uchar i = 0; i < 16; i++) {
                blocks[i] = chunk[i * 4 + 3] << 24 | chunk[i * 4 + 2] << 16 | chunk[i * 4 + 1] << 8 | chunk[i * 4];
            }
        }

        inline void unpackUint64(uint64 arg, uchar *outputBuffer) {
            // Little endian
            outputBuffer[7] = arg >> 56; // right shift for 56 bits
            outputBuffer[6] = (arg << 8) >> 56; // left shift for 8 bits | then right shift for 56 bits
            outputBuffer[5] = (arg << 16) >> 56; // left shift for 16 bits | then right shift for 56 bits
            outputBuffer[4] = (arg << 24) >> 56; // left shift for 24 bits | then right shift for 56 bits
            outputBuffer[3] = (arg << 32) >> 56; // left shift for 32 bits | then right shift for 56 bits
            outputBuffer[2] = (arg << 40) >> 56; // left shift for 40 bits | then right shift for 56 bits
            outputBuffer[1] = (arg << 48) >> 56; // left shift for 48 bits | then right shift for 56 bits
            outputBuffer[0] = (arg << 56) >> 56; // left shift for 56 bits | then right shift for 56 bits
        }

        inline void fillChunk(uchar *chunk, istream &stream) {
            stream.read((char *)chunk, CHUNK_SIZE_IN_BYTES);
        }

        inline void processChunk(uchar *chunk, uint32 &A, uint32 &B, uint32 &C, uint32 &D) {
            // Those values indicates the shift values for each operation
            uchar s[] = {
                7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
            };

            uint32 K[] = {
                0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
                0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE, 0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
                0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
                0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED, 0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
                0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
                0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05, 0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
                0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
                0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
            };

            uint32 M[16];
            packBlocksFromChunks(chunk, M); // Break chunk into sixteen 32-bit words

            // Initialize variables for the current chunk
            uint32 a0 = A, b0 = B, c0 = C, d0 = D;

            // Rounds
            for (uchar i = 0; i < 64; i++) {
                uint32 F, g;
                if (i < 16) {
                    F = (b0 & c0) | (~b0 & d0); g = i; // Round 1
                } else if (i < 32) {
                    F = (d0 & b0) | (~d0 & c0); g = (5 * i + 1) % 16; // Round 2
                } else if (i < 48) {
                    F = b0 ^ c0 ^ d0; g = (3 * i + 5) % 16; // Round 3
                } else {
                    F = c0 ^ (b0 | ~d0); g = (7 * i) % 16; // Round 4
                }

                F += a0 + K[i] + M[g]; a0 = d0; d0 = c0; c0 = b0; // Update intermediate buffers
                b0 = b0 + leftRotate(F, s[i]); // Perform shift
            }
            // Update hash buffers
            A += a0; B += b0; C += c0; D += d0;
        }
    }

    inline string md5FromStream(istream &stream, uint64 messageSize) {
        uint64 iterations = messageSize >> 6; // Evaluate the count of iterations (2 ^ 6 == 64 (bits))
        uint64 messageSizeInBits = messageSize << 3;

        uint32 A = 0x67452301;
        uint32 B = 0xEFCDAB89;
        uint32 C = 0x98BADCFE;
        uint32 D = 0x10325476;

        uchar chunk[_md5::CHUNK_SIZE_IN_BYTES];

        // Iterate through the main part of message body
        for (; iterations > 0; iterations--) {
            _md5::fillChunk(chunk, stream);
            _md5::processChunk(chunk, A, B, C, D);
        }

        // Process the final chunk(s) of data
        uchar bytesLeft = messageSize % _md5::CHUNK_SIZE_IN_BYTES;
        stream.read((char *)chunk, bytesLeft);

        // 1 set bit and 7 unset bits
        chunk[bytesLeft] = 0x80; // == 128, == 0b1000000

        // // We have to determine do we have enough space in the final chunk to fill it
        // // with at least one set (1) bit and 64 bits that contains the length of the original message
        // // The normal chunk size is 64 bytes. We need at least 9 of them (1 for a set bit and 8 for the length)
        // // So the `bytesLeft` value shouldn't exceed 64 - 9 = 55
        uchar i = bytesLeft + 1;
        if (bytesLeft < 56) {
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
            _md5::processChunk(chunk, A, B, C, D);

            // Fill the final chunk
            for (i = 0; i < 56; i++) {
                chunk[i] = 0;
            }
        }

        // Add the 64 bits at the end of the message that is represents the length of input message in bits
        _md5::unpackUint64(messageSizeInBits, chunk + 56);

        _md5::processChunk(chunk, A, B, C, D);

        return toHex(vector<uint32> { A, B, C, D }, true, false);
    }

    string md5(string filePath) {
        return validateFileForHashFunction(filePath, _md5::MAX_MESSAGE_SIZE, md5FromStream);
    }

}
