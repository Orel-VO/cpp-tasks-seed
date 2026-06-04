#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <array>

#include "base85ed.h"

namespace base85
{

static constexpr uint32_t BASE = 85;
static constexpr uint8_t FIRST_CHAR = '!';
static constexpr uint8_t LAST_CHAR = 'u';

static const std::vector<uint8_t> ZERO_BLOCK = {0, 0, 0, 0};
static constexpr uint32_t MAGIC = 0x616c6646;

static void encode_block(const uint8_t* input, uint8_t* output)
{
    uint32_t value = 0;
    value |= static_cast<uint32_t>(input[0]) << 24;
    value |= static_cast<uint32_t>(input[1]) << 16;
    value |= static_cast<uint32_t>(input[2]) << 8;
    value |= static_cast<uint32_t>(input[3]);

    if (value == 0)
    {
        output[0] = 'z';
        return;
    }

    uint8_t chars[5];
    for (int i = 4; i >= 0; --i)
    {
        chars[i] = static_cast<uint8_t>(value % BASE);
        value /= BASE;
    }

    for (int i = 0; i < 5; ++i)
    {
        output[i] = chars[i] + FIRST_CHAR;
    }
}

static void decode_block(const uint8_t* input, uint8_t* output)
{
    if (input[0] == 'z')
    {
        for (int i = 0; i < 4; ++i)
        {
            output[i] = 0;
        }
        return;
    }

    uint32_t value = 0;
    for (int i = 0; i < 5; ++i)
    {
        if (input[i] < FIRST_CHAR || input[i] > LAST_CHAR)
        {
            throw std::invalid_argument("Invalid Base85 character");
        }
        value = value * BASE + (input[i] - FIRST_CHAR);
    }

    output[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    output[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    output[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    output[3] = static_cast<uint8_t>(value & 0xFF);
}

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes)
{
    std::vector<uint8_t> result;
    result.reserve((bytes.size() + 3) / 4 * 5 + 4);

    size_t i = 0;
    size_t n = bytes.size();
    while (i + 4 <= n)
    {
        bool all_zero = true;
        for (size_t j = 0; j < 4; ++j)
        {
            if (bytes[i + j] != 0)
            {
                all_zero = false;
                break;
            }
        }

        if (all_zero)
        {
            result.push_back('z');
        }
        else
        {
            uint8_t block[4];
            std::copy(bytes.begin() + i, bytes.begin() + i + 4, block);
            uint8_t encoded[5];
            encode_block(block, encoded);
            result.insert(result.end(), encoded, encoded + 5);
        }
        i += 4;
    }

    size_t remaining = n - i;
    if (remaining > 0)
    {
        uint8_t block[4] = {0, 0, 0, 0};
        std::copy(bytes.begin() + i, bytes.end(), block);

        uint8_t encoded[5];
        encode_block(block, encoded);

        if (remaining == 1)
        {
            result.insert(result.end(), encoded, encoded + 2);
        }
        else if (remaining == 2)
        {
            result.insert(result.end(), encoded, encoded + 3);
        }
        else if (remaining == 3)
        {
            result.insert(result.end(), encoded, encoded + 4);
        }
    }

    result.push_back('~');
    result.push_back('>');

    return result;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str)
{
    if (b85str.size() < 2)
    {
        throw std::invalid_argument("Invalid Base85 string: too short");
    }

    if (b85str[b85str.size() - 2] != '~' || b85str[b85str.size() - 1] != '>')
    {
        throw std::invalid_argument("Invalid Base85 string: missing '~>' suffix");
    }

    std::vector<uint8_t> result;
    result.reserve((b85str.size() - 2) / 5 * 4);

    size_t i = 0;
    size_t n = b85str.size() - 2;

    while (i < n)
    {
        uint8_t c = b85str[i];

        if (c == 'z')
        {
            if (i + 1 < n && b85str[i + 1] == 'z')
            {
                throw std::invalid_argument("Invalid Base85: consecutive 'z'");
            }
            result.insert(result.end(), 4, 0);
            ++i;
            continue;
        }

        if (i + 5 > n)
        {
            size_t remaining = n - i;
            if (remaining < 2)
            {
                throw std::invalid_argument("Invalid Base85: incomplete block");
            }

            uint8_t block[5] = {FIRST_CHAR, FIRST_CHAR, FIRST_CHAR, FIRST_CHAR, FIRST_CHAR};
            for (size_t j = 0; j < remaining; ++j)
            {
                block[j] = b85str[i + j];
            }

            uint8_t decoded[4] = {0, 0, 0, 0};
            try
            {
                decode_block(block, decoded);
            }
            catch (const std::invalid_argument& e)
            {
                throw;
            }

            if (remaining == 2)
            {
                result.push_back(decoded[0]);
            }
            else if (remaining == 3)
            {
                result.push_back(decoded[0]);
                result.push_back(decoded[1]);
            }
            else if (remaining == 4)
            {
                result.push_back(decoded[0]);
                result.push_back(decoded[1]);
                result.push_back(decoded[2]);
            }
            break;
        }

        uint8_t block[5];
        bool has_z = false;
        for (size_t j = 0; j < 5; ++j)
        {
            block[j] = b85str[i + j];
            if (block[j] == 'z')
            {
                has_z = true;
            }
        }

        if (has_z)
        {
            throw std::invalid_argument("Invalid Base85: 'z' inside 5-char block");
        }

        uint8_t decoded[4];
        decode_block(block, decoded);
        result.insert(result.end(), decoded, decoded + 4);
        i += 5;
    }

    return result;
}

} // namespace base85
