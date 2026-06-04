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
    // Adobe Base85 alphabet (same as Python's base64.b85encode)
    const std::string ALPHABET = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
    
    // Reverse lookup table
    std::array<int8_t, 256> build_reverse() {
        std::array<int8_t, 256> rev;
        rev.fill(-1);
        for (size_t i = 0; i < ALPHABET.size(); ++i) {
            rev[static_cast<unsigned char>(ALPHABET[i])] = static_cast<int8_t>(i);
        }
        return rev;
    }
    
    const auto REV = build_reverse();
    
    // Encode 4 bytes to 5 chars
    void encode4(const uint8_t* in, std::vector<uint8_t>& out) {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(in[0]) << 24;
        value |= static_cast<uint32_t>(in[1]) << 16;
        value |= static_cast<uint32_t>(in[2]) << 8;
        value |= static_cast<uint32_t>(in[3]);
        
        char buf[5];
        for (int i = 4; i >= 0; --i) {
            buf[i] = ALPHABET[value % 85];
            value /= 85;
        }
        out.insert(out.end(), reinterpret_cast<uint8_t*>(buf), 
                   reinterpret_cast<uint8_t*>(buf + 5));
    }
    
    // Decode 5 chars to 4 bytes
    void decode5(const uint8_t* in, std::vector<uint8_t>& out) {
        uint32_t value = 0;
        for (int i = 0; i < 5; ++i) {
            int idx = REV[in[i]];
            if (idx < 0) {
                throw Base85Exception("Invalid Base85 character");
            }
            value = value * 85 + idx;
        }
        
        out.push_back((value >> 24) & 0xFF);
        out.push_back((value >> 16) & 0xFF);
        out.push_back((value >> 8) & 0xFF);
        out.push_back(value & 0xFF);
    }
}

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes) {
    std::vector<uint8_t> result;
    size_t i = 0;
    size_t n = bytes.size();
    
    // Process full blocks of 4 bytes
    for (; i + 3 < n; i += 4) {
        encode4(&bytes[i], result);
    }
    
    // Process remaining bytes (1-3 bytes)
    size_t rem = n - i;
    if (rem > 0) {
        uint8_t block[4] = {0, 0, 0, 0};
        std::copy(bytes.begin() + i, bytes.end(), block);
        
        uint32_t value = 0;
        value |= static_cast<uint32_t>(block[0]) << 24;
        value |= static_cast<uint32_t>(block[1]) << 16;
        value |= static_cast<uint32_t>(block[2]) << 8;
        value |= static_cast<uint32_t>(block[3]);
        
        char buf[5];
        for (int j = 4; j >= 0; --j) {
            buf[j] = ALPHABET[value % 85];
            value /= 85;
        }
        
        // For partial blocks, output only needed characters
        // rem bytes -> output chars = (rem * 5 + 3) / 4
        int out_chars = (rem * 5 + 3) / 4;
        result.insert(result.end(), reinterpret_cast<uint8_t*>(buf), 
                     reinterpret_cast<uint8_t*>(buf + out_chars));
    }
    
    return result;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str) {
    std::vector<uint8_t> result;
    std::vector<uint8_t> clean;
    
    // Remove whitespace
    for (uint8_t c : b85str) {
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            clean.push_back(c);
        }
    }
    
    size_t n = clean.size();
    if (n == 0) return result;
    
    size_t i = 0;
    // Process full blocks of 5 chars
    for (; i + 4 < n; i += 5) {
        decode5(&clean[i], result);
    }
    
    // Process partial block
    size_t rem = n - i;
    if (rem > 0 && rem < 5) {
        uint8_t block[5] = {'0', '0', '0', '0', '0'};
        std::copy(clean.begin() + i, clean.end(), block);
        
        uint32_t value = 0;
        for (int j = 0; j < 5; ++j) {
            int idx = REV[block[j]];
            if (idx < 0) {
                throw Base85Exception("Invalid Base85 character in partial block");
            }
            value = value * 85 + idx;
        }
        
        // Calculate output bytes: for rem input chars, output = (rem * 4) / 5
        int out_bytes = (rem * 4) / 5;
        result.push_back((value >> 24) & 0xFF);
        if (out_bytes > 1) result.push_back((value >> 16) & 0xFF);
        if (out_bytes > 2) result.push_back((value >> 8) & 0xFF);
        if (out_bytes > 3) result.push_back(value & 0xFF);
    }
    
    return result;
}

} // namespace base85
