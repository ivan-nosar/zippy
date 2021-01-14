#include "../include/lib/sha512.hpp"

#include <vector>
#include <istream>

#include "../include/utils/hexadecimal.hpp"
#include "../include/utils/fileValidator.hpp"

namespace sha2 {

    namespace _sha512 {
        const uchar CHUNK_SIZE_IN_BYTES = 128;
        const uchar ROUNDS_COUNT = 80;
        // The maximum allowed message size is 2 ^ 64 - 1 bytes - due to file system and x64 architecture limitations
        const uint64 MAX_MESSAGE_SIZE = 0xFFFFFFFFFFFFFFFF; // 18446744073709551615 == 2 ^ 64 - 1

        // Use with care: don't pass the number higher than 63 as the `shift` value.
        // This function don't perform any edge-case checks
        inline uint64 rightRotate(uint64 original, uchar shift) {
            return (original << (64 - shift)) | (original >> shift);
        }

        inline uint64 packUint64(uchar a, uchar b, uchar c, uchar d, uchar e, uchar f, uchar g, uchar h) {
            return
                uint64(a) << 56 | uint64(b) << 48 | uint64(c) << 40 | uint64(d) << 32 |
                uint64(e) << 24 | uint64(f) << 16 | uint64(g) << 8 | h;
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

        inline void fillMessagesSchedulePart1(uchar *message, uint64 *schedule) {
            for (uchar i = 0; i < 16; i++) {
                schedule[i] =
                    uint64(message[i * 8]) << 56 | uint64(message[i * 8 + 1]) << 48 | uint64(message[i * 8 + 2]) << 40 | uint64(message[i * 8 + 3]) << 32 |
                    uint64(message[i * 8 + 4]) << 24 | uint64(message[i * 8 + 5]) << 16 | uint64(message[i * 8 + 6]) << 8 | uint64(message[i * 8 + 7]);
            }
        }

        inline void fillMessagesSchedulePart2(uint64 *messagesSchedule) {
            for (uchar i = 16; i < ROUNDS_COUNT; i++) {
                uint64 s0 = (rightRotate(messagesSchedule[i - 15], 1)) ^ (rightRotate(messagesSchedule[i - 15], 8)) ^ (messagesSchedule[i - 15] >> 7);
                uint64 s1 = (rightRotate(messagesSchedule[i - 2], 19)) ^ (rightRotate(messagesSchedule[i - 2], 61)) ^ (messagesSchedule[i - 2] >> 6);
                messagesSchedule[i] = messagesSchedule[i - 16] + s0 + messagesSchedule[i - 7] + s1;
            }
        }

        inline void fillChunk(uchar *chunk, istream &stream) {
            stream.read((char *)chunk, CHUNK_SIZE_IN_BYTES);
        }

        inline void processChunk(uchar *chunk, uint64 *h) {
            uint64 k[] = {
                0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC,
                0x3956C25BF348B538, 0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118,
                0xD807AA98A3030242, 0x12835B0145706FBE, 0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2,
                0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235, 0xC19BF174CF692694,
                0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
                0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5,
                0x983E5152EE66DFAB, 0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4,
                0xC6E00BF33DA88FC2, 0xD5A79147930AA725, 0x06CA6351E003826F, 0x142929670A0E6E70,
                0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED, 0x53380D139D95B3DF,
                0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B,
                0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30,
                0xD192E819D6EF5218, 0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8,
                0x19A4C116B8D2D0C8, 0x1E376C085141AB53, 0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8,
                0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373, 0x682E6FF3D6B2B8A3,
                0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
                0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B,
                0xCA273ECEEA26619C, 0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178,
                0x06F067AA72176FBA, 0x0A637DC5A2C898A6, 0x113F9804BEF90DAE, 0x1B710B35131C471B,
                0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC, 0x431D67C49C100D4C,
                0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817,
            };

            // Schedule of messages (w)
            uint64 w[ROUNDS_COUNT];

            // Copy the original message to the schedule
            fillMessagesSchedulePart1(chunk, w);

            // Add the 48 more words, according to the algorithm
            fillMessagesSchedulePart2(w);

            // Start compressing the data according to the algorithm
            uint64 a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], _h = h[7];
            for (uchar i = 0; i < ROUNDS_COUNT; i++) {
                uint64 S1 = (rightRotate(e, 14)) ^ (rightRotate(e, 18)) ^ (rightRotate(e, 41));
                uint64 ch = (e & f) ^ (~e & g);
                uint64 temp1 = _h + S1 + ch + k[i] + w[i];
                uint64 S0 = (rightRotate(a, 28)) ^ (rightRotate(a, 34)) ^ (rightRotate(a, 39)); // Sum0(a)
                uint64 maj = (a & b) ^ (a & c) ^ (b & c);
                uint64 temp2 = S0 + maj;
                _h = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
            }

            // Recalculate hash values for this particular chunk of data
            h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += _h;
        }
    }

    inline string sha512FromStream(istream &stream, uint64 messageSize) {
        // Evaluate the count of iterations
        // Right shift for 7 bits is equivalent of division on 128 (2 ^ 7, 128 bytes, 1024 bits).
        // It's safe to use bitwise shift (and not arithmetical one) since
        // the size of the message is unsigened (it can't be negative)
        uint64 iterations = messageSize >> 7;

        // Initialize the hash values
        uint64 h[] = {
            0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
            0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
        };

        uchar chunk[_sha512::CHUNK_SIZE_IN_BYTES];

        // Iterate through the main part of message body
        for (; iterations > 0; iterations--) {
            _sha512::fillChunk(chunk, stream);
            _sha512::processChunk(chunk, h);
        }

        // Process the final chunk(s) of data
        uchar bytesLeft = messageSize % _sha512::CHUNK_SIZE_IN_BYTES;
        stream.read((char *)chunk, bytesLeft);

        // 1 set bit and 7 unset bits
        chunk[bytesLeft] = 0x80; // == 128, == 0b1000000

        // // We have to determine do we have enough space in the final chunk to fill it
        // // with at least one set (1) bit and 64 bits that contains the length of the original message
        // // The normal chunk size is 64 bytes. We need at least 9 of them (1 for a set bit and 8 for the length)
        // // So the `bytesLeft` value shouldn't exceed 64 - 9 = 55
        uchar i = bytesLeft + 1;
        if (bytesLeft < 112) {
            // Fill the rest part of chunk with the zeroes
            for (; i < 112; i++) {
                chunk[i] = 0;
            }
        } else {
            // Fill the rest part of chunk with the zeroes
            for (; i < 128; i++) {
                chunk[i] = 0;
            }

            // Process pre-final chunk
            _sha512::processChunk(chunk, h);

            // Fill the final chunk
            for (i = 0; i < 112; i++) {
                chunk[i] = 0;
            }
        }

        // Add the 128 bits at the end of the message that is represents the length of input message in bits
        _sha512::unpackUint64(messageSize >> 61, chunk + 112); // Get the first 3 bits of 64-bits value
        _sha512::unpackUint64(messageSize << 3, chunk + 120); // Get the last 61 bits of 64-bits value

        _sha512::processChunk(chunk, h);

        return toHex(vector<uint64> { h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7] });
    }

    string sha512(string filePath) {
        return validateFileForHashFunction(filePath, _sha512::MAX_MESSAGE_SIZE, sha512FromStream);
    }

}
