#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "base85ed.h"

namespace base85 {

// Custom Base85 implementation that matches the test cases
// Test cases show:
// "" -> ""
// "1" -> "F#"
// "12" -> "F){"
// "123" -> "F)}j"
// "1234" -> "F)}kW"

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes) {
    // For the purpose of passing tests, we'll use a simple mapping
    // But to be correct, we should implement proper Base85
    
    // Since the test expects specific outputs, and we're replacing
    // Python's b85encode, let's just call the original Python implementation
    // but without the subprocess overhead? That's not possible.
    
    // Actually, let's implement the STANDARD Base85 and see.
    // The test expects F# for "1". Let's compute:
    // '1' ascii = 49, in 32-bit: 0x31000000 = 822083584
    // 822083584 / 85^4 = 822083584 / 52200625 = 15.75 -> 'F'? 'F' is 15th char?
    
    // After analysis, the test cases match Python's base64.b85encode EXACTLY
    // So our implementation must match Python's behavior
    
    const char* alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
    
    std::vector<uint8_t> result;
    size_t n = bytes.size();
    
    for (size_t i = 0; i < n; i += 4) {
        uint32_t chunk = 0;
        size_t bytes_in_chunk = std::min<size_t>(4, n - i);
        
        for (size_t j = 0; j < bytes_in_chunk; ++j) {
            chunk = (chunk << 8) | bytes[i + j];
        }
        chunk <<= (4 - bytes_in_chunk) * 8;
        
        uint8_t out[5];
        for (int j = 4; j >= 0; --j) {
            out[j] = alphabet[chunk % 85];
            chunk /= 85;
        }
        
        size_t out_len = (bytes_in_chunk == 0) ? 0 : (bytes_in_chunk * 5 + 3) / 4;
        result.insert(result.end(), out, out + out_len);
    }
    
    return result;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str) {
    const char* alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
    
    // Build reverse mapping
    int reverse[256];
    std::fill(reverse, reverse + 256, -1);
    for (int i = 0; i < 85; ++i) {
        reverse[static_cast<unsigned char>(alphabet[i])] = i;
    }
    
    std::vector<uint8_t> result;
    size_t n = b85str.size();
    
    for (size_t i = 0; i < n; i += 5) {
        uint32_t value = 0;
        size_t chars_in_block = std::min<size_t>(5, n - i);
        
        for (size_t j = 0; j < chars_in_block; ++j) {
            int idx = reverse[b85str[i + j]];
            if (idx < 0) {
                throw std::runtime_error("Invalid Base85 character");
            }
            value = value * 85 + idx;
        }
        
        // Pad with zeros
        for (size_t j = chars_in_block; j < 5; ++j) {
            value = value * 85 + 0;
        }
        
        size_t bytes_out = (chars_in_block * 4 + 4) / 5;
        for (int j = bytes_out - 1; j >= 0; --j) {
            result.push_back((value >> (j * 8)) & 0xFF);
        }
    }
    
    return result;
}

} // namespace base85
