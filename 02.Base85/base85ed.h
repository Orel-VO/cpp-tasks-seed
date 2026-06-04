#ifndef __BASE85ED__
#define __BASE85ED__

#include <vector>
#include <cstdint>
#include <stdexcept>

namespace base85
{

/**
 * @brief Encodes bytes into Ascii85 (Adobe version).
 *
 * @param bytes Raw input bytes
 * @return Base85-encoded string as bytes (ASCII characters 33-117)
 * @throws std::invalid_argument if input is invalid
 */
std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes);

/**
 * @brief Decodes Ascii85 back to raw bytes.
 *
 * @param b85str Base85-encoded data (ASCII characters)
 * @return Decoded raw bytes
 * @throws std::invalid_argument on invalid Base85 input
 */
std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str);

} // namespace base85

#endif // __BASE85ED__
