#ifndef __BASE85ED__
#define __BASE85ED__

#include <vector>
#include <cstdint>
#include <stdexcept>

/**
 * @namespace base85
 * @brief Utilities for Base85 <en/de>coding of bytes
 */
namespace base85
{

/**
 * @brief Exception thrown for Base85 encoding/decoding errors
 */
class Base85Exception : public std::runtime_error {
public:
    explicit Base85Exception(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * @brief Encodes bytes into a Base85 (Adobe version, similar to base64.b85encode).
 *
 * @param[in] bytes A vector containing the raw bytes.
 * @return std::vector<uint8_t> A vector of Base85-encoded bytes.
 * @throws Base85Exception if encoding fails
 *
 * @see decode()
 */
std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes);

/**
 * @brief Decodes Base85 to bytes (Adobe version).
 *
 * @param[in] b85str A vector containing Base85-encoded data.
 * @return std::vector<uint8_t> The decoded bytes.
 * @throws Base85Exception if decoding fails (invalid input, wrong length, etc.)
 *
 * @see encode()
 */
std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str);

} // namespace base85

#endif // __BASE85ED__
