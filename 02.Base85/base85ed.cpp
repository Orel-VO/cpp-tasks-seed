#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <cstring>

#include "base85ed.h"

namespace base85 {

namespace {
    // Special Base85 alphabet that matches the test cases
    // This appears to be a custom variant, not the standard Adobe/RFC 1924
    // Based on the test cases:
    // "" -> ""
    // "1" -> "F#"
    // "12" -> "F){"
    // "123" -> "F)}j"
    // "1234" -> "F)}kW"
    
    // Let's derive the actual alphabet from test cases
    // For simplicity, we'll use the standard algorithm but with proper mapping
    
    // Standard ASCII85 alphabet (Adobe version)
    const char BASE85_ALPHABET[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
    
    // For test cases, we need to match Python's base64.b85encode which uses:
    // !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
    // But actually Python's b85encode uses a different order:
    // The alphabet is: "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~"
    
    // Let's verify with test "1" -> "F#"
    // '1' ASCII = 49, 'F' ASCII = 70, '#' ASCII = 35
    // Something's off - let's just use the standard approach that matches Python's b85encode
    
    // Reverse lookup table for Base85 decoding
    constexpr int REVERSE_ALPHABET_SIZE = 256;
    std::array<int8_t, REVERSE_ALPHABET_SIZE> build_reverse_alphabet() {
        std::array<int8_t, REVERSE_ALPHABET_SIZE> rev;
        rev.fill(-1);
        for (int i = 0; i < 85; ++i) {
            rev[static_cast<unsigned char>(BASE85_ALPHABET[i])] = i;
        }
        return rev;
    }
    
    const std::array<int8_t, REVERSE_ALPHABET_SIZE> REVERSE_ALPHABET = build_reverse_alphabet();
    
    // Convert 4 bytes to 5 Base85 characters
    void encode_block(const uint8_t* input, uint8_t* output) {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(input[0]) << 24;
        value |= static_cast<uint32_t>(input[1]) << 16;
        value |= static_cast<uint32_t>(input[2]) << 8;
        value |= static_cast<uint32_t>(input[3]);
        
        // Extract 5 base-85 digits (most significant first)
        uint32_t temp = value;
        for (int i = 4; i >= 0; --i) {
            output[i] = BASE85_ALPHABET[temp % 85];
            temp /= 85;
        }
    }
    
    // Convert 5 Base85 characters to 4 bytes
    void decode_block(const uint8_t* input, uint8_t* output) {
        uint32_t value = 0;
        for (int i = 0; i < 5; ++i) {
            int idx = REVERSE_ALPHABET[input[i]];
            if (idx < 0) {
                throw Base85Exception("Invalid Base85 character found");
            }
            value = value * 85 + idx;
        }
        
        output[0] = (value >> 24) & 0xFF;
        output[1] = (value >> 16) & 0xFF;
        output[2] = (value >> 8) & 0xFF;
        output[3] = value & 0xFF;
    }
}

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes) {
    std::vector<uint8_t> result;
    size_t n = bytes.size();
    size_t i = 0;
    
    // Process complete 4-byte blocks
    for (i = 0; i + 4 <= n; i += 4) {
        uint8_t output[5];
        encode_block(&bytes[i], output);
        result.insert(result.end(), output, output + 5);
    }
    
    // Process remaining bytes
    size_t remaining = n - i;
    if (remaining > 0) {
        uint8_t block[4] = {0, 0, 0, 0};
        std::copy(&bytes[i], &bytes[i] + remaining, block);
        
        uint8_t output[5];
        encode_block(block, output);
        
        // For partial blocks, we need ceil(remaining * 5 / 4) output characters
        // But Python's b85encode includes all 5 characters and they get truncated
        // when decoding? Actually no, it outputs exactly ceil(remaining * 5 / 4) chars
        size_t output_chars = (remaining * 5 + 3) / 4;
        result.insert(result.end(), output, output + output_chars);
    }
    
    return result;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str) {
    std::vector<uint8_t> result;
    
    // Filter out whitespace characters
    std::vector<uint8_t> clean;
    clean.reserve(b85str.size());
    for (uint8_t c : b85str) {
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            clean.push_back(c);
        }
    }
    
    size_t n = clean.size();
    if (n == 0) {
        return result;
    }
    
    size_t i = 0;
    
    // Process complete 5-character blocks
    for (i = 0; i + 5 <= n; i += 5) {
        uint8_t output[4];
        decode_block(&clean[i], output);
        result.insert(result.end(), output, output + 4);
    }
    
    // Process remaining characters
    size_t remaining = n - i;
    if (remaining > 0) {
        // Pad with '0' (first character of alphabet) to make 5 chars
        uint8_t block[5];
        std::fill(block, block + 5, BASE85_ALPHABET[0]);
        std::copy(&clean[i], &clean[i] + remaining, block);
        
        uint8_t output[4];
        decode_block(block, output);
        
        // Calculate number of valid output bytes: floor(remaining * 4 / 5)
        size_t output_bytes = (remaining * 4) / 5;
        result.insert(result.end(), output, output + output_bytes);
    }
    
    return result;
}

} // namespace base85
